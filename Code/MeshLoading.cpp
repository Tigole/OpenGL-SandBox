#include "MeshLoading.hpp"

#include "Rendering/Mesh.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Renderer.hpp"

#include "Utilities/LibCompatibility.hpp"
#include "Utilities/Log.hpp"


void fn_MeshLoading(sf::RenderWindow& l_Wnd,
                       sf::Event& l_Event,
                       Camera& l_Camera,
                       float l_Elapsed_Time,
                       float l_Accumulated_Time,
                       sf::Clock& l_Clk,
                       sf::Context& context)
{
    l_Wnd.create(sf::VideoMode(800, 600), "SandBox", sf::Style::Default, context.getSettings());

    glewInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    Renderer::smt_Get().m_Camera.mt_Set(glm::vec3(0.0f, 1.0f, 5.0f), -18.0f, -83.0f,
                                        CameraData(glm::radians(45.0f), l_Wnd.getSize(), 1.0f, 1000.0f));

    Renderer::smt_Get().m_Camera.mt_Set(glm::vec3(0.0f, 2.0f, 3.0f), -18.0f, -90.0f,
                                        CameraData(glm::radians(45.0f), l_Wnd.getSize(), 1.0f, 100.0f));

    Renderer::smt_Get().mt_Initialise(l_Wnd);

    Mesh* l_Mesh;
    std::map<std::string, std::unique_ptr<Mesh>> l_Mesh_Pool;
    //ReferenceMesh_Cube l_Mesh;
    VertexArray l_Map_VA;
    Shader l_Shader;
    sf::Texture l_Tileset_Texture;
    Material l_Map_Material;
    Transform l_Map_Transform;
    std::string l_Shader_Path = "../../Assets/MeshLoading/";
    glm::vec3 l_Rotation(0.0f);
    bool l_b_Show = true;
    const char* l_Object_File;



    l_Object_File = "../../Assets/MeshLoading/Cubetextured2.obj";
    l_Object_File = "../../Assets/MeshLoading/Terrain.obj";
    //l_Object_File = "../../Assets/MeshLoading/Plane.obj";
    //l_Object_File = "../../Assets/MeshLoading/Cubetextured.obj";
    //l_Object_File = "../../Assets/MeshLoading/Low-Poly_Models.obj";
    std::cout << "loading: " << fn_Load_Mesh(l_Object_File, l_Mesh_Pool);

    l_Mesh = l_Mesh_Pool.begin()->second.get();

    l_Shader.mt_Create(l_Shader_Path + "normalShader.vs", l_Shader_Path + "normalShader.fs");
    Renderer::smt_Get().m_Terrain_Shader.mt_Create(l_Shader_Path + "normalShader.vs", l_Shader_Path + "normalShader.fs");
    l_Map_VA.mt_Add_Vertex_Buffer(l_Mesh->m_VB.get());
    l_Map_VA.mt_Set_Index_Buffer(l_Mesh->m_IB.get());
    l_Map_VA.mt_Add_Vertex_Buffer(Renderer::smt_Get().m_Instanced_Buffer.get());

    l_Tileset_Texture.loadFromFile(l_Shader_Path + "WoodenBox.png");
    l_Tileset_Texture.setRepeated(true);
    Renderer::smt_Get().m_Terrain_Tileset.loadFromFile(l_Shader_Path + "WoodenBox.png");

    l_Map_Material.m_Shader = &l_Shader;
    l_Map_Material.m_Textures.push_back(&l_Tileset_Texture);

    l_Map_Transform.mt_Reset(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f), glm::vec3(1.0));

    LOG_dbg("\nvertices count: %d\nindices count: %d", l_Mesh->m_Vertices.size(), l_Mesh->m_Indices.size());

    for (std::size_t ii = 0; ii < l_Mesh->m_Indices.size() && false; ii++)
    {
        std::cout << "ii: " << l_Mesh->m_Indices[ii]
                  << ' ' << l_Mesh->m_Vertices[ii * 8 + 0]
                  << ' ' << l_Mesh->m_Vertices[ii * 8 + 1]
                  << ' ' << l_Mesh->m_Vertices[ii * 8 + 2]
                  << ' ' << l_Mesh->m_Vertices[ii * 8 + 3]
                  << ' ' << l_Mesh->m_Vertices[ii * 8 + 4]
                  << ' ' << l_Mesh->m_Vertices[ii * 8 + 5]
                  << ' ' << l_Mesh->m_Vertices[ii * 8 + 6]
                  << ' ' << l_Mesh->m_Vertices[ii * 8 + 7]
                  << '\n';
    }

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
            if (l_Event.type == sf::Event::KeyPressed)
            {
                switch(l_Event.key.code)
                {
                    case sf::Keyboard::Z : l_Rotation.x = -1.0f; break;
                    case sf::Keyboard::S : l_Rotation.x = 1.0f; break;
                    case sf::Keyboard::D : l_Rotation.y = 1.0f; break;
                    case sf::Keyboard::Q : l_Rotation.y = -1.0f; break;
                    case sf::Keyboard::A : l_Rotation.z = 1.0f; break;
                    case sf::Keyboard::E : l_Rotation.z = -1.0f; break;
                    case sf::Keyboard::Space : l_b_Show = true; break;
                }
            }
            else if (l_Event.type == sf::Event::KeyReleased)
            {
                if (l_Event.key.code == sf::Keyboard::Z || l_Event.key.code == sf::Keyboard::S)
                {
                    l_Rotation.x = 0.0f;
                }
                if (l_Event.key.code == sf::Keyboard::Q || l_Event.key.code == sf::Keyboard::D)
                {
                    l_Rotation.y = 0.0f;
                }
                if (l_Event.key.code == sf::Keyboard::A || l_Event.key.code == sf::Keyboard::E)
                {
                    l_Rotation.z = 0.0f;
                }
                if (l_Event.key.code == sf::Keyboard::Space)
                {
                    //l_b_Show = false;
                }
            }
        }

        l_Elapsed_Time = l_Clk.restart().asSeconds();
        l_Accumulated_Time += l_Elapsed_Time;

        if (l_Wnd.isOpen())
        {
            Renderer::smt_Get().mt_Begin_Draw();

            //l_Water_Transform.mt_Set_Position(glm::vec3(0.0f, 0.1f * sin(l_Elapsed_Time), 0.0f));
            //l_Map_Transform.mt_Rotate(glm::vec3(0.9f, 0.5f, 0.1f) * l_Elapsed_Time * 0.3f);
            l_Map_Transform.mt_Rotate(l_Rotation * l_Elapsed_Time * 0.5f);

            //l_Shader.mt_Use();
            if (l_b_Show == true)
            {
                Renderer::smt_Get().mt_Render_Mesh(&l_Map_VA, &l_Map_Material, l_Map_Transform.mt_Get_ModelMatrix());
            }
            //Renderer::smt_Get().mt_Render_Terrain_Part(&l_Map_VA, l_Map_Transform.mt_Get_ModelMatrix());

            Renderer::smt_Get().mt_End_Draw();

            l_Wnd.display();
        }

    }
}
