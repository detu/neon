
#pragma once

/// @file

#include "shape_function.hpp"

namespace neon
{
/// hexahedron8 is responsible for computing the tri-linear shape functions for an
/// eight noded hexahedron element.
/// The shape functions and ordering is from \cite Hughes2012
/**
 * Initialize the shape functions to the following polynomials
 * \f{align*}{
 * N_1(\xi, \eta, \zeta) &= \frac{1}{8}(1-\xi)(1-\eta)(1-\zeta) \\
 * N_2(\xi, \eta, \zeta) &= \frac{1}{8}(1+\xi)(1-\eta)(1-\zeta) \\
 * N_3(\xi, \eta, \zeta) &= \frac{1}{8}(1+\xi)(1+\eta)(1-\zeta) \\
 * N_4(\xi, \eta, \zeta) &= \frac{1}{8}(1-\xi)(1+\eta)(1-\zeta) \\
 * N_5(\xi, \eta, \zeta) &= \frac{1}{8}(1-\xi)(1-\eta)(1+\zeta) \\
 * N_6(\xi, \eta, \zeta) &= \frac{1}{8}(1+\xi)(1-\eta)(1+\zeta) \\
 * N_7(\xi, \eta, \zeta) &= \frac{1}{8}(1+\xi)(1+\eta)(1+\zeta) \\
 * N_8(\xi, \eta, \zeta) &= \frac{1}{8}(1-\xi)(1+\eta)(1+\zeta)
 * \f}
 */
class hexahedron8 : public volume_interpolation
{
public:
    explicit hexahedron8();

    /// Evaluate the shape functions at the natural coordinate
    virtual auto evaluate(coordinate_type const& coordinate) const noexcept(false)
        -> value_type override final;
};

/// hexahedron20 is responsible for computing the quadratic shape functions for an
/// twenty noded hexahedron element.  Nodes are only defined on the midside
/// and corner nodes.  The node ordering is from @cite Hughes2012.
class hexahedron20 : public volume_interpolation
{
public:
    explicit hexahedron20();

    /// Evaluate the shape functions at the natural coordinate
    virtual auto evaluate(coordinate_type const& coordinate) const noexcept(false)
        -> value_type override final;
};

/// hexahedron27 is responsible for computing the quadratic shape functions for an
/// twenty-nine noded hexahedron element.  Nodes are also on the faces and the centre
/// of the reference cube.  The node ordering is from @cite Hughes2012.
class hexahedron27 : public volume_interpolation
{
public:
    explicit hexahedron27();

    /// Evaluate the shape functions at the natural coordinate
    virtual auto evaluate(coordinate_type const& coordinate) const noexcept(false)
        -> value_type override final;
};
}
