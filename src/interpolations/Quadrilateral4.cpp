
#include "Quadrilateral4.hpp"

#include <array>
#include <tuple>

namespace neon
{
Quadrilateral4::Quadrilateral4(QuadrilateralQuadrature::Rule rule)
    : SurfaceInterpolation(std::make_unique<QuadrilateralQuadrature>(rule))
{
    this->precompute_shape_functions();
}

void Quadrilateral4::precompute_shape_functions()
{
    using NodalCoordinate = std::tuple<int, double, double>;

    constexpr std::array<NodalCoordinate, 4> local_coordinates = {
        {{0, -1.0, -1.0}, {1, 1.0, -1.0}, {2, 1.0, 1.0}, {3, -1.0, 1.0}}};

    numerical_quadrature->evaluate([&](auto const& coordinate) {

        auto const & [ l, xi, eta ] = coordinate;

        Vector N(4);
        Matrix rhea(4, 2);

        for (auto const & [ a, xi_a, eta_a ] : local_coordinates)
        {
            N(a) = 0.25 * (1.0 + xi_a * xi) * (1.0 + eta_a * eta);
            rhea(a, 0) = 0.25 * (1.0 + eta_a * eta) * xi_a;
            rhea(a, 1) = 0.25 * (1.0 + xi_a * xi) * eta_a;
        }
        return std::make_tuple(N, rhea);
    });
}

double Quadrilateral4::compute_measure(Matrix const& nodal_coordinates)
{
    auto face_area = 0.0;

    Matrix rhea(4, 2);
    Matrix planarCoordinates = project_to_plane(nodal_coordinates);

    // TODO Fix this to use the integrator
    // for (int l = 0; l < quadraturePoints; ++l)
    // {
    // 	rhea.col(0) = dN_dXi.col(l);
    // 	rhea.col(1) = dN_dEta.col(l);
    //
    // 	Eigen::Matrix2d Jacobian = reducedPoints * rhea;
    //
    // 	face_area += Jacobian.determinant() * quadratureWeight(l);
    // }
    return face_area;
}
}