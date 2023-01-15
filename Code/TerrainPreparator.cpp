#include "TerrainPreparator.hpp"

#include "Utilities/Log.hpp"

#include <fstream>
#include <sstream>

namespace priv
{


bool fn_Create_Obj_File(const char* file, const char* object_name, std::vector<glm::vec3>& pos)
{
    std::ofstream l_Output;
    std::stringstream l_Faces;

    l_Output.open(file);

    if (l_Output.is_open())
    {
        l_Output << "o " << object_name;

        LOG_dbg("size %% 6: %d", pos.size() % 6);

        for (std::size_t ii = 0; ii < pos.size(); ii++)
        {
            l_Output << "\nv " << pos[ii].x << ' ' << pos[ii].y << ' ' << pos[ii].z;
        }

        for (std::size_t ii = 0; ii < pos.size(); ii += 6)
        {
            l_Faces << "\nf";
            for (std::size_t jj = 0; jj < 6; jj++)
            {
                l_Faces << ' ' << ii + jj + 1 << "//" << 1;
            }
        }

        l_Output << "\nvn 0 1 0" << "\nusemtl None" << "\ns 1" << l_Faces.str();
    }

    return l_Output.is_open();
}



const glm::mat2 sm_Hex_To_Pixel_Pointy(sqrt(3.0f), 0.0f, sqrt(3.0f)/2.0f, 3.0f/2.0f);
const glm::mat2 sm_Hex_To_Pixel_Flat(3.0f/2.0f, sqrt(3.0f)/2.0f, 0.0f, sqrt(3.0f));
const glm::mat2 sm_Pixel_To_Hex_Pointy(sqrt(3.0)/3.0f, -1.0f/3.0f, 0.0f, 2.0f/3.0f);
const glm::mat2 sm_Pixel_To_Hex_Flat(2.0f/3.0f, 0.0f, -1.0f/3.0f, sqrt(3.0)/3.0f);



sf::Vector2f fn_Coord_To_Pix(const sf::Vector2u& coord, const sf::Vector2f& tile_size)
{
    sf::Vector2f l_Pix;

    l_Pix.x = tile_size.x * (coord.x + (0.5f * (coord.y & 1)));
    l_Pix.y = tile_size.y * coord.y * 0.75f;

    return l_Pix;
}


void fn_Push_Hex_Tile(std::vector<glm::vec3>& pos, const sf::Vector2u& offset_index)
{
    sf::Vector2f l_Offset_Pos;

    l_Offset_Pos = fn_Coord_To_Pix(offset_index, sf::Vector2f(1.0f * sqrt(3.0f) / 1.0f, 2.0f));

    for (int ii = 0; ii < 6; ii++)
    {
        double l_Angle = (6 - ii) * 2 * 3.14f / 6;

        l_Angle += 90 * 3.14f / 180.0f;

        pos.push_back(glm::vec3(l_Offset_Pos.x + std::cos(l_Angle), 0.0f, l_Offset_Pos.y + std::sin(l_Angle)));
        std::cout << '\n' << pos.back().x << ' ' << pos.back().y << ' ' << pos.back().z;
    }
}

}


void fn_TerrainPreparator(int argc, char** argv)
{
    sf::Vector2u l_Terrain_Size;
    std::vector<glm::vec3> l_Vertices;

    l_Terrain_Size.x = 20;
    l_Terrain_Size.y = 20;

    for (unsigned int yy = 0; yy < l_Terrain_Size.y; yy++)
    {
        for (unsigned int xx = 0; xx < l_Terrain_Size.x; xx++)
        {
            priv::fn_Push_Hex_Tile(l_Vertices, sf::Vector2u(xx, yy));
        }
    }

    priv::fn_Create_Obj_File("../../Assets/TerrainPreparator/Terrain.obj", "Tile", l_Vertices);
}
