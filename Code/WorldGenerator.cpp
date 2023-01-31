#include "WorldGenerator.hpp"

#define JC_VORONOI_IMPLEMENTATION
#include "../Submodules/voronoi/src/jc_voronoi.h"

#include "Math/Rand.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Rendering/SFML_Specific/SFML_2D_Renderer.hpp"

#include <algorithm>





WorldGenerator::WorldGenerator() : Prototype("World Generator")
{
    memset(&m_Diagram, 0, sizeof(m_Diagram));
}

WorldGenerator::~WorldGenerator()
{
    jcv_diagram_free(&m_Diagram);
}

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
    const int l_Pixel_Count = 500;
    jcv_rect l_Bounding_Box = {{0.0f, 0.0f}, {1.0f, 1.0f}};
    std::vector<jcv_point> l_Points;
    const jcv_site* l_Sites;
    jcv_graphedge* l_Graph_Edge;

    l_Bounding_Box.max.x *= l_Pixel_Count;
    l_Bounding_Box.max.y *= l_Pixel_Count;

    mt_Generate_Points_Position_Default(l_Points, l_Bounding_Box);
    //mt_Generate_Points_Position_Center_Density(l_Points, l_Bounding_Box);

    std::cout << "Points count: " << l_Points.size();
    mt_Resize_Cells_Container(l_Bounding_Box);

    jcv_diagram_generate(l_Points.size(), l_Points.data(), &l_Bounding_Box, nullptr, &m_Diagram);

    std::cout << "\nSite count: " << m_Diagram.numsites;
    l_Sites = jcv_diagram_get_sites(&m_Diagram);
    for (int ii = 0; ii < m_Diagram.numsites; ii++)
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

            //std::cout << "\n[ " << l_Graph_Edge->pos[0].x << ' ' << l_Graph_Edge->pos[0].y << " ] [ " << l_Graph_Edge->pos[1].x << ' ' << l_Graph_Edge->pos[1].y << " ]";

            l_fn_Fix_Point(l_Graph_Edge->pos[0]);
            l_fn_Fix_Point(l_Graph_Edge->pos[1]);

            mt_Get_Line(sf::Vector2f(l_Graph_Edge->pos[0].x, l_Graph_Edge->pos[0].y), sf::Vector2f(l_Graph_Edge->pos[1].x, l_Graph_Edge->pos[1].y), l_Line);

            for (std::size_t ii = 0; ii < l_Line.size(); ii++)
            {
                mt_Set_Cell_Edge(l_Line[ii].x, l_Line[ii].y);
            }

            /// Draw Line (sf::Color::Yellow)
            /*mt_Draw_Line(l_Image,
                         sf::Vector2f(l_Graph_Edge->pos[0].x, l_Graph_Edge->pos[0].y),
                         sf::Vector2f(l_Graph_Edge->pos[1].x, l_Graph_Edge->pos[1].y),
                         sf::Color::Yellow);*/

            l_Graph_Edge = l_Graph_Edge->next;
        }
    }
    for (int ii = 0; ii < m_Diagram.numsites; ii++)
    {
        jcv_point l_Relaxed_Point = l_Sites[ii].p;
        int l_Edge_Count = 1;

        //l_Image.setPixel(l_Sites[ii].p.x, l_Sites[ii].p.y, sf::Color::Blue);
        mt_Set_Cell_PrimaryPoint(l_Sites[ii].p.x, l_Sites[ii].p.y);

        l_Graph_Edge = l_Sites[ii].edges;

        while(l_Graph_Edge != nullptr)
        {
            //l_Image.setPixel(l_Graph_Edge->pos[0].x, l_Graph_Edge->pos[0].y, sf::Color::Magenta);
            //l_Image.setPixel(l_Graph_Edge->pos[1].x, l_Graph_Edge->pos[1].y, sf::Color::Magenta);

            mt_Set_Cell_EdgeJunction(l_Graph_Edge->pos[0].x, l_Graph_Edge->pos[0].y);
            mt_Set_Cell_EdgeJunction(l_Graph_Edge->pos[1].x, l_Graph_Edge->pos[1].y);

            l_Relaxed_Point.x += l_Graph_Edge->pos[0].x;
            l_Relaxed_Point.y += l_Graph_Edge->pos[0].y;

            l_Edge_Count++;

            l_Graph_Edge = l_Graph_Edge->next;
        }

        l_Relaxed_Point.x /= l_Edge_Count;
        l_Relaxed_Point.y /= l_Edge_Count;

        //l_Image.setPixel(l_Relaxed_Point.x, l_Relaxed_Point.y, sf::Color::Cyan);
        mt_Set_Cell_RelaxedPoint(l_Relaxed_Point.x, l_Relaxed_Point.y);
    }

    mt_Save_Image("WorldGeneration.png", l_Bounding_Box.max.x, l_Bounding_Box.max.y);

    std::cout << "\n\n";
}

