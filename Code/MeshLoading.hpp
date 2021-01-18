#ifndef _MESH_LOADING_HPP
#define _MESH_LOADING_HPP 1

#include "Rendering/Mesh.hpp"

#include <SFML/Graphics.hpp>
#include "Rendering/Renderer.hpp"

void fn_MeshLoading(sf::RenderWindow& l_Wnd,
                       sf::Event& l_Event,
                       Camera& l_Camera,
                       float l_Elapsed_Time,
                       float l_Accumulated_Time,
                       sf::Clock& l_Clk,
                       sf::Context& context);



#endif // _MESH_LOADING_HPP
