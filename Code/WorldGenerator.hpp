#ifndef _WORLD_GENERATOR_HPP
#define _WORLD_GENERATOR_HPP 1

#include "_Prototype.hpp"

#include "../Submodules/voronoi/src/jc_voronoi.h"



namespace world_gen
{

enum class CellType
{
    PrimaryPoint,
    RelaxedPoint,
    Edge,
    EdgeJunction,
    SiteContent
};

struct Cell
{
    CellType m_Type = CellType::SiteContent;
    //int m_Site_Id = -1;
    sf::Color m_Color = sf::Color::Black;
};


}


class WorldGenerator : public Prototype
{
public:
    WorldGenerator();
    ~WorldGenerator();

    bool mt_Specialized_Init() override;

    void mt_Handle_Event(const sf::Event& event) override;
    void mt_Update(float elapsed_time) override;
    void mt_Draw(sf::RenderTarget& target) override;

private:

    void mt_Generate_Diagram();

    void mt_Generate_Points_Position_Default(std::vector<jcv_point>& points, const jcv_rect& boundings);
    void mt_Generate_Points_Position_Center_Density(std::vector<jcv_point>& points, const jcv_rect& boundings);

    jcv_diagram m_Diagram;
    std::vector<std::vector<world_gen::Cell>> m_Cells;
    void mt_Resize_Cells_Container(const jcv_rect& boundings);
    world_gen::Cell& mt_Get_Cell(int xx, int yy);

    void mt_Set_Cell(int xx, int yy, world_gen::CellType cell_type, const sf::Color& cell_color);
    void mt_Set_Cell_PrimaryPoint(int xx, int yy);
    void mt_Set_Cell_RelaxedPoint(int xx, int yy);
    void mt_Set_Cell_Edge(int xx, int yy);
    void mt_Set_Cell_EdgeJunction(int xx, int yy);
    void mt_Set_Cell_SiteContent(int xx, int yy);

    void mt_Get_Line(const sf::Vector2f& start, const sf::Vector2f& end, std::vector<sf::Vector2f>& line);

    void mt_Save_Image(const char* file_name, int width, int height);
    void mt_Draw_Line(sf::Image& image, const sf::Vector2f& start, const sf::Vector2f& end, const sf::Color& color);

    sf::Texture m_Texture;
};


#endif // _WORLD_GENERATOR_HPP
