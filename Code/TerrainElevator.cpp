#include "TerrainElevator.hpp"

#include <array>
#include <vector>
#include "glm/glm.hpp"

#include "Utilities/Log.hpp"

#include <fstream>
#include <sstream>

#include "../Submodules/tinyXML/Sources/XMLFileLoader.hpp"

namespace terrain_elevator
{


struct TilesetLoader
{
    bool mt_Load(const XML_Element& e)
    {
        e.mt_Get_Attribute("tilecount", m_Tileset_Data->y);
        e.mt_Get_Attribute("columns", m_Tileset_Data->x);

        m_Tileset_Data->y /= m_Tileset_Data->x;

        return true;
    }

    sf::Vector2u* m_Tileset_Data;
};

struct MapLoader
{
    bool mt_Load(const XML_Element& e)
    {
        std::stringstream l_Data_Stream;
        std::string l_Data;

        l_Data_Stream.str(e.mt_Get_Text());

        while(std::getline(l_Data_Stream, l_Data, ' '))
        {
            std::stringstream ss(l_Data);

            m_Data->push_back({});

            while(std::getline(ss, l_Data, ','))
            {
                m_Data->back().push_back(std::stoi(l_Data));
            }
        }

        for (std::size_t ii = 0; ii < m_Data->size(); ii++)
        {
            for (std::size_t jj = 0; jj < (*m_Data)[ii].size(); jj++)
            {
                std::cout << (*m_Data)[ii][jj] << ',';
            }
            std::cout << '\n';
        }

        return true;
    }

    std::vector<std::vector<unsigned int>>* m_Data;
};






bool fn_Create_Obj_File(const char* file, const char* object_name, const std::vector<glm::vec3>& pos, const std::vector<unsigned int> indices)
{
    std::ofstream l_Output;
    std::stringstream l_Faces;

    l_Output.open(file);

    if (l_Output.is_open())
    {
        l_Output << "o " << object_name;

        LOG_dbg("size %% 8: %d", pos.size() % 8);

        for (std::size_t ii = 0; ii < pos.size(); ii++)
        {
            l_Output << "\nv " << pos[ii].x << ' ' << pos[ii].y << ' ' << pos[ii].z;
        }

        for (std::size_t ii = 0; ii < indices.size(); ii += 4)
        {
            l_Faces << "\nf";
            for (std::size_t jj = 0; jj < 4; jj++)
            {
                l_Faces << ' ' << indices[ii + jj] + 1;
            }
        }

        l_Output << "\nvn 0 1 0" << "\nusemtl None" << "\ns 1" << l_Faces.str();
    }

    return l_Output.is_open();
}


void fn_Push_Cube(std::vector<glm::vec3>& pos, std::vector<unsigned int>& indices, const sf::Vector2u& offset, float height)
{
    const unsigned int l_Index_Offset = pos.size() / 8;
    std::array<std::array<unsigned int, 4>, 6> l_Indices = {{
        { 0, 1, 2, 3 },
        { 4, 5, 6, 7 },
        { 0, 1, 5, 4 },
        { 0, 4, 7, 3 },
        { 1, 2, 6, 5 },
        { 2, 3, 7, 6 }
    }};

    pos.push_back(glm::vec3(offset.x + 0.0f, 0.0f, offset.y + 0.0f));
    pos.push_back(glm::vec3(offset.x + 1.0f, 0.0f, offset.y + 0.0f));
    pos.push_back(glm::vec3(offset.x + 1.0f, 0.0f, offset.y + 1.0f));
    pos.push_back(glm::vec3(offset.x + 0.0f, 0.0f, offset.y + 1.0f));

    pos.push_back(glm::vec3(offset.x + 0.0f, height, offset.y + 0.0f));
    pos.push_back(glm::vec3(offset.x + 1.0f, height, offset.y + 0.0f));
    pos.push_back(glm::vec3(offset.x + 1.0f, height, offset.y + 1.0f));
    pos.push_back(glm::vec3(offset.x + 0.0f, height, offset.y + 1.0f));

    for (std::size_t ii = 0; ii < l_Indices.size(); ii++)
    {
        for (std::size_t jj = 0; jj < l_Indices[ii].size(); jj++)
        {
            indices.push_back(l_Index_Offset * 8 + l_Indices[ii][jj]);
        }
    }
}

void fn_Push_Stair(std::vector<glm::vec3>& pos, std::vector<unsigned int>& indices, const sf::Vector2u& offset, float height)
{
    //
}




void fn_Load_Tileset(const char* tileset, sf::Vector2u& tileset_data)
{
    XMLFileLoader l_Loader;
    TilesetLoader l_Load;

    l_Loader.mt_Set_File(tileset);
    l_Loader.mt_Add_On_Entry_Callback(tileset, "/tileset", &TilesetLoader::mt_Load, &l_Load);

    l_Load.m_Tileset_Data = &tileset_data;

    l_Loader.mt_Work();
}

void fn_Load_Map(const char* map, std::vector<std::vector<unsigned int>>& data)
{
    XMLFileLoader l_Loader;
    MapLoader l_Load;

    data.resize({});
    l_Load.m_Data = &data;

    l_Loader.mt_Set_File(map);
    l_Loader.mt_Add_On_Entry_Callback(map, "/map/layer/data", &MapLoader::mt_Load, &l_Load);

    l_Loader.mt_Work();
}

void fn_Create_Terrain(const char* tileset, const char* map, const char* result_file)
{
    sf::Vector2u l_Tiles;
    sf::Vector2u l_Tileset_Data;
    std::vector<std::vector<unsigned int>> l_Map_Data;

    std::vector<glm::vec3> l_Positions;
    std::vector<unsigned int> l_Indices;
    float l_Height = 1.0f;
    int l_Tile_Line_Index;

    fn_Load_Tileset(tileset, l_Tileset_Data);
    fn_Load_Map(map, l_Map_Data);

    l_Positions.resize(0);
    l_Indices.resize(0);
    for (l_Tiles.y = 0; l_Tiles.y < l_Map_Data.size(); l_Tiles.y++)
    {
        for (l_Tiles.x = 0; l_Tiles.x < l_Map_Data[l_Tiles.y].size(); l_Tiles.x++)
        {
            l_Tile_Line_Index = l_Map_Data[l_Tiles.y][l_Tiles.x];

            if (l_Tile_Line_Index != 0)
            {
                l_Height = ((l_Tile_Line_Index - 1) / l_Tileset_Data.x) / 4.0f;

                fn_Push_Cube(l_Positions, l_Indices, l_Tiles, l_Height);
            }
        }
    }

    fn_Create_Obj_File(result_file, "Terrain", l_Positions, l_Indices);
}


}


