#ifndef _MESH_HPP
#define _MESH_HPP 1

#include "glm/glm.hpp"
#include "Math/Transform.hpp"

#include "VertexArray.hpp"

#include <vector>
#include <iostream>
#include <algorithm>

struct Mesh
{
    std::unique_ptr<VertexBuffer> m_VB;
    std::unique_ptr<IndexBuffer> m_IB;

    std::vector<float> m_Vertices;
    std::vector<uint32_t> m_Indices;
};

class ReferenceMesh : public Mesh
{
public:
    virtual ~ReferenceMesh() {}

protected:
    void mt_Load_Vertices(const std::vector<float> vertices, const std::vector<uint32_t> indices, const VertexBufferLayout& layout)
    {
        m_Vertices = vertices;
        m_Indices = indices;

        m_VB.reset(new VertexBuffer(m_Vertices, layout));
        m_IB.reset(new IndexBuffer(m_Indices));
    }

    void mt_Hard_Transform(const Transform& t, const std::vector<std::string>& elements)
    {
        const VertexBufferLayout& l_Layout = m_VB->mt_Get_Layout();
        const std::vector<VertexBufferLayoutElement>& l_Elements = l_Layout.mt_Get_Elements();
        std::vector<const VertexBufferLayoutElement*> l_Tgt_Elements;

        for (std::size_t ii = 0; ii < elements.size(); ii++)
        {
            auto l_it = std::find_if(l_Elements.begin(), l_Elements.end(), [&](const VertexBufferLayoutElement& e) {return e.m_Name == elements[ii];});
            if (l_it != l_Elements.end())
            {
                l_Tgt_Elements.push_back(&l_Elements[ii]);
            }
        }

        /*for (std::size_t ii = 0; ii < m_Vertices.size(); ii += 8)
        {
            std::cout << ii << ' ' << std::fixed;
            for (std::size_t jj = 0; jj < 8; jj++)
                std::cout << m_Vertices[ii + jj] << ' ';
            std::cout << '\n';
        }*/

        for (std::size_t ii = 0; ii < m_Vertices.size(); ii += l_Layout.mt_Get_Stride() / sizeof(float))
        {
            for (std::size_t jj = 0; jj < l_Tgt_Elements.size(); jj++)
            {
                if (l_Tgt_Elements[jj]->m_Type == ShaderDataType::vec3)
                {
                    std::size_t l_Index = l_Tgt_Elements[jj]->m_Offset + ii;
                    glm::vec3 l_Vec(glm::vec3(m_Vertices[l_Index], m_Vertices[l_Index + 1], m_Vertices[l_Index + 2]));

                    l_Vec = t.mt_Transform(l_Vec);

                    m_Vertices[l_Index] = l_Vec.x;
                    m_Vertices[l_Index + 1] = l_Vec.y;
                    m_Vertices[l_Index + 2] = l_Vec.z;
                }
            }
        }

        /*for (std::size_t ii = 0; ii < m_Vertices.size(); ii += 8)
        {
            std::cout << ii << ' ' << std::fixed;
            for (std::size_t jj = 0; jj < 8; jj++)
                std::cout << m_Vertices[ii + jj] << ' ';
            std::cout << '\n';
        }*/

        m_VB->mt_Update_Data(m_Vertices, DrawingUsage::Static);
    }

    void mt_Concatenare(const Mesh& m)
    {
        uint32_t l_Index_offset = m_Indices.size();
        for (std::size_t ii = 0; ii < m.m_Vertices.size(); ii++)
        {
            m_Vertices.push_back(m.m_Vertices[ii]);
        }
        for (std::size_t ii = 0; ii < m.m_Indices.size(); ii++)
        {
            m_Indices.push_back(m.m_Indices[ii] + l_Index_offset);
        }
    }
};

