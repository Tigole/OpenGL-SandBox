#include "ShaderWaves.hpp"

#include <SFML/Graphics.hpp>

#include "Rendering/Renderer.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/Material.hpp"

#include "Utilities/Log.hpp"

void fn_Shader_Waves(sf::RenderWindow& l_Wnd,
                     sf::Event& l_Event,
                     Camera& l_Camera,
                     float l_Elapsed_Time,
                     float l_Accumulated_Time,
                     sf::Clock& l_Clk)
{
    l_Wnd.create(sf::VideoMode(800, 600), "SandBox");


    glewInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    Renderer::smt_Get().m_Camera.mt_Set(glm::vec3(0.0f, 1.0f, 5.0f), -18.0f, -83.0f,
                                        CameraData(glm::radians(45.0f), l_Wnd.getSize(), 1.0f, 1000.0f));

    Renderer::smt_Get().m_Camera.mt_Set(glm::vec3(0.0f, 2.0f, 5.0f), -18.0f, -90.0f,
                                        CameraData(glm::radians(45.0f), l_Wnd.getSize(), 1.0f, 1000.0f));

    Renderer::smt_Get().mt_Initialise(l_Wnd);

    ReferenceMesh_Plane l_Water_Mesh(10, 5, 10, 10);
    VertexArray l_Water_VA;
    Shader l_Water_Shader;
    sf::Texture l_Water_Texture;
    Material l_Water_Material;
    Transform l_Water_Transform;
    std::string l_Water_Shader_Path = "../../Assets/WaterShader/";

    l_Water_Shader.mt_Create(l_Water_Shader_Path + "normalShader.vs", l_Water_Shader_Path + "normalShader.fs");
    l_Water_VA.mt_Add_Vertex_Buffer(l_Water_Mesh.m_VB.get());
    l_Water_VA.mt_Add_Vertex_Buffer(Renderer::smt_Get().m_Instanced_Buffer.get());
    l_Water_VA.mt_Set_Index_Buffer(l_Water_Mesh.m_IB.get());

    l_Water_Texture.loadFromFile(l_Water_Shader_Path + "Texture.png");
    l_Water_Texture.setRepeated(true);

    l_Water_Material.m_Shader = &l_Water_Shader;
    l_Water_Material.m_Textures.push_back(&l_Water_Texture);

    l_Water_Transform.mt_Reset(glm::vec3(-5.0f, 0.0f, -5.0f), glm::vec3(0.0f), glm::vec3(1.0));

    LOG_dbg("\nvertices count: %d\nindices count: %d", l_Water_Mesh.m_Vertices.size(), l_Water_Mesh.m_Indices.size());

    l_Elapsed_Time = 0.0f;
    l_Accumulated_Time = 0.0f;
    l_Clk.restart();
    while(l_Wnd.isOpen())
    {
        while(l_Wnd.pollEvent(l_Event))
        {
            if (l_Event.type == sf::Event::Closed)
            {
                l_Wnd.close();
            }
        }

        l_Elapsed_Time = l_Clk.restart().asSeconds();
        l_Accumulated_Time += l_Elapsed_Time;

        if (l_Wnd.isOpen())
        {
            Renderer::smt_Get().mt_Begin_Draw();

            //l_Water_Transform.mt_Set_Position(glm::vec3(0.0f, 0.1f * sin(l_Elapsed_Time), 0.0f));

            l_Water_Shader.mt_Use();
            l_Water_Shader.mt_Set_Uniform("t", l_Accumulated_Time);
            l_Water_Shader.mt_Set_Uniform("wave_amplitude", 0.1f);
            l_Water_Shader.mt_Set_Uniform("wave_frequency", 2.0f);
            Renderer::smt_Get().mt_Render_Mesh(&l_Water_VA, &l_Water_Material, l_Water_Transform.mt_Get_ModelMatrix());

            Renderer::smt_Get().mt_End_Draw();

            l_Wnd.display();
        }

    }
}