TerrainElevator::TerrainElevator() : Prototype("Terrain Elevator") {}

bool TerrainElevator::mt_Specialized_Init()
{
    std::vector<glm::vec3> l_Positions;
    std::vector<unsigned int> l_Indices;

    terrain_elevator::fn_Push_Cube(l_Positions, l_Indices, sf::Vector2u(0, 0), 0.25f);
    terrain_elevator::fn_Push_Cube(l_Positions, l_Indices, sf::Vector2u(0, 2), 0.25f);
    terrain_elevator::fn_Push_Cube(l_Positions, l_Indices, sf::Vector2u(2, 0), 0.25f);
    terrain_elevator::fn_Push_Cube(l_Positions, l_Indices, sf::Vector2u(2, 2), 0.25f);
    terrain_elevator::fn_Push_Cube(l_Positions, l_Indices, sf::Vector2u(0, 1), 0.5f);
    terrain_elevator::fn_Push_Cube(l_Positions, l_Indices, sf::Vector2u(1, 0), 0.5f);
    terrain_elevator::fn_Push_Cube(l_Positions, l_Indices, sf::Vector2u(2, 1), 0.5f);
    terrain_elevator::fn_Push_Cube(l_Positions, l_Indices, sf::Vector2u(1, 2), 0.5f);
    terrain_elevator::fn_Push_Cube(l_Positions, l_Indices, sf::Vector2u(1, 1), 1.0f);

    terrain_elevator::fn_Create_Obj_File("../../Assets/TerrainElevator/CubicTerrain.obj", "Terrain", l_Positions, l_Indices);


    terrain_elevator::fn_Create_Terrain("../../Assets/TerrainElevator/Tileset.tsx",
                                        "../../Assets/TerrainElevator/Map.tmx",
                                        "../../Assets/TerrainElevator/CubicTerrain2.obj");

    return true;
}

void TerrainElevator::mt_Handle_Event(const sf::Event& event)
{
    //
}

void TerrainElevator::mt_Update(float elapsed_time)
{
    //
}

void TerrainElevator::mt_Draw(sf::RenderTarget& target)
{
    //
}