class ReferenceMesh_Cube : public ReferenceMesh
{
public:
    ReferenceMesh_Cube()
    {
        mt_Set(glm::vec3(0.0f), glm::vec3(1.0f));
    }
    ReferenceMesh_Cube(const glm::vec3& origin, const glm::vec3& edge_size)
    {
        mt_Set(origin, edge_size);
    }

private:
    void mt_Set(const glm::vec3& origin, const glm::vec3& edge_size)
    {
        Transform l_Transform;
        std::vector<float> l_Vertices =
        {
            /// Front
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, 0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,    1.0f, 0.0f,     0.0f, 0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, 0.0f, -1.0f,

            /// Back
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,    1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,    1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,    1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,     0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,

            /// Left ?
            -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,

            /// Right ?
             0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 0.0f, 0.0f,

            /// Bottom ?
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     0.0f, -1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,    1.0f, 1.0f,     0.0f, -1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,    1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,    1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     0.0f, -1.0f, 0.0f,

            /// Top ?
            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
        };
        std::vector<uint32_t> l_Indices;

        for (std::size_t ii = 0; ii < 6*6; ii++)
        {
            l_Indices.push_back(ii);
        }

        //l_Transform.mt_Set_Origin(origin);
        l_Transform.mt_Set_Position(origin);
        l_Transform.mt_Set_Scale(edge_size);

        mt_Load_Vertices(l_Vertices, l_Indices, VertexBufferLayout({VertexBufferLayoutElement("aPos", ShaderDataType::vec3, false),
                                                                    VertexBufferLayoutElement("aTexCoord", ShaderDataType::vec2, false),
                                                                    VertexBufferLayoutElement("aNormal", ShaderDataType::vec3, false)}));
        mt_Hard_Transform(l_Transform, {"aPos", "aNormal"});
    }
};

class ReferenceMesh_HexagonPrism : public ReferenceMesh
{
public:
    ReferenceMesh_HexagonPrism()
    {
        mt_Set(1.0f, 1.0f);
    }

