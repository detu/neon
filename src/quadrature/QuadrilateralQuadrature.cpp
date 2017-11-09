
#include "QuadrilateralQuadrature.hpp"

namespace neon
{
QuadrilateralQuadrature::QuadrilateralQuadrature(Rule rule)
{
    switch (rule)
    {
        case Rule::OnePoint:
        {
            w = {4.0};
            clist = {{0, 0.0, 0.0}};
            break;
        }
        case Rule::FourPoint:
        {
            w = {1.0, 1.0, 1.0, 1.0};

            clist = {{0, -1.0 / std::sqrt(3.0), -1.0 / std::sqrt(3.0)},
                     {1, 1.0 / std::sqrt(3.0), -1.0 / std::sqrt(3.0)},
                     {2, 1.0 / std::sqrt(3.0), 1.0 / std::sqrt(3.0)},
                     {3, -1.0 / std::sqrt(3.0), 1.0 / std::sqrt(3.0)}};
            break;
        }
        case Rule::NinePoint:
        {
            w = {25.0 / 81.0,
                 25.0 / 81.0,
                 25.0 / 81.0,
                 25.0 / 81.0,
                 40.0 / 81.0,
                 40.0 / 81.0,
                 40.0 / 81.0,
                 40.0 / 81.0,
                 64.0 / 81.0};

            auto const a = std::sqrt(3.0 / 5.0);

            clist = {{0, -a, -a},
                     {1, a, -a},
                     {2, a, a},
                     {3, -a, a},
                     {4, 0.0, -a},
                     {5, a, 0.0},
                     {6, 0.0, a},
                     {7, -a, 0.0},
                     {8, 0.0, 0.0}};
            break;
        }
    }
}
}
