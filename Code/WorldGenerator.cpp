#include "WorldGenerator.hpp"

#define JC_VORONOI_IMPLEMENTATION
#include "../Submodules/voronoi/src/jc_voronoi.h"

#include "Math/Rand.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Rendering/SFML_Specific/SFML_2D_Renderer.hpp"

#include <algorithm>


namespace world_gen
{

void VoronoiMapInitializer::mt_Generate(std::vector<jcv_point>& points, const jcv_rect& boundings)
{
    //const std::size_t l_Point_Count = 2000;
    const std::size_t l_Point_Count = 300;
    jcv_point l_Point;

    for (int ii = 0; ii < l_Point_Count; ii++)
    {
        l_Point.x = RandomGenerator::smt_Get()(boundings.min.x, boundings.max.x);
        l_Point.y = RandomGenerator::smt_Get()(boundings.min.y, boundings.max.y);

        if (std::find_if(points.begin(), points.end(), [&](const jcv_point& p) {return p.x == l_Point.x && p.y == l_Point.y;}) == points.end())
        {
            points.push_back(l_Point);
        }
    }

    points.clear();

    for (int yy = 0; yy < boundings.max.y; yy++)
    {
        for (int xx = 0; xx < boundings.max.x; xx++)
        {
            const float l_Diff_X = (boundings.max.x * 0.5f - xx) / (boundings.max.x * 0.5f);
            const float l_Diff_Y = (boundings.max.y * 0.5f - yy) / (boundings.max.y * 0.5f);
            const float l_Distance_To_Center = sqrt(l_Diff_X * l_Diff_X + l_Diff_Y * l_Diff_Y);
            const float l_Threshold = 0.003f * exp(-2.0f * l_Distance_To_Center * l_Distance_To_Center);
            const float l_Rand = RandomGenerator::smt_Get().mt_Generate(0.0f, 1.0f);

            /// 0.0005f

            if ((l_Rand < l_Threshold) && (points.size() < l_Point_Count))
            {
                points.push_back(jcv_point{xx, yy});
            }
        }
    }

    points.clear();

    for (int ii = 0; ii < l_Point_Count; /*ii++*/)
    {
        const float xx = RandomGenerator::smt_Get()(boundings.min.x, boundings.max.x);
        const float yy = RandomGenerator::smt_Get()(boundings.min.y, boundings.max.y);
        const float l_Diff_X = (boundings.max.x * 0.5f - xx) / (boundings.max.x * 0.5f);
        const float l_Diff_Y = (boundings.max.y * 0.5f - yy) / (boundings.max.y * 0.5f);
        const float l_Distance_To_Center = sqrt(l_Diff_X * l_Diff_X + l_Diff_Y * l_Diff_Y);
        const float l_Threshold = 0.1f * exp(-2.0f * l_Distance_To_Center * l_Distance_To_Center);
        const float l_Rand = RandomGenerator::smt_Get().mt_Generate(0.0f, 1.0f);
        const bool l_Record =       (std::find_if(points.begin(), points.end(), [&](const jcv_point& p) {return p.x == xx && p.y == yy;}) == points.end())
                              &&    (l_Rand < l_Threshold);

        if (l_Record == true)
        {
            points.push_back(jcv_point{xx, yy});
            ii++;
        }
    }
}


void VoronoiMapGenerator::mt_Generate(const sf::Vector2u& map_size, Array2D<Cell>& map_content)
{
    const int l_Pixel_Count = 500;
    jcv_rect l_Bounding_Box = {{0.0f, 0.0f}, {1.0f, 1.0f}};
    std::vector<jcv_point> l_Points;
    const jcv_site* l_Sites;
    jcv_graphedge* l_Graph_Edge;
    VoronoiMapInitializer l_Map_Initializer;
    jcv_diagram l_Diagram;
    memset(&l_Diagram, 0, sizeof(l_Diagram));

    l_Bounding_Box.max.x *= l_Pixel_Count;
    l_Bounding_Box.max.y *= l_Pixel_Count;

    l_Map_Initializer.mt_Generate(l_Points, l_Bounding_Box);

    std::cout << "Points count: " << l_Points.size() << '\n';
    map_content.mt_Resize(l_Bounding_Box.max.x, l_Bounding_Box.max.y);

    jcv_diagram_generate(l_Points.size(), l_Points.data(), &l_Bounding_Box, nullptr, &l_Diagram);

    std::cout << "Site count: " << l_Diagram.numsites << '\n';
    l_Sites = jcv_diagram_get_sites(&l_Diagram);
    for (int ii = 0; ii < l_Diagram.numsites; ii++)
    {
        l_Graph_Edge = l_Sites[ii].edges;

        while(l_Graph_Edge != nullptr)
        {
            auto l_fn_Fix_Point = [&](jcv_point& p)
            {
                p.x = std::max(p.x, 0.0f);
                p.x = std::min(p.x, l_Bounding_Box.max.x - 1.0f);
                p.y = std::max(p.y, 0.0f);
                p.y = std::min(p.y, l_Bounding_Box.max.y - 1.0f);
            };
            std::vector<sf::Vector2f> l_Line;

            l_fn_Fix_Point(l_Graph_Edge->pos[0]);
            l_fn_Fix_Point(l_Graph_Edge->pos[1]);

            mt_Get_Line(sf::Vector2f(l_Graph_Edge->pos[0].x, l_Graph_Edge->pos[0].y), sf::Vector2f(l_Graph_Edge->pos[1].x, l_Graph_Edge->pos[1].y), l_Line);

            for (std::size_t jj = 0; jj < l_Line.size(); jj++)
            {
                mt_Set_Cell_Edge(map_content, l_Line[jj].x, l_Line[jj].y, ii);
            }

            l_Graph_Edge = l_Graph_Edge->next;
        }
    }
    for (int ii = 0; ii < l_Diagram.numsites; ii++)
    {
        jcv_point l_Relaxed_Point = l_Sites[ii].p;
        int l_Edge_Count = 1;

        mt_Set_Cell_PrimaryPoint(map_content, l_Sites[ii].p.x, l_Sites[ii].p.y, ii);

        l_Graph_Edge = l_Sites[ii].edges;

        while(l_Graph_Edge != nullptr)
        {
            mt_Set_Cell_EdgeJunction(map_content, l_Graph_Edge->pos[0].x, l_Graph_Edge->pos[0].y, ii);
            mt_Set_Cell_EdgeJunction(map_content, l_Graph_Edge->pos[1].x, l_Graph_Edge->pos[1].y, ii);

            l_Relaxed_Point.x += l_Graph_Edge->pos[0].x;
            l_Relaxed_Point.y += l_Graph_Edge->pos[0].y;

            l_Edge_Count++;

            l_Graph_Edge = l_Graph_Edge->next;
        }

        l_Relaxed_Point.x /= l_Edge_Count;
        l_Relaxed_Point.y /= l_Edge_Count;

        mt_Set_Cell_RelaxedPoint(map_content, l_Relaxed_Point.x, l_Relaxed_Point.y, ii);
    }

    jcv_diagram_free(&l_Diagram);
}


void VoronoiMapGenerator::mt_Set_Cell(Array2D<Cell>& map_content, int xx, int yy, world_gen::CellType cell_type, const sf::Color& cell_color, int site_id)
{
    Cell& l_Cell = map_content(xx, yy);

    l_Cell.m_Type = cell_type;
    l_Cell.m_Color = cell_color;
    if (cell_type != l_Cell.m_Type)
    {
        l_Cell.m_Site_Ids.clear();
    }
    l_Cell.m_Site_Ids.push_back(site_id);
}

void VoronoiMapGenerator::mt_Set_Cell_PrimaryPoint(Array2D<Cell>& map_content, int xx, int yy, int site_id)
{
    mt_Set_Cell(map_content, xx, yy, world_gen::CellType::PrimaryPoint, sf::Color::Blue, site_id);
}

void VoronoiMapGenerator::mt_Set_Cell_RelaxedPoint(Array2D<Cell>& map_content, int xx, int yy, int site_id)
{
    mt_Set_Cell(map_content, xx, yy, world_gen::CellType::RelaxedPoint, sf::Color::Cyan, site_id);
}

void VoronoiMapGenerator::mt_Set_Cell_Edge(Array2D<Cell>& map_content, int xx, int yy, int site_id)
{
    mt_Set_Cell(map_content, xx, yy, world_gen::CellType::Edge, sf::Color::Yellow, site_id);
}

void VoronoiMapGenerator::mt_Set_Cell_EdgeJunction(Array2D<Cell>& map_content, int xx, int yy, int site_id)
{
    mt_Set_Cell(map_content, xx, yy, world_gen::CellType::EdgeJunction, sf::Color::Magenta, site_id);
}

void VoronoiMapGenerator::mt_Set_Cell_SiteContent(Array2D<Cell>& map_content, int xx, int yy, int site_id)
{
    mt_Set_Cell(map_content, xx, yy, world_gen::CellType::SiteContent, sf::Color::Black, site_id);
}

void VoronoiMapGenerator::mt_Get_Line(const sf::Vector2f& start, const sf::Vector2f& end, std::vector<sf::Vector2f>& line)
{
    const float dx = (end.x - start.x);
    const float dy = (end.y - start.y);
    float min, max;
    float l_Slope;

    line.clear();

    if ((static_cast<int>(dx) == 0) && (static_cast<int>(dy) == 0))
    {
        line.push_back(start);
        return;
    }

    if (std::fabs(dx) > std::fabs(dy))
    {
        if (start.x < end.x)
        {
            min = start.x;
            max = end.x;
        }
        else
        {
            min = end.x;
            max = start.x;
        }

        l_Slope = dy / dx;

        for (float xx = min; xx < max; xx += 1.0f)
        {
            float yy = start.y + ((xx - start.x) * l_Slope);
            line.push_back(sf::Vector2f(xx, yy));
        }
    }
    else
    {
        if (start.y < end.y)
        {
            min = start.y;
            max = end.y;
        }
        else
        {
            min = end.y;
            max = start.y;
        }

        l_Slope = dx / dy;

        for (float yy = min; yy < max; yy += 1.0f)
        {
            float xx = start.x + ((yy - start.y) * l_Slope);
            line.push_back(sf::Vector2f(xx, yy));
        }
    }
}


}