    ReferenceMesh_HexagonPrism(float radius, float height)
    {
        mt_Set(radius, height);
    }

private:
    void mt_Set(float radius, float height)
    {
        glm::vec3 l_Pos;
        glm::vec3 l_Normal;
        glm::vec2 l_TexCoord;
        float l_PI(3.1416f);
        float l_2PI(2.0f*l_PI);
        std::vector<float> l_Vertices;
        std::vector<uint32_t> l_Indices;

        for (std::size_t ii = 0; ii < 6; ii++)
        {
#if 1
            /// Bottom
            l_TexCoord.x = 0.0f;
            l_TexCoord.y = 0.0f;
            l_Pos.y = 0.0f;
            l_Pos.x = 0.0f;
            l_Pos.z = 0.0f;
            l_Indices.push_back(l_Indices.size());
            l_Vertices.push_back(l_Pos.x); l_Vertices.push_back(l_Pos.y); l_Vertices.push_back(l_Pos.z);
            l_Vertices.push_back(l_TexCoord.x); l_Vertices.push_back(l_TexCoord.y);
            l_Vertices.push_back(l_Normal.x); l_Vertices.push_back(l_Normal.y); l_Vertices.push_back(l_Normal.z);

            l_TexCoord.x = 1.0f;
            l_TexCoord.y = 0.0f;
            l_Pos.x = radius * cos(ii * l_2PI / 6);
            l_Pos.z = radius * sin(ii * l_2PI / 6);
            l_Indices.push_back(l_Indices.size());
            l_Vertices.push_back(l_Pos.x); l_Vertices.push_back(l_Pos.y); l_Vertices.push_back(l_Pos.z);
            l_Vertices.push_back(l_TexCoord.x); l_Vertices.push_back(l_TexCoord.y);
            l_Vertices.push_back(l_Normal.x); l_Vertices.push_back(l_Normal.y); l_Vertices.push_back(l_Normal.z);

            l_Pos.x = radius * cos((ii+1) * l_2PI / 6);
            l_Pos.z = radius * sin((ii+1) * l_2PI / 6);
            l_Indices.push_back(l_Indices.size());
            l_Vertices.push_back(l_Pos.x); l_Vertices.push_back(l_Pos.y); l_Vertices.push_back(l_Pos.z);
            l_Vertices.push_back(l_TexCoord.x); l_Vertices.push_back(l_TexCoord.y);
            l_Vertices.push_back(l_Normal.x); l_Vertices.push_back(l_Normal.y); l_Vertices.push_back(l_Normal.z);
#endif // 0
#if 1
            /// Top
            l_TexCoord.x = 0.0f;
            l_TexCoord.y = 0.0f;
            l_Pos.y = height;
            l_Pos.x = 0.0f;
            l_Pos.z = 0.0f;
            l_Indices.push_back(l_Indices.size());
            l_Vertices.push_back(l_Pos.x); l_Vertices.push_back(l_Pos.y); l_Vertices.push_back(l_Pos.z);
            l_Vertices.push_back(l_TexCoord.x); l_Vertices.push_back(l_TexCoord.y);
            l_Vertices.push_back(l_Normal.x); l_Vertices.push_back(l_Normal.y); l_Vertices.push_back(l_Normal.z);

            l_TexCoord.x = 1.0f;
            l_TexCoord.y = 0.0f;
            l_Pos.x = radius * cos(ii * l_2PI / 6);
            l_Pos.z = radius * sin(ii * l_2PI / 6);
            l_Indices.push_back(l_Indices.size());
            l_Vertices.push_back(l_Pos.x); l_Vertices.push_back(l_Pos.y); l_Vertices.push_back(l_Pos.z);
            l_Vertices.push_back(l_TexCoord.x); l_Vertices.push_back(l_TexCoord.y);
            l_Vertices.push_back(l_Normal.x); l_Vertices.push_back(l_Normal.y); l_Vertices.push_back(l_Normal.z);

            l_Pos.x = radius * cos((ii+1) * l_2PI / 6);
            l_Pos.z = radius * sin((ii+1) * l_2PI / 6);
            l_Indices.push_back(l_Indices.size());
            l_Vertices.push_back(l_Pos.x); l_Vertices.push_back(l_Pos.y); l_Vertices.push_back(l_Pos.z);
            l_Vertices.push_back(l_TexCoord.x); l_Vertices.push_back(l_TexCoord.y);
            l_Vertices.push_back(l_Normal.x); l_Vertices.push_back(l_Normal.y); l_Vertices.push_back(l_Normal.z);
#endif // 0
#if 1
            /// Face
            l_TexCoord.x = 1.0f;
            l_TexCoord.y = 1.0f;
            l_Pos.y = 0.0f;
            l_Pos.x = radius * cos(ii * l_2PI / 6);
            l_Pos.z = radius * sin(ii * l_2PI / 6);
            l_Indices.push_back(l_Indices.size());
            l_Vertices.push_back(l_Pos.x); l_Vertices.push_back(l_Pos.y); l_Vertices.push_back(l_Pos.z);
            l_Vertices.push_back(l_TexCoord.x); l_Vertices.push_back(l_TexCoord.y);
            l_Vertices.push_back(l_Normal.x); l_Vertices.push_back(l_Normal.y); l_Vertices.push_back(l_Normal.z);

            l_Pos.y = 0.0f;
            l_Pos.x = radius * cos((ii+1) * l_2PI / 6);
            l_Pos.z = radius * sin((ii+1) * l_2PI / 6);
            l_Indices.push_back(l_Indices.size());
            l_Vertices.push_back(l_Pos.x); l_Vertices.push_back(l_Pos.y); l_Vertices.push_back(l_Pos.z);
            l_Vertices.push_back(l_TexCoord.x); l_Vertices.push_back(l_TexCoord.y);
            l_Vertices.push_back(l_Normal.x); l_Vertices.push_back(l_Normal.y); l_Vertices.push_back(l_Normal.z);

            l_TexCoord.x = 0.0f;
            l_TexCoord.y = 1.0f;
            l_Pos.y = height;
            l_Pos.x = radius * cos((ii+1) * l_2PI / 6);
            l_Pos.z = radius * sin((ii+1) * l_2PI / 6);
            l_Indices.push_back(l_Indices.size());
            l_Vertices.push_back(l_Pos.x); l_Vertices.push_back(l_Pos.y); l_Vertices.push_back(l_Pos.z);
            l_Vertices.push_back(l_TexCoord.x); l_Vertices.push_back(l_TexCoord.y);
            l_Vertices.push_back(l_Normal.x); l_Vertices.push_back(l_Normal.y); l_Vertices.push_back(l_Normal.z);




            l_TexCoord.x = 1.0f;
            l_TexCoord.y = 1.0f;
            l_Pos.y = 0.0f;
            l_Pos.x = radius * cos(ii * l_2PI / 6);
            l_Pos.z = radius * sin(ii * l_2PI / 6);
            l_Indices.push_back(l_Indices.size());
            l_Vertices.push_back(l_Pos.x); l_Vertices.push_back(l_Pos.y); l_Vertices.push_back(l_Pos.z);
            l_Vertices.push_back(l_TexCoord.x); l_Vertices.push_back(l_TexCoord.y);
            l_Vertices.push_back(l_Normal.x); l_Vertices.push_back(l_Normal.y); l_Vertices.push_back(l_Normal.z);

            l_TexCoord.x = 0.0f;
            l_TexCoord.y = 1.0f;
            l_Pos.y = height;
            l_Pos.x = radius * cos(ii * l_2PI / 6);
            l_Pos.z = radius * sin(ii * l_2PI / 6);
            l_Indices.push_back(l_Indices.size());
            l_Vertices.push_back(l_Pos.x); l_Vertices.push_back(l_Pos.y); l_Vertices.push_back(l_Pos.z);
            l_Vertices.push_back(l_TexCoord.x); l_Vertices.push_back(l_TexCoord.y);
            l_Vertices.push_back(l_Normal.x); l_Vertices.push_back(l_Normal.y); l_Vertices.push_back(l_Normal.z);

            l_Pos.y = height;
            l_Pos.x = radius * cos((ii+1) * l_2PI / 6);
            l_Pos.z = radius * sin((ii+1) * l_2PI / 6);
            l_Indices.push_back(l_Indices.size());
            l_Vertices.push_back(l_Pos.x); l_Vertices.push_back(l_Pos.y); l_Vertices.push_back(l_Pos.z);
            l_Vertices.push_back(l_TexCoord.x); l_Vertices.push_back(l_TexCoord.y);
            l_Vertices.push_back(l_Normal.x); l_Vertices.push_back(l_Normal.y); l_Vertices.push_back(l_Normal.z);
#endif // 0
        }
        mt_Load_Vertices(l_Vertices, l_Indices,  VertexBufferLayout({VertexBufferLayoutElement("aPos", ShaderDataType::vec3, false),
                                                                      VertexBufferLayoutElement("aTexCoord", ShaderDataType::vec2, false),
                                                                      VertexBufferLayoutElement("aNormal", ShaderDataType::vec3, false)}));
    }
};

