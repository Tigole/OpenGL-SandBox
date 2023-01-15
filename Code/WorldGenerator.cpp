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
    float l_Threshold = 0.001f;

    while (m_Accumulated_Time >= l_Threshold)
    {
        m_Accumulated_Time -= l_Threshold;

        if (m_Shuffled_Remaining_Site_Indices.size() > 0)
        {
            world_gen::Cell l_Cell;
            const jcv_graphedge* l_Edge;

            l_Cell.m_Site_Id = m_Shuffled_Remaining_Site_Indices.back();

            l_Edge = jcv_diagram_get_sites(&m_Diagram)[m_Shuffled_Remaining_Site_Indices.back()].edges;
            l_Cell.m_Point_Count = 0;
            while (l_Edge != nullptr)
            {
                l_Cell.m_Point_Count++;
                l_Edge = l_Edge->next;
            }

            l_Cell.m_Color = sf::Color::White;
            l_Cell.m_Region_Id = -1;

            m_Cells.push_back(l_Cell);

            std::cout << "Added cell " << l_Cell.m_Site_Id << '\n';

            m_Shuffled_Remaining_Site_Indices.pop_back();
        }
        else if (m_Region_Count > 0)
        {
            while(m_Region_Count >= 0)
            {
                world_gen::Region l_Region;

                l_Region.m_Region_Id = m_Region_Count;
                l_Region.m_Cell_Ids.push_back(m_Region_Count);

                m_Cells[m_Region_Count].m_Color = sf::Color::Blue;
                m_Cells[m_Region_Count].m_Region_Id = m_Region_Count;
                m_Cells[m_Region_Count].m_Altitude = RandomGenerator::smt_Get()(500) - 250;

                m_Regions.push_back(l_Region);

                m_Region_Count--;
            }
        }
        else
        {
            //
        }
    }
}

void WorldGenerator::mt_Draw(sf::RenderTarget& target)
{
    const jcv_site* l_Sites;
    const jcv_graphedge* l_Edge;
    sf::ConvexShape l_Shape;

    Renderer2D::smt_Render_Texture(target, m_RenderTexture.getTexture(), Renderer2D::TextureMode::None);

    l_Sites = jcv_diagram_get_sites(&m_Diagram);

    for (std::size_t ii = 0; ii < m_Cells.size(); ii++)
    {
        int l_Current_Point = 0;

        l_Shape.setPointCount(m_Cells[ii].m_Point_Count);
        l_Shape.setFillColor(m_Cells[ii].m_Color);
        l_Shape.setOutlineThickness(1.0f);
        l_Shape.setOutlineColor(sf::Color::Black);

        l_Edge = l_Sites[m_Cells[ii].m_Site_Id].edges;

        while(l_Edge != nullptr)
        {
            l_Shape.setPoint(l_Current_Point, sf::Vector2f(l_Edge->pos[0].x, l_Edge->pos[0].y));

            l_Current_Point++;

            l_Edge = l_Edge->next;
        }

        target.draw(l_Shape);
    }

    for (std::size_t ii = 0; ii < m_Regions.size(); ii++)
    {
        //
    }

    Renderer2D::smt_Render_Text(target, "azerty", sf::Vector2f(0, 0), 32, sf::Color::Black);
}


void WorldGenerator::mt_Generate_Diagram()
{
    const int l_Point_Count = 5000;
    const int l_Pixel_Count = 500;
    jcv_rect l_Bounding_Box = {{0.0f, 0.0f}, {2.0f, 2.0f}};
    jcv_point l_Points[l_Point_Count];
    const jcv_site* l_Sites;
    jcv_graphedge* l_Graph_Edge;

    m_Region_Count = (l_Point_Count * 1) / 100;

    l_Bounding_Box.max.x *= l_Pixel_Count;
    l_Bounding_Box.max.y *= l_Pixel_Count;

    m_RenderTexture.create(l_Bounding_Box.max.x, l_Bounding_Box.max.y, false);

    m_Shuffled_Remaining_Site_Indices.resize(l_Point_Count);
    std::cout << "Points count: " << l_Point_Count;
    for (int ii = 0; ii < l_Point_Count; ii++)
    {
        l_Points[ii].x = RandomGenerator::smt_Get()(l_Bounding_Box.min.x, l_Bounding_Box.max.x);
        l_Points[ii].y = RandomGenerator::smt_Get()(l_Bounding_Box.min.y, l_Bounding_Box.max.y);

        m_Shuffled_Remaining_Site_Indices[ii] = ii;

        //std::cout << "\n[ " << ii << " ] : " << l_Points[ii].x << ' ' << l_Points[ii].y;
    }

    jcv_diagram_generate(l_Point_Count, l_Points, &l_Bounding_Box, nullptr, &m_Diagram);

    m_RenderTexture.clear();

    std::cout << "\nSite count: " << m_Diagram.numsites;
    l_Sites = jcv_diagram_get_sites(&m_Diagram);
    for (int ii = 0; ii < m_Diagram.numsites; ii++)
    {
        l_Graph_Edge = l_Sites[ii].edges;

        while(l_Graph_Edge != nullptr)
        {
            //std::cout << "\n[ " << l_Graph_Edge->pos[0].x << ' ' << l_Graph_Edge->pos[0].y << " ] [ " << l_Graph_Edge->pos[1].x << ' ' << l_Graph_Edge->pos[1].y << " ]";

            Renderer2D::smt_Render_Line(m_RenderTexture,
                                        sf::Vector2f(l_Graph_Edge->pos[0].x, l_Graph_Edge->pos[0].y),
                                        sf::Vector2f(l_Graph_Edge->pos[1].x, l_Graph_Edge->pos[1].y),
                                        sf::Color::Yellow);

            l_Graph_Edge = l_Graph_Edge->next;
        }
    }
    for (int ii = 0; ii < m_Diagram.numsites; ii++)
    {
        Renderer2D::smt_Render_Point(m_RenderTexture, sf::Vector2f(l_Sites[ii].p.x, l_Sites[ii].p.y), sf::Color::Green);

        l_Graph_Edge = l_Sites[ii].edges;

        while(l_Graph_Edge != nullptr)
        {
            Renderer2D::smt_Render_Point(m_RenderTexture, sf::Vector2f(l_Graph_Edge->pos[0].x, l_Graph_Edge->pos[0].y), sf::Color::Magenta);
            Renderer2D::smt_Render_Point(m_RenderTexture, sf::Vector2f(l_Graph_Edge->pos[1].x, l_Graph_Edge->pos[1].y), sf::Color::Magenta);

            l_Graph_Edge = l_Graph_Edge->next;
        }
    }

    m_RenderTexture.display();
    m_RenderTexture.getTexture().copyToImage().saveToFile("WorldGeneration.png");

    /// Shuffle
    for (std::size_t ii = m_Shuffled_Remaining_Site_Indices.size() - 1; ii > 1; ii--)
    {
        std::size_t jj = RandomGenerator::smt_Get()(0, static_cast<uint32_t>(ii));
        std::swap(m_Shuffled_Remaining_Site_Indices[jj], m_Shuffled_Remaining_Site_Indices[ii]);
    }

    std::cout << "\n\n";
}
