
#include "mesh/node_ordering_adapter.hpp"

#include "Exceptions.hpp"

#include <unordered_map>

namespace neon
{
std::unordered_map<int, element_topology> const gmsh_converter{{1, element_topology::line2},
                                                               {2, element_topology::triangle3},
                                                               {3, element_topology::quadrilateral4},
                                                               {4, element_topology::tetrahedron4},
                                                               {5, element_topology::hexahedron8},
                                                               {6, element_topology::prism6},
                                                               {8, element_topology::line3},
                                                               {9, element_topology::triangle6},
                                                               {11, element_topology::tetrahedron10},
                                                               {10, element_topology::quadrilateral9},
                                                               {12, element_topology::hexahedron27},
                                                               {13, element_topology::prism18},
                                                               {16, element_topology::quadrilateral8},
                                                               {18, element_topology::prism15},
                                                               {17, element_topology::hexahedron20}};

std::unordered_map<element_topology, VTKCellType> const
    vtk_converter{{element_topology::triangle3, VTK_TRIANGLE},
                  {element_topology::quadrilateral4, VTK_QUAD},
                  {element_topology::quadrilateral8, VTK_QUADRATIC_QUAD},
                  {element_topology::quadrilateral9, VTK_BIQUADRATIC_QUAD},
                  {element_topology::tetrahedron4, VTK_TETRA},
                  {element_topology::hexahedron8, VTK_HEXAHEDRON},
                  {element_topology::prism6, VTK_WEDGE},
                  {element_topology::triangle6, VTK_QUADRATIC_TRIANGLE},
                  {element_topology::tetrahedron10, VTK_QUADRATIC_TETRA},
                  {element_topology::prism15, VTK_QUADRATIC_WEDGE},
                  // {element_topology::prism18, VTK_TRIQUADRATIC_WEDGE},
                  {element_topology::hexahedron20, VTK_QUADRATIC_HEXAHEDRON},
                  {element_topology::hexahedron27, VTK_TRIQUADRATIC_HEXAHEDRON}};

void convert_from_gmsh(std::vector<local_indices>& nodal_connectivity, element_topology const topology)
{
    // Reorder based on the differences between the local node numbering
    // provided from Section 9.3 Node ordering
    // http://gmsh.info/doc/texinfo/gmsh.html#Node-ordering
    switch (topology)
    {
        case element_topology::tetrahedron10:
        {
            for (auto& nodal_list : nodal_connectivity)
            {
                std::swap(nodal_list.at(4), nodal_list.at(7));
                std::swap(nodal_list.at(4), nodal_list.at(5));
                std::swap(nodal_list.at(5), nodal_list.at(8));
                std::swap(nodal_list.at(8), nodal_list.at(9));
                std::swap(nodal_list.at(6), nodal_list.at(9));
            }
        }
        case element_topology::tetrahedron4:
        {
            for (auto& nodal_list : nodal_connectivity)
            {
                std::swap(nodal_list.at(0), nodal_list.at(3));
                std::swap(nodal_list.at(0), nodal_list.at(2));
                std::swap(nodal_list.at(0), nodal_list.at(1));
            }
            break;
        }
        case element_topology::prism6:
        {
            for (auto& nodal_list : nodal_connectivity)
            {
                std::swap(nodal_list.at(0), nodal_list.at(1));
                std::swap(nodal_list.at(3), nodal_list.at(4));
            }
            break;
        }
        case element_topology::prism15:
        {
            for (auto& nodal_list : nodal_connectivity)
            {
                // -1 face
                std::swap(nodal_list.at(0), nodal_list.at(1));
                std::swap(nodal_list.at(3), nodal_list.at(6));
                std::swap(nodal_list.at(7), nodal_list.at(4));
                std::swap(nodal_list.at(5), nodal_list.at(9));

                // mid face
                std::swap(nodal_list.at(8), nodal_list.at(7));
                std::swap(nodal_list.at(10), nodal_list.at(6));
                std::swap(nodal_list.at(11), nodal_list.at(8));

                // +1 face
                std::swap(nodal_list.at(11), nodal_list.at(9));
            }
            break;
        }
        case element_topology::hexahedron27:
        {
            /*
            Gmsh ordering (0 based indexing) taken from gmsh.info

                3----13----2
                |\         |\
                |15    24  | 14
                9  \ 20    11 \
                |   7----19+---6
                |22 |  26  | 23|
                0---+-8----1   |
                 \ 17    25 \  18
                 10 |  21    12|
                   \|         \|
                    4----16----5

            Hughes ordering (0 based indexing)

                3----10----2
                |\         |\
                | 19   23  | 18
               11  \ 20    9  \
                |   7----14+---6
                |24 |  26  | 25|
                0---+-8----1   |
                 \  15   21 \  13
                 16 |  22    17|
                   \|         \|
                    4----12----5

            */
            for (auto& nodal_list : nodal_connectivity)
            {
                std::swap(nodal_list.at(21), nodal_list.at(25));
                std::swap(nodal_list.at(25), nodal_list.at(22));
                std::swap(nodal_list.at(24), nodal_list.at(25));
                std::swap(nodal_list.at(23), nodal_list.at(25));
            }
            [[fallthrough]];
        }
        case element_topology::hexahedron20:
        {
            /*
              Gmsh ordering (0 based indexing) taken from gmsh.info

               3----13----2
               |\         |\
               | 15       | 14
               9  \       11 \
               |   7----19+---6
               |   |      |   |
               0---+-8----1   |
                \  17      \  18
                10 |        12|
                  \|         \|
                   4----16----5

              Hughes ordering (0 based indexing)

               3----10----2
               |\         |\
               | 19       | 18
              11  \       9  \
               |   7----14+---6
               |   |      |   |
               0---+-8----1   |
                \  15      \  13
                16 |        17|
                  \|         \|
                   4----12----5
            */
            for (auto& nodal_list : nodal_connectivity)
            {
                std::swap(nodal_list.at(11), nodal_list.at(9));
                std::swap(nodal_list.at(13), nodal_list.at(10));

                std::swap(nodal_list.at(12), nodal_list.at(17));
                std::swap(nodal_list.at(16), nodal_list.at(12));
                std::swap(nodal_list.at(16), nodal_list.at(13));

                std::swap(nodal_list.at(13), nodal_list.at(15));
                std::swap(nodal_list.at(13), nodal_list.at(19));

                std::swap(nodal_list.at(13), nodal_list.at(18));
                std::swap(nodal_list.at(14), nodal_list.at(18));
            }
            break;
        }
        default:
            break;
    }
}

std::vector<local_indices> convert_to_vtk(std::vector<local_indices> nodal_connectivity, element_topology const topology)
{
    switch (topology)
    {
        case element_topology::tetrahedron4:
        {
            for (auto& nodal_list : nodal_connectivity)
            {
                std::swap(nodal_list.at(0), nodal_list.at(1));
            }
            break;
        }
        case element_topology::tetrahedron10:
        {
            for (auto& nodal_list : nodal_connectivity)
            {
                std::swap(nodal_list.at(6), nodal_list.at(8));
                std::swap(nodal_list.at(8), nodal_list.at(9));
            }
            break;
        }
        case element_topology::hexahedron20:
        {
            // The ordering of the twenty points defining the cell is point ids (0-7,8-19) where
            // point ids 0-7 are the eight corner vertices of the cube; followed by twelve midedge
            // nodes (8-19). Note that these midedge nodes correspond lie on the edges defined by
            // 8 > (0,1), 9 > (1,2), 10 > (2,3), 11 > (3,0),
            // 12 > (4,5), 13 > (5,6), 14 > (6,7), 15 > (7,4),
            // 16 > (0,4), 17 > (1,5), 18 > (2,6), 19 > (3,7).

            // NOTE This corresponds nicely to the Hughes ordering
            break;
        }
        case element_topology::hexahedron27:
        {
            /* top
             *  7--14--6
             *  |      |
             * 15  25  13
             *  |      |
             *  4--12--5
             *
             *  middle
             * 19--23--18
             *  |      |
             * 20  26  21
             *  |      |
             * 16--22--17
             *
             * bottom
             *  3--10--2
             *  |      |
             * 11  24  9
             *  |      |
             *  0-- 8--1
             */
            for (auto& nodal_list : nodal_connectivity)
            {
                std::swap(nodal_list.at(21), nodal_list.at(25));
                std::swap(nodal_list.at(20), nodal_list.at(24));
            }
            break;
        }
        default:
            break;
    }
    return nodal_connectivity;
}

element_topology gmsh_type_to_enum(int const element_code)
{
    auto const found = gmsh_converter.find(element_code);
    if (found == gmsh_converter.end())
    {
        throw std::runtime_error("Element code " + std::to_string(element_code)
                                 + " not implemented for gmsh element type");
    }
    return found->second;
}

VTKCellType to_vtk(element_topology const topology)
{
    auto const found = vtk_converter.find(topology);
    if (found == vtk_converter.end())
    {
        throw std::runtime_error("Element code " + std::to_string(static_cast<int>(topology))
                                 + " not implemented for vtk element type");
    }
    return found->second;
}
} // namespace neon