class ReferenceMesh_Pane : public ReferenceMesh
{
public:
    ReferenceMesh_Pane(float w, float h)
    {
        mt_Set(w, h);
    }

private:
    void mt_Set(float w, float h)
    {
        std::vector<float> l_Vertices;
        std::vector<uint32_t> l_Indices;

        l_Vertices.push_back(0.0f); l_Vertices.push_back(0.0f); l_Vertices.push_back(0.0f);
        l_Vertices.push_back(w); l_Vertices.push_back(0.0f); l_Vertices.push_back(0.0f);
        l_Vertices.push_back(w); l_Vertices.push_back(h); l_Vertices.push_back(0.0f);
        l_Vertices.push_back(0.0f); l_Vertices.push_back(0.0f); l_Vertices.push_back(0.0f);
        l_Vertices.push_back(0.0f); l_Vertices.push_back(h); l_Vertices.push_back(0.0f);
        l_Vertices.push_back(w); l_Vertices.push_back(h); l_Vertices.push_back(0.0f);

        l_Indices.push_back(0);
        l_Indices.push_back(1);
        l_Indices.push_back(2);
        l_Indices.push_back(3);
        l_Indices.push_back(4);
        l_Indices.push_back(5);

        mt_Load_Vertices(m_Vertices, l_Indices, VertexBufferLayout({VertexBufferLayoutElement("aPos", ShaderDataType::vec3, false),
                                                                      VertexBufferLayoutElement("aTexCoord", ShaderDataType::vec2, false),
                                                                      VertexBufferLayoutElement("aNormal", ShaderDataType::vec3, false)}));
    }
};

void fn_Create_Mesh_Square(Mesh& m, float x_edge, float y_edge, float z_edge);
void fn_Create_Mesh_Hexagon(Mesh& m, float radius);
void fn_Create_Mesh_Pane(Mesh& m, float x_edge, float y_edge);
void fn_Create_Mesh_Hexagon_Prism(Mesh& m, float radius, float height);

#endif // _MESH_HPP
