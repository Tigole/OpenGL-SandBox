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
#include "TerrainPreparator.hpp"
#include "CatmullSpline.hpp"

#include "WorldGenerator.hpp"
#include "TerrainElevator.hpp"
#include "AntColony.hpp"
#include "ParticleSystem.hpp"
#include "SoundeRecord.hpp"
#include "Skeleton.hpp"

#include <map>
#include <memory>

using namespace std;



void fn_SandBox(int argc, char** argv)
{
    sf::RenderWindow l_Wnd;
    sf::Event l_Event;
    Camera l_Camera;
    float l_Elapsed_Time;
    float l_Accumulated_Time;
    sf::Clock l_Clk;
    sf::Context l_Context(sf::ContextSettings(24, 0, 8, 4, 6), 10, 10);

    fn_Shader_Waves(l_Wnd, l_Event, l_Camera, l_Elapsed_Time, l_Accumulated_Time, l_Clk);
    //fn_Collision_Test(l_Wnd, l_Event, l_Camera, l_Elapsed_Time, l_Accumulated_Time, l_Clk);
    //fn_Map3D(l_Wnd, l_Event, l_Camera, l_Elapsed_Time, l_Accumulated_Time, l_Clk);
    fn_MeshLoading(l_Wnd, l_Event, l_Camera, l_Elapsed_Time, l_Accumulated_Time, l_Clk, l_Context);
    //fn_CatmullSpline(l_Wnd, l_Event, l_Camera, l_Elapsed_Time, l_Accumulated_Time, l_Clk, l_Context);
    //fn_TerrainPreparator(argc, argv);

    std::map<std::string, std::unique_ptr<Prototype>> l_Prototypes;

#define ADD_TO_PROTOTYPE(prototype_name, prototype_type) l_Prototypes.emplace(prototype_name, std::unique_ptr<Prototype>(new prototype_type));

    //ADD_TO_PROTOTYPE("WorldGenerator", WorldGenerator);
    //ADD_TO_PROTOTYPE("TerrainElevator", TerrainElevator);
    //l_Prototypes.emplace("AntColony", std::unique_ptr<Prototype>(new ant::AntColony<300, 300, 100>));
    //ADD_TO_PROTOTYPE("AntColony", ant::AntColony<5, 5, 3>);
    //ADD_TO_PROTOTYPE("ParticleSystem", ParticleSystem);
    //ADD_TO_PROTOTYPE("SoundRecord", SoundRecord);
    //ADD_TO_PROTOTYPE("2D-Skeleton", Skeleton);

    std::cout << "l_Prototypes.size(): " << l_Prototypes.size() << '\n';
    for (auto& p : l_Prototypes)
    {
        std::cout << p.first << '\n';
        p.second->mt_Init();
        p.second->mt_Run();
    }
}



int main(int argc, char** argv)
{
    fn_SandBox(argc, argv);

    cout << "Hello world!" << endl;
    return 0;
}
