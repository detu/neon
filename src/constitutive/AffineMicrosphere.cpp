
#include "AffineMicrosphere.hpp"

#include "InternalVariables.hpp"
#include "numeric/DenseTypes.hpp"

#include <json/json.h>
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>

#include <exception>
#include <iostream>
#include <omp.h>

namespace neon
{
AffineMicrosphere::AffineMicrosphere(InternalVariables& variables, Json::Value const& material_data)
    : Hyperelastic(variables), material(material_data)
{
    variables.add(InternalVariables::Matrix::TruesdellModuli, 6);

    // Deviatoric stress
    variables.add(InternalVariables::Tensor::Kirchhoff);

    variables.add(InternalVariables::Scalar::Chains, InternalVariables::Scalar::ShearModuli);

    // Shrink these down to the correct size
    variables(InternalVariables::Scalar::Chains).resize(material.groups(), 0.0);
    variables(InternalVariables::Scalar::ShearModuli).resize(material.groups(), 0.0);
    variables(InternalVariables::Scalar::Chains).shrink_to_fit();
    variables(InternalVariables::Scalar::ShearModuli).shrink_to_fit();

    // Fill the data with material properties using the material class
    variables(InternalVariables::Scalar::Chains) = material.chain_groups();
    variables(InternalVariables::Scalar::ShearModuli) = material.shear_moduli_groups();

    // Commit these to history in case of failure on first time step
    variables.commit();
}

void AffineMicrosphere::update_internal_variables(double const time_step_size)
{
    using namespace ranges;
    using view::transform;
    using view::zip;

    // TODO Change these back once OpenMP allows structured bindings

    // Get references into the hash table
    auto& F_list = variables(InternalVariables::Tensor::DeformationGradient);
    auto& cauchy_stress_list = variables(InternalVariables::Tensor::Cauchy);
    auto& dev_stress_list = variables(InternalVariables::Tensor::Kirchhoff);

    auto const& detF_list = variables(InternalVariables::Scalar::DetF);

    auto& n_list = variables(InternalVariables::Scalar::Chains);
    auto& G_list = variables(InternalVariables::Scalar::ShearModuli);

    // Update the material properties
    n_list = material.update_chains(n_list, time_step_size);
    G_list = material.compute_shear_moduli(n_list);

    auto const K = material.bulk_modulus();

    /*----------------------------------------------------------------------------*
     *                          Stress computation                                *
     *----------------------------------------------------------------------------*/

#pragma omp parallel for
    for (auto l = 0; l < F_list.size(); ++l)
    {
        auto& stress_dev = dev_stress_list[l]; // Deviatoric stress
        auto const& F = F_list[l];             // Deformation gradient
        auto const& J = detF_list[l];          // Determinant of the deformation gradient

        Matrix3 const unimodular_F = std::pow(J, -1.0 / 3.0) * F;

        stress_dev = weighting(G_list, Matrix3::Zero().eval(), [&](auto const& N) -> Matrix3 {
            return compute_kirchhoff_stress(unimodular_F, N);
        });
    }

    // Perform the projection of the stresses
    cauchy_stress_list = zip(dev_stress_list, detF_list) | transform([&](auto const& tpl) -> Matrix3 {

                             auto const & [ cauchy_stress_dev, J ] = tpl;

                             auto const pressure = J * volumetric_free_energy_dJ(J, K);

                             return deviatoric_projection(pressure, cauchy_stress_dev) / J;
                         });

    /*------------------------------------------------------------------------*
     *                     Tangent material computation                       *
     *------------------------------------------------------------------------*/

    // Compute tangent moduli
    auto& D_list = variables(InternalVariables::Matrix::TruesdellModuli);

#pragma omp parallel for
    for (auto l = 0; l < F_list.size(); ++l)
    {
        auto const& F = F_list[l];
        auto const& cauchy_stress_dev = dev_stress_list[l];
        auto const& J = detF_list[l];

        auto const pressure = J * volumetric_free_energy_dJ(J, K);
        auto const kappa = std::pow(J, 2) * volumetric_free_energy_second_d2J(J, K);

        Matrix3 const unimodular_F = std::pow(J, -1.0 / 3.0) * F;

        CMatrix const Ddev = weighting(G_list,
                                       CMatrix::Zero(6, 6).eval(),
                                       [&](auto const& N) -> CMatrix {
                                           return compute_material_matrix(unimodular_F, N);
                                       });

        D_list[l] = deviatoric_projection(Ddev, cauchy_stress_dev) + (kappa + pressure) * IoI
                    - 2.0 * pressure * I;
    }
}

Matrix3 AffineMicrosphere::deviatoric_projection(double const p, Matrix3 const& cauchy_stress_dev) const
{
    auto const P = voigt::kinetic::deviatoric();

    return p * Matrix3::Identity() + voigt::kinetic::from(P * voigt::kinetic::to(cauchy_stress_dev));
}

CMatrix AffineMicrosphere::deviatoric_projection(CMatrix const& C_dev, Matrix3 const& stress_dev) const
{
    CMatrix const D = C_dev + 2.0 / 3.0 * stress_dev.trace() * voigt::kinematic::identity()
                      - 2.0 / 3.0
                            * (outer_product(stress_dev, Matrix3::Identity())
                               + outer_product(Matrix3::Identity(), stress_dev));

    return voigt::kinetic::deviatoric() * D * voigt::kinetic::deviatoric();
}

Matrix3 AffineMicrosphere::compute_kirchhoff_stress(Matrix3 const& unimodular_F, double const N) const
{
    return unit_sphere.integrate(Matrix3::Zero().eval(),
                                 [&](auto const& coordinates, auto const& l) -> Matrix3 {
                                     auto const & [ r, r_outer_r ] = coordinates;

                                     // Deformed tangents
                                     Vector3 const t = unimodular_F * r;

                                     // Microstretches
                                     auto const micro_stretch = t.norm();

                                     return pade_first(micro_stretch, N) * t * t.transpose();
                                 });
}

CMatrix AffineMicrosphere::compute_material_matrix(Matrix3 const& unimodular_F, double const N) const
{
    return unit_sphere
        .integrate(CMatrix::Zero(6, 6).eval(), [&](auto const& coordinates, auto const& l) -> CMatrix {
            auto const & [ r, r_outer_r ] = coordinates;

            // Deformed tangents
            auto const t = unimodular_F * r;

            // Microstretches
            auto const micro_stretch = t.norm();

            auto const a = std::pow(micro_stretch, -2)
                           * (pade_second(micro_stretch, N) - pade_first(micro_stretch, N));

            return a * outer_product(t * t.transpose(), (t * t.transpose()).transpose());
        });
}
}
