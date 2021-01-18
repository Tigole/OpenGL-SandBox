#ifndef _SHADER_WAVES_HPP
#define _SHADER_WAVES_HPP 1

#include <SFML/Graphics.hpp>
#include "Rendering/Renderer.hpp"

void fn_Shader_Waves(sf::RenderWindow& l_Wnd,
                     sf::Event& l_Event,
                     Camera& l_Camera,
                     float l_Elapsed_Time,
                     float l_Accumulated_Time,
                     sf::Clock& l_Clk);

#endif // _SHADER_WAVES_HPP
