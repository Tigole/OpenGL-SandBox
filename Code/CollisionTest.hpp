#ifndef _COLLISION_TEST_HPP
#define _COLLISION_TEST_HPP 1

#include <SFML/Graphics.hpp>
#include "Rendering/Renderer.hpp"

void fn_Collision_Test(sf::RenderWindow& l_Wnd,
                       sf::Event& l_Event,
                       Camera& l_Camera,
                       float l_Elapsed_Time,
                       float l_Accumulated_Time,
                       sf::Clock& l_Clk);

#endif // _COLLISION_TEST_HPP
