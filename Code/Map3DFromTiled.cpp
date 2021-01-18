#include "Map3DFromTiled.hpp"

#include <map>

#include "tinyxml.h"

#include "Rendering/Material.hpp"
#include "Utilities/Log.hpp"

#include <cstdlib>
#include <cstdio>
#include <cstring>


template<typename T>
struct Array2D
{
    Array2D(int w, int h)
    {
        m_Buffer = new T[w * h];
        m_Width = w;
        m_Height = h;
    }

    ~Array2D()
    {
        delete [] m_Buffer;
    }

    T& operator()(int x, int y)
    {
        return m_Buffer[y * m_Width + x];
    }

    int mt_Get_Width(void) const
    {
        return m_Width;
    }

    int mt_Get_Height(void) const
    {
        return m_Height;
    }

private:
    T* m_Buffer;
    int m_Width;
    int m_Height;
};




enum Edge
{
    Norh,
    South,
    Est,
    West,
    Top,
    Bottom,
};

struct Cell
{
    bool m_Exist;
    bool m_Edge[6];
    int m_Edge_Id[6];

    int m_Tile_Id[6];
};

struct Cube
{
    Cube(int x, int y, int z, int width, int height, int depth)
    {
        m_Buffer = new Cell[width * height * depth];
        m_X = x;
        m_Y = y;
        m_Z = z;
        m_Width = width;
        m_Height = height;
        m_Depth = depth;

        for (int ii = 0; ii < width * height * depth; ii++)
        {
            m_Buffer[ii].m_Exist = false;
            m_Buffer[ii].m_Edge[0] = false;
            m_Buffer[ii].m_Edge[1] = false;
            m_Buffer[ii].m_Edge[2] = false;
            m_Buffer[ii].m_Edge[3] = false;
            m_Buffer[ii].m_Edge[4] = false;
            m_Buffer[ii].m_Edge[5] = false;
            m_Buffer[ii].m_Edge_Id[0] = -1;
            m_Buffer[ii].m_Edge_Id[1] = -1;
            m_Buffer[ii].m_Edge_Id[2] = -1;
            m_Buffer[ii].m_Edge_Id[3] = -1;
            m_Buffer[ii].m_Edge_Id[4] = -1;
            m_Buffer[ii].m_Edge_Id[5] = -1;
        }

        m_None.m_Exist = false;
    }

    Cell& operator()(int x, int y, int z)
    {
        if (    (x >= m_X) && (x < m_Width)
            &&  (y >= m_Y) && (y < m_Height)
            &&  (z >= m_Z) && (z < m_Depth))
            return m_Buffer[z * m_Height + y * m_Width + x];
        return m_None;
    }

private:
    Cell m_None;
    Cell* m_Buffer;
    int m_X, m_Y, m_Z;
    int m_Width, m_Height, m_Depth;
};



ReferenceMesh_FromFile::ReferenceMesh_FromFile(const std::string& file_name)
{
    mt_Load(file_name.c_str());
}
ReferenceMesh_FromFile::ReferenceMesh_FromFile(const char* file_name)
{
    mt_Load(file_name);
}









#include <sstream>

struct NCell
{
    //
};

struct NCube
{
    //
};

template<typename T>
void fn_Extract(std::stringstream& ss, T& out)
{
    ss >> out;
}

template<typename T, typename... Args>
void fn_Extract(std::stringstream& ss, T& out, Args... args)
{
    ss >> out;
    fn_Extract(ss, args...);
}

template<typename T, typename... Args>
void fn_Extract(const std::string& str, T& out, Args... args)
{
    std::stringstream l_ss;

    l_ss.str(str);

    fn_Extract(l_ss, out, args...);
}

template<typename T, typename... Args>
void fn_Extract(const char*& str, T& out, Args... args)
{
    std::stringstream l_ss;

    l_ss.str(str);

    fn_Extract(l_ss, out, args...);
}

