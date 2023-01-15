#ifndef _WORLD_GENERATOR_HPP
#define _WORLD_GENERATOR_HPP 1

#include "_Prototype.hpp"

#include "../Submodules/voronoi/src/jc_voronoi.h"



namespace world_gen
{

struct Cell
{
    int m_Site_Id;

    int m_Point_Count;

    sf::Color m_Color;
    int m_Region_Id;
    int m_Altitude;
};

struct Region
{
    int m_Region_Id;
    std::vector<int> m_Cell_Ids;
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

    jcv_diagram m_Diagram;
    std::vector<int> m_Shuffled_Remaining_Site_Indices;
    sf::RenderTexture m_RenderTexture;

    std::vector<world_gen::Cell> m_Cells;
    std::vector<world_gen::Region> m_Regions;
    int m_Region_Count;
};


#endif // _WORLD_GENERATOR_HPP