void fn_City_Test(const Array2D<world_gen::Cell>& source, Array2D<CityCell>& dest)
{
    fn_City_Test_Make_Roads(source, dest);
    fn_City_Test_Make_Walls(source, dest);
    fn_City_Test_Make_Houses(source, dest);
}


void fn_City_Test_Make_Roads(const Array2D<world_gen::Cell>& source, Array2D<CityCell>& dest)
{
    auto l_fn_Is_Edge = [](const world_gen::Cell& cell)
    {
        return cell.m_Type == world_gen::CellType::Edge || cell.m_Type == world_gen::CellType::EdgeJunction;
    };

    for (int yy = 1; yy < source.mt_Get_Height() - 1; yy++)
    {
        for (int xx = 1; xx < source.mt_Get_Width() - 1; xx++)
        {
            const world_gen::Cell& l_North = source(xx, yy - 1);
            const world_gen::Cell& l_South = source(xx, yy + 1);
            const world_gen::Cell& l_West = source(xx - 1, yy);
            const world_gen::Cell& l_East = source(xx + 1, yy);
            const world_gen::Cell& l_Here = source(xx, yy);
            CityCell& l_Current_Cell = dest(xx, yy);

            if (l_Current_Cell.m_Type == CityCellType::None)
            {
                if (l_fn_Is_Edge(l_Here) || l_fn_Is_Edge(l_North) || l_fn_Is_Edge(l_South) || l_fn_Is_Edge(l_West) || l_fn_Is_Edge(l_East))
                {
                    l_Current_Cell.m_Type = CityCellType::Road;
                }
            }
        }
    }
}

