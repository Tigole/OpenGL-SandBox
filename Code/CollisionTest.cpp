#include "CollisionTest.hpp"

#include <SFML/Graphics.hpp>

#include "Rendering/Renderer.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/Material.hpp"

#include "Utilities/Log.hpp"


struct Entity
{
    glm::vec3 m_Pos;
    Material m_Material;
};

glm::vec3 operator+=(glm::vec3& v, const sf::Vector3f& v2)
{
    v.x += v2.x;
    v.y += v2.y;
    v.z += v2.z;

    return v;
}


void fn_Collision_Test(sf::RenderWindow& l_Wnd,
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

    ReferenceMesh_Cube l_Water_Mesh;
    //ReferenceMesh_Plane l_Water_Mesh(10, 5, 10, 10);
    VertexArray l_Water_VA;
    Shader l_Water_Shader;
    Material l_Water_Material;
    Transform l_Water_Transform;
    std::string l_Water_Shader_Path = "../../Assets/Basic/";
    sf::Vector3f l_Vel;
    float l_Speed = 10.0f;

    std::vector<Entity> l_Entities;

    auto l_fn_Add_Entity = [&](const sf::Color& col, const glm::vec3 pos)
    {
        l_Entities.push_back(Entity());
        l_Entities.back().m_Pos = pos;
        l_Entities.back().m_Material.m_Color["rgb"] = col;
        l_Entities.back().m_Material.m_Shader = &l_Water_Shader;
    };

    l_fn_Add_Entity(sf::Color::Red, glm::vec3(-2.0f, 0.0f, -5.0f));
    l_fn_Add_Entity(sf::Color::Green, glm::vec3(2.0f, 0.0f, -5.0f));
    l_fn_Add_Entity(sf::Color::Blue, glm::vec3(0.0f, 0.0f, -5.0f));
    l_fn_Add_Entity(sf::Color::Cyan, glm::vec3(0.0f, 0.0f, 0.0f));
    l_fn_Add_Entity(sf::Color::Magenta, glm::vec3(-2.0f, 0.0f, 0.0f));
    l_fn_Add_Entity(sf::Color::Yellow, glm::vec3(2.0f, 0.0f, 0.0f));

    l_Water_Shader.mt_Create(l_Water_Shader_Path + "shader.vs", l_Water_Shader_Path + "color.fs");
    l_Water_Shader.mt_Use();
    l_Water_Shader.mt_Set_Uniform("rgb", sf::Color::Red);

    l_Water_VA.mt_Add_Vertex_Buffer(l_Water_Mesh.m_VB.get());
    l_Water_VA.mt_Add_Vertex_Buffer(Renderer::smt_Get().m_Instanced_Buffer.get());
    l_Water_VA.mt_Set_Index_Buffer(l_Water_Mesh.m_IB.get());

    l_Water_Material.m_Shader = &l_Water_Shader;

    l_Water_Transform.mt_Reset(glm::vec3(-2.0f, 0.0f, -5.0f), glm::vec3(0.0f), glm::vec3(1.0));

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
            else if (l_Event.type == sf::Event::KeyPressed)
            {
                sf::Keyboard::Key l_Key = l_Event.key.code;
                if (l_Key == sf::Keyboard::Z)
                {
                    l_Vel.z = -1.0f;
                }
                else if (l_Key == sf::Keyboard::Q)
                {
                    l_Vel.x = -1.0f;
                }
                else if (l_Key == sf::Keyboard::Space)
                {
                    l_Vel.y = 1.0f;
                }
                else if (l_Key == sf::Keyboard::S)
                {
                    l_Vel.z = 1.0f;
                }
                else if (l_Key == sf::Keyboard::D)
                {
                    l_Vel.x = 1.0f;
                }
                else if (l_Key == sf::Keyboard::C)
                {
                    l_Vel.y = -1.0f;
                }
            }
            else if (l_Event.type == sf::Event::KeyReleased)
            {
                sf::Keyboard::Key l_Key = l_Event.key.code;
                if (l_Key == sf::Keyboard::Z || l_Key == sf::Keyboard::S)
                {
                    l_Vel.z = 0.0f;
                }
                else if (l_Key == sf::Keyboard::Q || l_Key == sf::Keyboard::D)
                {
                    l_Vel.x = 0.0f;
                }
                else if (l_Key == sf::Keyboard::Space || l_Key == sf::Keyboard::C)
                {
                    l_Vel.y = 0.0f;
                }
            }
        }

        l_Elapsed_Time = l_Clk.restart().asSeconds();
        l_Accumulated_Time += l_Elapsed_Time;

        //std::cout << l_Vel.x << '\t' << l_Vel.y << '\t' << l_Vel.z << '\n';

        l_Entities[0].m_Pos += l_Vel * l_Speed * l_Elapsed_Time;

        if (l_Wnd.isOpen())
        {
            Renderer::smt_Get().mt_Begin_Draw();

            for (std::size_t ii = 0; ii < l_Entities.size(); ii++)
            {
                l_Water_Transform.mt_Set_Position(l_Entities[ii].m_Pos);

                Renderer::smt_Get().mt_Render_Mesh(&l_Water_VA, &l_Entities[ii].m_Material, l_Water_Transform.mt_Get_ModelMatrix());
            }

            Renderer::smt_Get().mt_End_Draw();

            l_Wnd.display();
        }

    }
}
