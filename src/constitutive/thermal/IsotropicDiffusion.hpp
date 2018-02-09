
#pragma once

#include "constitutive/ConstitutiveModel.hpp"

#include "material/linear_diffusion.hpp"

namespace neon::diffusion
{
/**
 * IsotropicDiffusion computes the isotropic constitutive matrix for linear
 * and isotropic diffusion problems.
 */
class IsotropicDiffusion : public diffusion::ConstitutiveModel
{
public:
    IsotropicDiffusion(std::shared_ptr<InternalVariables>& variables,
                       json const& material_data);

    void update_internal_variables(double const time_step_size) override;

    material_property const& intrinsic_material() const override { return material; }

    bool is_finite_deformation() const override { return false; }

protected:
    linear_diffusion material;
};
}
