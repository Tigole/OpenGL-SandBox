#ifndef _WORLD_GENERATOR_HPP
#define _WORLD_GENERATOR_HPP 1

#include "_Prototype.hpp"

#include "../Submodules/voronoi/src/jc_voronoi.h"
#include "Utilities/Array2D.hpp"


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
    sf::Color m_Color = sf::Color::Black;
    std::vector<int> m_Site_Ids {};
};

class VoronoiMapInitializer
{
public:

    void mt_Generate(std::vector<jcv_point>& points, const jcv_rect& boundings);
};

class VoronoiMapGenerator
{
public:

    void mt_Generate(const sf::Vector2u& map_size, Array2D<Cell>& map_content);

private:

    void mt_Set_Cell(Array2D<Cell>& map_content, int xx, int yy, world_gen::CellType cell_type, const sf::Color& cell_color, int site_id);
    void mt_Set_Cell_PrimaryPoint(Array2D<Cell>& map_content, int xx, int yy, int site_id);
    void mt_Set_Cell_RelaxedPoint(Array2D<Cell>& map_content, int xx, int yy, int site_id);
    void mt_Set_Cell_Edge(Array2D<Cell>& map_content, int xx, int yy, int site_id);
    void mt_Set_Cell_EdgeJunction(Array2D<Cell>& map_content, int xx, int yy, int site_id);
    void mt_Set_Cell_SiteContent(Array2D<Cell>& map_content, int xx, int yy, int site_id);

    void mt_Get_Line(const sf::Vector2f& start, const sf::Vector2f& end, std::vector<sf::Vector2f>& line);
};


}






enum class CityCellType
{
    None,
    Road,
    Wall,
    HouseFloor,
};

struct CityCell
{
    CityCellType m_Type = CityCellType::None;
};

void fn_City_Test(const Array2D<world_gen::Cell>& source, Array2D<CityCell>& dest);
void fn_City_Test_Make_Roads(const Array2D<world_gen::Cell>& source, Array2D<CityCell>& dest);
void fn_City_Test_Make_Walls(const Array2D<world_gen::Cell>& source, Array2D<CityCell>& dest);
void fn_City_Test_Make_Houses(const Array2D<world_gen::Cell>& source, Array2D<CityCell>& dest);



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

    Array2D<world_gen::Cell> m_Cells;

    void mt_Save_Image(const char* file_name);

    void mt_City_Test(Array2D<world_gen::Cell>& cells);

    void mt_Water_Test(void);

    sf::Texture m_Texture;
};


#endif // _WORLD_GENERATOR_HPP