void fn_City_Test_Make_Walls(const Array2D<world_gen::Cell>& source, Array2D<CityCell>& dest)
{
    auto l_fn_Is_Road = [](const CityCell& cell)
    {
        return cell.m_Type == CityCellType::Road;
    };
    auto l_fn_Is_Not_None = [](const CityCell& cell)
    {
        return cell.m_Type != CityCellType::None;
    };

    for (int yy = 1; yy < source.mt_Get_Height() - 1; yy++)
    {
        for (int xx = 1; xx < source.mt_Get_Width() - 1; xx++)
        {
            const CityCell& l_North = dest(xx, yy - 1);
            const CityCell& l_South = dest(xx, yy + 1);
            const CityCell& l_West = dest(xx - 1, yy);
            const CityCell& l_East = dest(xx + 1, yy);
            CityCell& l_Current_Cell = dest(xx, yy);

            if (l_Current_Cell.m_Type == CityCellType::None)
            {
                if (l_fn_Is_Road(l_North) || l_fn_Is_Road(l_South) || l_fn_Is_Road(l_West) || l_fn_Is_Road(l_East))
                {
                    l_Current_Cell.m_Type = CityCellType::Wall;
                }
            }
        }
    }

    for (int yy = 1; yy < source.mt_Get_Height() - 1; yy++)
    {
        for (int xx = 1; xx < source.mt_Get_Width() - 1; xx++)
        {
            const CityCell& l_NW = dest(xx - 1, yy - 1);
            const CityCell& l_NE = dest(xx + 1, yy - 1);
            const CityCell& l_SW = dest(xx - 1, yy + 1);
            const CityCell& l_SE = dest(xx + 1, yy + 1);
            CityCell& l_Current_Cell = dest(xx, yy);

            if (l_Current_Cell.m_Type == CityCellType::None)
            {
                if (l_fn_Is_Road(l_NW) || l_fn_Is_Road(l_NE) || l_fn_Is_Road(l_SW) || l_fn_Is_Road(l_SE))
                {
                    l_Current_Cell.m_Type = CityCellType::Wall;
                }
            }
        }
    }

    for (int yy = 1; yy < source.mt_Get_Height() - 1; yy++)
    {
        for (int xx = 1; xx < source.mt_Get_Width() - 1; xx++)
        {
            const CityCell& l_North = dest(xx, yy - 1);
            const CityCell& l_South = dest(xx, yy + 1);
            const CityCell& l_West = dest(xx - 1, yy);
            const CityCell& l_East = dest(xx + 1, yy);
            const CityCell& l_NW = dest(xx - 1, yy - 1);
            const CityCell& l_NE = dest(xx + 1, yy - 1);
            const CityCell& l_SW = dest(xx - 1, yy + 1);
            const CityCell& l_SE = dest(xx + 1, yy + 1);
            CityCell& l_Current_Cell = dest(xx, yy);

            if (l_Current_Cell.m_Type == CityCellType::Wall)
            {
                if (    l_fn_Is_Not_None(l_NW) && l_fn_Is_Not_None(l_NE) && l_fn_Is_Not_None(l_SW) && l_fn_Is_Not_None(l_SE)
                    &&  l_fn_Is_Not_None(l_North) && l_fn_Is_Not_None(l_South) && l_fn_Is_Not_None(l_West) && l_fn_Is_Not_None(l_East))
                {
                    l_Current_Cell.m_Type = CityCellType::Road;
                }
            }
        }
    }
}

