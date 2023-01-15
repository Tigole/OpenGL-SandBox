#ifndef _TERRAIN_ELEVATOR_HPP
#define _TERRAIN_ELEVATOR_HPP 1

#include "_Prototype.hpp"

class TerrainElevator : public Prototype
{
public:
    TerrainElevator();

    bool mt_Specialized_Init() override;

    void mt_Handle_Event(const sf::Event& event) override;
    void mt_Update(float elapsed_time) override;
    void mt_Draw(sf::RenderTarget& target) override;
};

#endif // _TERRAIN_ELEVATOR_HPP