void WorldGenerator::mt_Generate_Points_Position_Default(std::vector<jcv_point>& points, const jcv_rect& boundings)
{
    points.resize(2000);

    for (int ii = 0; ii < points.size(); ii++)
    {
        points[ii].x = RandomGenerator::smt_Get()(boundings.min.x, boundings.max.x);
        points[ii].y = RandomGenerator::smt_Get()(boundings.min.y, boundings.max.y);
    }
}

void WorldGenerator::mt_Generate_Points_Position_Center_Density(std::vector<jcv_point>& points, const jcv_rect& boundings)
{
    points.resize(50);

    for (int ii = 0; ii < points.size(); ii++)
    {
        points[ii].x = RandomGenerator::smt_Get()(boundings.min.x, boundings.max.x);
        points[ii].y = RandomGenerator::smt_Get()(boundings.min.y, boundings.max.y);
    }

    const float dx = (boundings.max.x - boundings.min.x) / 10.0f;
    const float dy = (boundings.max.y - boundings.min.y) / 10.0f;
    const float centerx = (boundings.max.x - boundings.min.x) / 2.0f;
    const float centery = (boundings.max.y - boundings.min.y) / 2.0f;

    points.clear();
    for (float xx = boundings.min.x; xx < boundings.max.x; xx += dx)
    {
        for (float yy = boundings.min.y; yy < boundings.max.y; yy += dy)
        {
            uint32_t l_Threshold = (std::abs(centerx - xx) / boundings.max.x * std::abs(centery - yy) / boundings.max.y) * 100;
            if (RandomGenerator::smt_Get().mt_Generate(100) < l_Threshold)
            {
                points.push_back({xx, yy});
            }
        }
    }
}

void WorldGenerator::mt_Resize_Cells_Container(const jcv_rect& boundings)
{
    m_Cells.resize(boundings.max.y, std::vector<world_gen::Cell>(boundings.max.x));
}

world_gen::Cell& WorldGenerator::mt_Get_Cell(int xx, int yy)
{
    return m_Cells[yy][xx];
}

void WorldGenerator::mt_Set_Cell(int xx, int yy, world_gen::CellType cell_type, const sf::Color& cell_color)
{
    world_gen::Cell& l_Cell = mt_Get_Cell(xx, yy);

    l_Cell.m_Type = cell_type;
    l_Cell.m_Color = cell_color;
}

void WorldGenerator::mt_Set_Cell_PrimaryPoint(int xx, int yy)
{
    mt_Set_Cell(xx, yy, world_gen::CellType::PrimaryPoint, sf::Color::Blue);
}

void WorldGenerator::mt_Set_Cell_RelaxedPoint(int xx, int yy)
{
    mt_Set_Cell(xx, yy, world_gen::CellType::RelaxedPoint, sf::Color::Cyan);
}

void WorldGenerator::mt_Set_Cell_Edge(int xx, int yy)
{
    mt_Set_Cell(xx, yy, world_gen::CellType::Edge, sf::Color::Yellow);
}

void WorldGenerator::mt_Set_Cell_EdgeJunction(int xx, int yy)
{
    mt_Set_Cell(xx, yy, world_gen::CellType::EdgeJunction, sf::Color::Magenta);
}

void WorldGenerator::mt_Set_Cell_SiteContent(int xx, int yy)
{
    mt_Set_Cell(xx, yy, world_gen::CellType::SiteContent, sf::Color::Black);
}


void WorldGenerator::mt_Get_Line(const sf::Vector2f& start, const sf::Vector2f& end, std::vector<sf::Vector2f>& line)
{
    const float dx = (end.x - start.x);
    const float dy = (end.y - start.y);
    float min, max;
    float l_Slope;

    line.clear();

    if ((dx == 0.0f) && (dy == 0.0f))
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

void WorldGenerator::mt_Save_Image(const char* file_name, int width, int height)
{
    sf::Image l_Image;

    l_Image.create(width, height, sf::Color::Black);

    for (int yy = 0; yy < height; yy++)
    {
        for (int xx = 0; xx < width; xx++)
        {
            l_Image.setPixel(xx, yy, mt_Get_Cell(xx, yy).m_Color);
        }
    }

    l_Image.saveToFile(file_name);
}

void WorldGenerator::mt_Draw_Line(sf::Image& image, const sf::Vector2f& start, const sf::Vector2f& end, const sf::Color& color)
{
    std::vector<sf::Vector2f> l_Line;

    mt_Get_Line(start, end, l_Line);

    for (std::size_t ii = 0; ii < l_Line.size(); ii++)
    {
        image.setPixel(l_Line[ii].x, l_Line[ii].y, color);
    }
}