void fn_City_Test_Make_Houses(const Array2D<world_gen::Cell>& source, Array2D<CityCell>& dest)
{
    for (int yy = 1; yy < source.mt_Get_Height() - 1; yy++)
    {
        for (int xx = 1; xx < source.mt_Get_Width() - 1; xx++)
        {
            CityCell& l_Current_Cell = dest(xx, yy);

            if (l_Current_Cell.m_Type == CityCellType::None)
            {
                l_Current_Cell.m_Type = CityCellType::HouseFloor;
            }
        }
    }
}






WorldGenerator::WorldGenerator() : Prototype("World Generator")
{}

WorldGenerator::~WorldGenerator()
{}

bool WorldGenerator::mt_Specialized_Init()
{
    mt_Generate_Diagram();

    return true;
}

void WorldGenerator::mt_Handle_Event(const sf::Event& event)
{
    //
}

void WorldGenerator::mt_Update(float elapsed_time)
{
    float l_Threshold = 0.1f;

    while (m_Accumulated_Time >= l_Threshold)
    {
        m_Accumulated_Time -= l_Threshold;
    }
}

void WorldGenerator::mt_Draw(sf::RenderTarget& target)
{
    Renderer2D::smt_Render_Texture(target, m_Texture, Renderer2D::TextureMode::None);
}

