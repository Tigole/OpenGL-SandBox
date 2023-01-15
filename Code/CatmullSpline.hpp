#ifndef _CATMULL_SPLINE_HPP
#define _CATMULL_SPLINE_HPP 1

#include "Rendering/Mesh.hpp"

#include <SFML/Graphics.hpp>
#include "Rendering/Renderer.hpp"

void fn_CatmullSpline(sf::RenderWindow& l_Wnd,
                       sf::Event& l_Event,
                       Camera& l_Camera,
                       float l_Elapsed_Time,
                       float l_Accumulated_Time,
                       sf::Clock& l_Clk,
                       sf::Context& context);


#endif // _CATMULL_SPLINE_HPP
