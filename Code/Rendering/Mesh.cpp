#include "Mesh.hpp"

#include <fstream>
#include <sstream>

namespace priv
{
struct Face
{
    std::size_t m_Vertex[4];
    std::size_t m_Normal[4];
    std::size_t m_TexCoord[4];
    std::size_t m_Count;
};




void fn_Convert_Faces_Into_Mesh(const std::vector<glm::vec3>& pos,
                                const std::vector<glm::vec3>& normals,
                                const std::vector<glm::vec2>& tex_coords,
                                const std::vector<priv::Face>& faces,
                                Mesh& mesh)
{
    std::vector<float> l_Vertices;
    std::vector<unsigned int> l_Indices;

    for (std::size_t ii = 0; ii < faces.size(); ii++)
    {
        for (std::size_t jj = 0; jj < faces[ii].m_Count; jj++)
        {
            l_Vertices.push_back(pos[faces[ii].m_Vertex[jj]].x);
            l_Vertices.push_back(pos[faces[ii].m_Vertex[jj]].y);
            l_Vertices.push_back(pos[faces[ii].m_Vertex[jj]].z);
            if (tex_coords.size())
            {
                l_Vertices.push_back(tex_coords[faces[ii].m_TexCoord[jj]].x);
                l_Vertices.push_back(tex_coords[faces[ii].m_TexCoord[jj]].y);
            }
            else
            {
                l_Vertices.push_back(0.0f);
                l_Vertices.push_back(0.0f);
            }

            if (normals.size())
            {
                l_Vertices.push_back(normals[faces[ii].m_Normal[jj]].x);
                l_Vertices.push_back(normals[faces[ii].m_Normal[jj]].y);
                l_Vertices.push_back(normals[faces[ii].m_Normal[jj]].z);
            }
            else
            {
                l_Vertices.push_back(0.0f);
                l_Vertices.push_back(0.0f);
                l_Vertices.push_back(0.0f);
            }
        }

        if (faces[ii].m_Count == 4)
        {
            unsigned int l_Offset = l_Indices.size();

            l_Indices.push_back(l_Offset + 0);
            l_Indices.push_back(l_Offset + 1);
            l_Indices.push_back(l_Offset + 2);
            l_Indices.push_back(l_Offset + 2);
            l_Indices.push_back(l_Offset + 0);
            l_Indices.push_back(l_Offset + 3);
        }
        else
        {
            unsigned int l_Offset = l_Indices.size();

            l_Indices.push_back(l_Offset + 0);
            l_Indices.push_back(l_Offset + 1);
            l_Indices.push_back(l_Offset + 2);
        }
    }

    std::cout << "Mesh vertices: " << l_Vertices.size() << '\n';
    for (std::size_t ii = 0; ii < l_Vertices.size() && false; ii += (3+2+3))
    {
        std::cout   << l_Vertices[ii + 0] << ' ' << l_Vertices[ii + 1] << ' ' << l_Vertices[ii + 2] << ' '
                    << l_Vertices[ii + 3] << ' ' << l_Vertices[ii + 4] << ' '
                    << l_Vertices[ii + 5] << ' ' << l_Vertices[ii + 6] << ' ' << l_Vertices[ii + 7] << '\n';
    }
    std::cout << "Mesh indices: " << l_Indices.size() << '\n';
    for (std::size_t ii = 0; ii < l_Indices.size() && false; ii += 3)
    {
        std::cout << l_Indices[ii+0] << ' ' << l_Indices[ii+1] << ' ' << l_Indices[ii+2] << '\n';
    }

    mesh.mt_Load_Vertices(l_Vertices, l_Indices, VertexBufferLayout({
                                                                     VertexBufferLayoutElement("aPos", ShaderDataType::vec3, false),
                                                                     VertexBufferLayoutElement("aTexCoord", ShaderDataType::vec2, false),
                                                                     VertexBufferLayoutElement("aNormal", ShaderDataType::vec3, false)
                                                                    }));
}

bool fn_Extract_Data(std::istream& input,
                     std::vector<glm::vec3>& pos,
                     std::vector<glm::vec3>& normals,
                     std::vector<glm::vec2>& tex_coords,
                     std::vector<Face>& faces)
{
    return true;
}


}