void toto(const char* file_name)
{
    TiXmlDocument l_Doc;

    if (l_Doc.LoadFile(file_name))
    {
        const TiXmlElement* l_Element = l_Doc.RootElement();
        std::string l_xyz;
        int x, y, z;
        std::stringstream l_ss;

        l_Element->QueryStringAttribute("xyz", &l_xyz);

        l_ss.str(l_xyz);
        fn_Extract(l_ss, x, y, z);

        {
            Cube l_Cube(0, 0, 0, x, y, z);

            for (l_Element = l_Element->FirstChildElement("cell"); l_Element != nullptr; l_Element = l_Element->NextSiblingElement("cell"))
            {
                l_Element->QueryStringAttribute("xyz", &l_xyz);
                fn_Extract(l_xyz, x, y, z);

                Cell& l_Cell = l_Cube(x, y, z);

                //l_Cell
            }
        }
    }
    else
    {
        std::cout << "Can't load file '" << file_name << "': " << l_Doc.ErrorDesc() << '\n';
    }
}






void ReferenceMesh_FromFile::mt_Load(const char* file_name)
{
    TiXmlDocument l_Document;
    std::map<int, std::unique_ptr<Array2D<int>>> l_Map_Descripion;

    toto(file_name);

    return;

    if (l_Document.LoadFile(file_name))
    {
        TiXmlHandle l_Handle = l_Document.RootElement();
        const TiXmlElement* l_Layer = l_Handle.FirstChildElement("layer").ToElement();
        const TiXmlElement* l_Data;
        int l_Cube_Width = 3;
        int l_Cube_Height = 2;
        int l_Cube_Depth = 3;

        for (; l_Layer != nullptr; l_Layer = l_Layer->NextSiblingElement("layer"))
        {
            int l_Layer_Elevation = INT_MAX;
            int l_Layer_Width;
            int l_Layer_Height;
            l_Data = l_Layer->FirstChildElement("data");

            l_Layer->QueryIntAttribute("name", &l_Layer_Elevation);
            l_Layer->QueryIntAttribute("width", &l_Layer_Width);
            l_Layer->QueryIntAttribute("height", &l_Layer_Height);

            l_Map_Descripion[l_Layer_Elevation].reset(new Array2D<int>(l_Layer_Width, l_Layer_Height));

            if (l_Data != nullptr)
            {
                const char* l_Ptr = l_Data->GetText();
                int l_Str_Size = 0;
                int xx, yy;
                char l_Buffer[5];

                //std::cout << "l_Ptr: '" << l_Ptr << "'\n";

                xx = 0;
                yy = 0;
                while (*l_Ptr != '\0')
                {
                    if (    (*l_Ptr == ',')
                        ||  (*l_Ptr == '\n')
                        ||  (*l_Ptr == '\r')
                        ||  (*l_Ptr == ' '))
                    {
                        if (l_Str_Size > 0)
                        {
                            (*l_Map_Descripion[l_Layer_Elevation])(xx, yy) = std::atoi(l_Buffer);
                            std::memset(l_Buffer, 0, sizeof(l_Buffer));
                            l_Str_Size = 0;
                        }

                        if (*l_Ptr == ',')
                        {
                            xx++;
                        }
                        else
                        {
                            xx = 0;
                            yy++;
                        }
                    }
                    else
                    {
                        l_Buffer[l_Str_Size] = *l_Ptr;
                        l_Str_Size++;
                    }

                    l_Ptr++;
                }

                if (l_Str_Size > 0)
                {
                    (*l_Map_Descripion[l_Layer_Elevation])(xx, yy) = std::atoi(l_Buffer);
                    std::memset(l_Buffer, 0, sizeof(l_Buffer));
                    l_Str_Size = 0;
                }
            }
        }

        for (auto& md : l_Map_Descripion)
        {
            std::cout << "Elevation: " << md.first << '\n';
            for (int yy = 0; yy < md.second->mt_Get_Height(); yy++)
            {
                for (int xx = 0; xx < md.second->mt_Get_Width(); xx++)
                {
                    std::cout << (*md.second)(xx, yy) << ' ';
                }
                std::cout << '\n';
            }
        }

        /// Creating cube
        Cube l_Cube(0, 0, 0, l_Cube_Width, l_Cube_Height, l_Cube_Depth);
        for (auto& md : l_Map_Descripion)
        {
            for (int yy = 0; yy < md.second->mt_Get_Height(); yy++)
            {
                for (int xx = 0; xx < md.second->mt_Get_Width(); xx++)
                {
                    int l_Tile_Id = (*md.second)(xx, yy);
                    if (l_Tile_Id != 0)
                    {
                        Cell& l_Cell = l_Cube(xx, md.first, yy);

                        l_Cell.m_Exist = true;
                        l_Cell.m_Tile_Id[0] = l_Tile_Id;
                    }
                }
            }
        }

        /// Creating vertices
        for (int yy = 0; yy < l_Cube_Height; yy++)
        {
            for (int xx = 0; xx < l_Cube_Width; xx++)
            {
                for (int zz = 0; zz < l_Cube_Depth; zz++)
                {
                    Cell& l_Cell = l_Cube(xx, yy, zz);
                    Cell& l_Cell_North = l_Cube(xx, yy, zz - 1);
                    Cell& l_Cell_South = l_Cube(xx, yy, zz + 1);
                    Cell& l_Cell_Est = l_Cube(xx - 1, yy, zz);
                    Cell& l_Cell_West = l_Cube(xx + 1, yy, zz);
                    Cell& l_Cell_Top = l_Cube(xx, yy + 1, zz);
                    Cell& l_Cell_Bottom = l_Cube(xx, yy - 1, zz);

                    if (l_Cell.m_Exist)
                    {
                        if (l_Cell_West.m_Exist)
                        {
                            if (l_Cell_North.m_Exist)
                            {
                                //
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        std::cout << "Can't load file '" << file_name << "': " << l_Document.ErrorDesc() << '\n';
    }
}




















void fn_Map3D(sf::RenderWindow& l_Wnd,
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

    ReferenceMesh_FromFile l_Map_Mesh("../../Assets/Map3DFromTiled/map.tmx");
    VertexArray l_Map_VA;
    Shader l_Shader;
    sf::Texture l_Tileset_Texture;
    Material l_Map_Material;
    Transform l_Map_Transform;
    std::string l_Shader_Path = "../../Assets/Map3DFromTiled/";

    l_Shader.mt_Create(l_Shader_Path + "normalShader.vs", l_Shader_Path + "normalShader.fs");
    l_Map_VA.mt_Add_Vertex_Buffer(l_Map_Mesh.m_VB.get());
    l_Map_VA.mt_Add_Vertex_Buffer(Renderer::smt_Get().m_Instanced_Buffer.get());
    l_Map_VA.mt_Set_Index_Buffer(l_Map_Mesh.m_IB.get());

    l_Tileset_Texture.loadFromFile(l_Shader_Path + "city.png");
    l_Tileset_Texture.setRepeated(true);

    l_Map_Material.m_Shader = &l_Shader;
    l_Map_Material.m_Textures.push_back(&l_Tileset_Texture);

    l_Map_Transform.mt_Reset(glm::vec3(-5.0f, 0.0f, -5.0f), glm::vec3(0.0f), glm::vec3(1.0));

    LOG_dbg("\nvertices count: %d\nindices count: %d", l_Map_Mesh.m_Vertices.size(), l_Map_Mesh.m_Indices.size());

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

            l_Shader.mt_Use();
            Renderer::smt_Get().mt_Render_Mesh(&l_Map_VA, &l_Map_Material, l_Map_Transform.mt_Get_ModelMatrix());

            Renderer::smt_Get().mt_End_Draw();

            l_Wnd.display();
        }

    }
}