void WorldGenerator::mt_Generate_Diagram()
{
    constexpr const int l_Pixel_Count = 500;
    world_gen::VoronoiMapGenerator l_Generator;

    l_Generator.mt_Generate(sf::Vector2u(l_Pixel_Count, l_Pixel_Count), m_Cells);
    mt_Save_Image("WorldGeneration.png");
    m_Texture.loadFromFile("WorldGeneration.png");

    mt_City_Test(m_Cells);
    mt_Save_Image("WorldGenerationCity.png");
    m_Texture.loadFromFile("WorldGenerationCity.png");
}

void WorldGenerator::mt_Save_Image(const char* file_name)
{
    sf::Image l_Image;

    l_Image.create(m_Cells.mt_Get_Width(), m_Cells.mt_Get_Height(), sf::Color::Black);

    for (int yy = 0; yy < m_Cells.mt_Get_Height(); yy++)
    {
        for (int xx = 0; xx < m_Cells.mt_Get_Width(); xx++)
        {
            l_Image.setPixel(xx, yy, m_Cells(xx, yy).m_Color);
        }
    }

    l_Image.saveToFile(file_name);
}

void WorldGenerator::mt_City_Test(Array2D<world_gen::Cell>& cells)
{
    Array2D<CityCell> l_City_Cells;

    l_City_Cells.mt_Resize(cells.mt_Get_Width(), cells.mt_Get_Height());

    fn_City_Test(cells, l_City_Cells);

    for (int yy = 0; yy < l_City_Cells.mt_Get_Height(); yy++)
    {
        for (int xx = 0; xx < l_City_Cells.mt_Get_Width(); xx++)
        {
            world_gen::Cell& l_Color_Cell = cells(xx, yy);
            const CityCell& l_City_Cell = l_City_Cells(xx, yy);

            switch(l_City_Cell.m_Type)
            {
            case CityCellType::None:        l_Color_Cell.m_Color = sf::Color::Black; break;
            case CityCellType::Road:        l_Color_Cell.m_Color = sf::Color::Yellow; break;
            case CityCellType::Wall:        l_Color_Cell.m_Color = sf::Color::Blue; break;
            case CityCellType::HouseFloor:  l_Color_Cell.m_Color = sf::Color(0, 0, 128); break;
            default:                        l_Color_Cell.m_Color = sf::Color::Black; break;
            }
        }
    }
}

void WorldGenerator::mt_Water_Test(void)
{
    sf::Image l_Image;
    std::size_t xx = 0;

    l_Image.create(m_Cells.mt_Get_Width(), m_Cells.mt_Get_Height(), sf::Color::Black);

    for(; xx < m_Cells.mt_Get_Width(); xx++)
    {
        if (m_Cells(xx, 0).m_Type == world_gen::CellType::EdgeJunction)
        {
            break;
        }
    }
}