bool fn_Load_Mesh(const std::string& file_name, Mesh& mesh)
{
    std::ifstream l_input_stream(file_name);
    std::vector<glm::vec3> l_pos;
    std::vector<glm::vec3> l_normals;
    std::vector<glm::vec2> l_tex_coords;
    std::vector<priv::Face> l_faces;
    std::string l_line;
    float l_f0, l_f1, l_f2;

    /** Get info from file **/
    while(std::getline(l_input_stream, l_line, '\n'))
    {
        std::stringstream l_ss;
        if (l_line.size() != 0 && l_line[0] != '#')
        {
            l_ss << l_line.substr(2, std::string::npos);
            if (l_line[0] == 'v')
            {
                //l_ss >> l_f0;
                l_ss >> l_f0 >> l_f1 >> l_f2;

                if (l_line[1] == 'n')
                {
                    l_normals.push_back(glm::vec3(l_f0, l_f1, l_f2));
                }
                else if (l_line[1] == 't')
                {
                    l_tex_coords.push_back(glm::vec2(l_f0, l_f1));
                }
                else
                {
                    l_pos.push_back(glm::vec3(l_f0, l_f1, l_f2));
                }
            }
            else if (l_line[0] == 'f')
            {
                priv::Face l_face;
                std::size_t l_current_index;
                l_line = l_line.substr(2, std::string::npos);
                auto l_fn_Extract_Face_Part = [&](std::string line_part, priv::Face& face, std::size_t current_index)
                {
                    std::stringstream l_ss;
                    l_ss << line_part;
                    std::getline(l_ss, line_part, '/');
                    face.m_Vertex[current_index] = std::atoi(line_part.c_str()) - 1;
                    std::getline(l_ss, line_part, '/');
                    face.m_TexCoord[current_index] = std::atoi(line_part.c_str()) - 1;
                    std::getline(l_ss, line_part, '/');
                    face.m_Normal[current_index] = std::atoi(line_part.c_str()) - 1;
                };

                l_current_index = 0;
                while(std::getline(l_ss, l_line, ' '))
                {
                    l_fn_Extract_Face_Part(l_line, l_face, l_current_index);
                    l_current_index++;
                }

                l_face.m_Count = l_current_index;

                l_faces.push_back(l_face);
            }
        }
    }

    /*std::cout << "Vertices: " << l_pos.size() << '\n';
    for (std::size_t ii = 0; ii < l_pos.size(); ii++)
    {
        std::cout << l_pos[ii].x << ' ' << l_pos[ii].y << ' ' << l_pos[ii].z << '\n';
    }
    std::cout << "Normals: " << l_normals.size() << '\n';
    for (std::size_t ii = 0; ii < l_normals.size(); ii++)
    {
        std::cout << l_normals[ii].x << ' ' << l_normals[ii].y << ' ' << l_normals[ii].z << '\n';
    }
    std::cout << "TexCoords: " << l_tex_coords.size() << '\n';
    for (std::size_t ii = 0; ii < l_tex_coords.size(); ii++)
    {
        std::cout << l_tex_coords[ii].x << ' ' << l_tex_coords[ii].y << '\n';
    }
    std::cout << "Faces: " << l_faces.size() << '\n';
    for (std::size_t ii = 0; ii < l_faces.size(); ii++)
    {
        for (std::size_t jj = 0; jj < 3; jj++)
        {
            std::cout << l_faces[ii].m_Vertex[jj] << ' ' << l_faces[ii].m_Normal[jj] << ' ' << l_faces[ii].m_TexCoord[jj] << '\t';
        }
        std::cout << '\n';
    }*/


    /** Convert info to mesh **/
    priv::fn_Convert_Faces_Into_Mesh(l_pos, l_normals, l_tex_coords, l_faces, mesh);

    return true;
}




bool fn_Load_Mesh(const std::string& file_name, std::map<std::string, std::unique_ptr<Mesh>>& mesh)
{
    std::ifstream l_input_stream(file_name);
    std::vector<glm::vec3> l_pos;
    std::vector<glm::vec3> l_normals;
    std::vector<glm::vec2> l_tex_coords;
    std::vector<priv::Face> l_faces;
    std::string l_line;
    std::string l_object_name;
    float l_f0, l_f1, l_f2;
    Mesh* l_mesh;

    /** Get info from file **/
    while(std::getline(l_input_stream, l_line, '\n'))
    {
        std::stringstream l_ss;
        if (l_line.size() != 0 && l_line[0] != '#')
        {
            l_ss << l_line.substr(2, std::string::npos);
            if (l_line[0] == 'v')
            {
                //l_ss >> l_f0;
                l_ss >> l_f0 >> l_f1 >> l_f2;

                if (l_line[1] == 'n')
                {
                    l_normals.push_back(glm::vec3(l_f0, l_f1, l_f2));
                }
                else if (l_line[1] == 't')
                {
                    l_tex_coords.push_back(glm::vec2(l_f0, l_f1));
                }
                else
                {
                    l_pos.push_back(glm::vec3(l_f0, l_f1, l_f2));
                }
            }
            else if (l_line[0] == 'f')
            {
                priv::Face l_face;
                std::size_t l_current_index;
                l_line = l_line.substr(2, std::string::npos);
                auto l_fn_Extract_Face_Part = [&](std::string line_part, priv::Face& face, std::size_t current_index)
                {
                    std::stringstream l_ss;
                    l_ss << line_part;
                    std::getline(l_ss, line_part, '/');
                    face.m_Vertex[current_index] = std::atoi(line_part.c_str()) - 1;
                    std::getline(l_ss, line_part, '/');
                    face.m_TexCoord[current_index] = std::atoi(line_part.c_str()) - 1;
                    std::getline(l_ss, line_part, '/');
                    face.m_Normal[current_index] = std::atoi(line_part.c_str()) - 1;
                };

                l_current_index = 0;
                while(std::getline(l_ss, l_line, ' '))
                {
                    l_fn_Extract_Face_Part(l_line, l_face, l_current_index);
                    l_current_index++;
                }

                l_face.m_Count = l_current_index;

                l_faces.push_back(l_face);
            }
            else if (l_line[0] == 'o')
            {
                if (l_faces.size() > 0)
                {
                    l_mesh = new Mesh;

                    /** Convert info to mesh **/
                    priv::fn_Convert_Faces_Into_Mesh(l_pos, l_normals, l_tex_coords, l_faces, *l_mesh);

                    l_pos.clear();
                    l_normals.clear();
                    l_tex_coords.clear();
                    l_faces.clear();

                    mesh.emplace(l_object_name, std::unique_ptr<Mesh>(l_mesh));
                }

                l_object_name = l_line.substr(2, std::string::npos);
            }
        }
    }

    if (l_faces.size() > 0)
    {
        l_mesh = new Mesh;

        /** Convert info to mesh **/
        priv::fn_Convert_Faces_Into_Mesh(l_pos, l_normals, l_tex_coords, l_faces, *l_mesh);

        l_pos.clear();
        l_normals.clear();
        l_tex_coords.clear();
        l_faces.clear();

        mesh.emplace(l_object_name, std::unique_ptr<Mesh>(l_mesh));
    }

    return true;
}


bool fn_Load_Mesh(const std::string& file_name, Mesh* mesh)
{
    //
}
