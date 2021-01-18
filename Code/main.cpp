#include <iostream>

#include <SFML/Graphics.hpp>

#include "Rendering/Renderer.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/Material.hpp"

#include "Utilities/Log.hpp"



#include "ShaderWaves.hpp"
#include "CollisionTest.hpp"
#include "Map3DFromTiled.hpp"
#include "MeshLoading.hpp"

using namespace std;



void fn_SandBox(void)
{
    sf::RenderWindow l_Wnd;
    sf::Event l_Event;
    Camera l_Camera;
    float l_Elapsed_Time;
    float l_Accumulated_Time;
    sf::Clock l_Clk;
    sf::Context l_Context(sf::ContextSettings(24, 0, 8, 4, 6), 10, 10);

    //fn_Shader_Waves(l_Wnd, l_Event, l_Camera, l_Elapsed_Time, l_Accumulated_Time, l_Clk);
    //fn_Collision_Test(l_Wnd, l_Event, l_Camera, l_Elapsed_Time, l_Accumulated_Time, l_Clk);
    //fn_Map3D(l_Wnd, l_Event, l_Camera, l_Elapsed_Time, l_Accumulated_Time, l_Clk);
    fn_MeshLoading(l_Wnd, l_Event, l_Camera, l_Elapsed_Time, l_Accumulated_Time, l_Clk, l_Context);
}



int main()
{
    fn_SandBox();

    cout << "Hello world!" << endl;
    return 0;
}
