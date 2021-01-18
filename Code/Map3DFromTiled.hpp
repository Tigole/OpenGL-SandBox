#ifndef _MAP_3D_FROM_TILED
#define _MAP_3D_FROM_TILED 1

#include "Rendering/Mesh.hpp"

#include <SFML/Graphics.hpp>
#include "Rendering/Renderer.hpp"


class ReferenceMesh_FromFile : public ReferenceMesh
{
public:
    ReferenceMesh_FromFile(const std::string& file_name);
    ReferenceMesh_FromFile(const char* file_name);

private:
    void mt_Load(const char* file_name);
};

void fn_Map3D(sf::RenderWindow& l_Wnd,
                       sf::Event& l_Event,
                       Camera& l_Camera,
                       float l_Elapsed_Time,
                       float l_Accumulated_Time,
                       sf::Clock& l_Clk);


#endif // _MAP_3D_FROM_TILED
