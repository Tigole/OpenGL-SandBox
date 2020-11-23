#include "Renderer.hpp"

#include <string>

#include "Utilities/Log.hpp"

#include "VertexArray.hpp"
#include "Mesh.hpp"
#include "Material.hpp"


#include "glm/gtx/string_cast.hpp"

#include "Utilities/LibCompatibility.hpp"

#include <iostream>


void TextAlignment::mt_Set(TextAlignment::Align x, TextAlignment::Align y)
{
    m_X_Alignment = x;
    m_Y_Alignment = y;
}

sf::Vector2f TextAlignment::mt_Update_Origin(sf::Text& txt) const
{
    sf::FloatRect l_Bounds = txt.getGlobalBounds();
    sf::Vector2f l_Origin = txt.getOrigin();

    if (m_X_Alignment == X_Left)
    {
        l_Origin.x = 0.0f;
    }
    else if (m_X_Alignment == X_Center)
    {
        l_Origin.x = l_Bounds.width / 2;
    }
    else if (m_X_Alignment == X_Right)
    {
        l_Origin.x = l_Bounds.width;
    }

    if (m_Y_Alignment == Y_Top)
    {
        l_Origin.y = 0.0f;
    }
    else if (m_Y_Alignment == Y_Center)
    {
        l_Origin.y = l_Bounds.height / 2;
    }
    else if (m_Y_Alignment == Y_Bottom)
    {
        l_Origin.y = l_Bounds.height;
    }

    txt.setOrigin(l_Origin);

    return txt.getOrigin();
}



Renderer::Renderer() : m_Wnd(nullptr)
{}

void Renderer::mt_Initialise(sf::RenderWindow& wnd)
{
    m_Wnd = &wnd;

    if (!m_Default_Font.loadFromFile("../cour.ttf"))
    {
        LOG_ctr("Failed loading CourrierNew");
    }
    else
    {}

    m_Pane.reset(new ReferenceMesh_Cube(glm::vec3(0.0f), glm::vec3(1.0f)));
    m_Instanced_Buffer.reset(new VertexBuffer(VertexBufferLayout({VertexBufferLayoutElement("aModelMatrix", ShaderDataType::mat4, false)})));

    m_Particle_VA.reset(new VertexArray);
    m_Particle_VA->mt_Add_Vertex_Buffer(m_Pane->m_VB.get());
    m_Particle_VA->mt_Add_Vertex_Buffer(m_Instanced_Buffer.get());
    m_Particle_VA->mt_Set_Index_Buffer(m_Pane->m_IB.get());
}

Renderer& Renderer::smt_Get(void)
{
    static Renderer ls_Renderer;

    return ls_Renderer;
}

void Renderer::mt_Begin_Draw(void)
{
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    m_GUI_Screen.clear(sf::Color::Transparent);
}

void Renderer::mt_End_Draw(void)
{
    sf::Sprite l_GUI_Sprite;

    m_Wnd->setActive(true);

    /// CubeMap
    if (m_CubeMap.first != nullptr && m_CubeMap.second != 0)
    {
        glDepthMask(GL_FALSE);
        m_CubeMap_Shader.mt_Use();
        m_CubeMap_Shader.mt_Set_Uniform("uCameraMatrix", m_Camera.mt_Get_ViewProjection_Matrix());

        m_CubeMap.first->mt_Bind();

        glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMap.second);
        glDrawElementsInstanced(GL_TRIANGLES, m_CubeMap.first->mt_Indice_Count(), GL_UNSIGNED_INT, nullptr, 1);

        m_CubeMap.first->mt_Unbind();
        glDepthMask(GL_TRUE);
    }

    /// Terrain
    m_Terrain_Shader.mt_Use();
    m_Terrain_Shader.mt_Set_Uniform("uCameraMatrix", m_Camera.mt_Get_ViewProjection_Matrix());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Terrain_Tileset.getNativeHandle());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (auto l_it = m_Terrain_Parts.begin(); l_it != m_Terrain_Parts.end(); l_it++)
    {
        VertexArray* l_VA = l_it->first;
        const std::vector<glm::mat4>& l_Matrices = l_it->second;
        const glm::mat4* l_m4 = l_Matrices.data();
        int l_count = l_Matrices.size();

        if (l_Matrices.empty())
            continue;

        l_VA->mt_Bind();

        l_VA->mt_Update_Data("aModelMatrix", &l_Matrices[0], l_Matrices.size() * sizeof(l_Matrices[0]), DrawingUsage::Dynamic);

        glDrawElementsInstanced(GL_TRIANGLES, l_VA->mt_Indice_Count(), GL_UNSIGNED_INT, nullptr, l_Matrices.size());

        l_VA->mt_Unbind();
        l_it->second.resize(0);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /// Entities
    m_Shader.mt_Use();
    m_Shader.mt_Set_Uniform("uCameraMatrix", m_Camera.mt_Get_ViewProjection_Matrix());
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (auto l_it = m_Mesh_Render_Buffer.begin(); l_it != m_Mesh_Render_Buffer.end() && true; l_it++)
    {
        VertexArray* l_VA = l_it->first.first;
        const sf::Texture* l_Texture = l_it->first.second;
        const std::vector<glm::mat4>& l_Matrices = l_it->second;

        if (l_Matrices.empty())
            continue;

        l_VA->mt_Bind();

        l_VA->mt_Update_Data("aModelMatrix", &l_Matrices[0], l_Matrices.size() * sizeof(l_Matrices[0]), DrawingUsage::Dynamic);

        glDrawElementsInstanced(GL_TRIANGLES, l_VA->mt_Indice_Count(), GL_UNSIGNED_INT, nullptr, l_Matrices.size());

        l_VA->mt_Unbind();
        l_it->second.resize(0);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);



    /// Material
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (auto l_it = m_Mesh_Material_Render_Buffer.begin(); l_it != m_Mesh_Material_Render_Buffer.end() && true; l_it++)
    {
        VertexArray* l_VA = l_it->first.first;
        Material* l_Material = l_it->first.second;
        const std::vector<glm::mat4>& l_Matrices = l_it->second;

        if (l_Matrices.empty())
            continue;

        l_Material->m_Shader->mt_Use();
        l_Material->m_Shader->mt_Set_Uniform("uCameraMatrix", m_Camera.mt_Get_ViewProjection_Matrix());

        for (std::size_t ii = 0; ii < l_Material->m_Textures.size(); ii++)
        {
            glActiveTexture(GL_TEXTURE0 + ii);
            glBindTexture(GL_TEXTURE_2D, l_Material->m_Textures[ii]->getNativeHandle());
        }

        mt_Send_To_Shader(l_Material->m_Shader, l_Material->m_Mat2);
        mt_Send_To_Shader(l_Material->m_Shader, l_Material->m_Mat3);
        mt_Send_To_Shader(l_Material->m_Shader, l_Material->m_Mat4);
        mt_Send_To_Shader(l_Material->m_Shader, l_Material->m_Vec2);
        mt_Send_To_Shader(l_Material->m_Shader, l_Material->m_Vec3);
        mt_Send_To_Shader(l_Material->m_Shader, l_Material->m_Vec4);

        l_VA->mt_Bind();

        l_VA->mt_Update_Data("aModelMatrix", &l_Matrices[0], l_Matrices.size() * sizeof(l_Matrices[0]), DrawingUsage::Dynamic);

        glDrawElementsInstanced(GL_TRIANGLES, l_VA->mt_Indice_Count(), GL_UNSIGNED_INT, nullptr, l_Matrices.size());

        l_VA->mt_Unbind();
        l_it->second.resize(0);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    sf::Shader::bind(&m_SFML_Shader);
    m_GUI_Screen.display();
    l_GUI_Sprite.setTexture(m_GUI_Screen.getTexture());
    m_Wnd->draw(l_GUI_Sprite);
}



void Renderer::mt_Render_Terrain_Part(VertexArray* va, const glm::mat4& model_matrix)
{
    m_Terrain_Parts[va].push_back(model_matrix);
}

void Renderer::mt_Render_Mesh(VertexArray* va, Material* material, const glm::mat4& model_matrix)
{
    m_Mesh_Material_Render_Buffer[std::make_pair(va, material)].push_back(model_matrix);
}

void Renderer::mt_Render_Mesh(VertexArray* va, const sf::Texture* texture, const glm::mat4& model_matrix)
{
    m_Mesh_Render_Buffer[std::make_pair(va, texture)].push_back(model_matrix);
}

void Renderer::mt_Render_CubeMap(VertexArray* va, unsigned int textures)
{
    m_CubeMap.first = va;
    m_CubeMap.second = textures;
}



void Renderer::mt_Render_Particle(const sf::RectangleShape& particle)
{
    mt_Render_Mesh(m_Particle_VA.get(), particle.getTexture(), fn_SFML_Transform_To_GLM(particle.getTransform()));
}


void Renderer::mt_Draw_Drawable(const sf::Drawable& d, sf::RenderStates states)
{
    m_Wnd->pushGLStates();

    m_Wnd->draw(d, states);

    m_Wnd->popGLStates();
}



void Renderer::mt_Draw_Text(const sf::String& str, const sf::Vector2f& pos, sf::Font* font)
{
    TextDrawingParam l_Params;

    l_Params.m_Font = font;
    l_Params.m_Screen_Pos = pos;

    mt_Draw_Text(str, l_Params);
}

void Renderer::mt_Draw_Text(const sf::String& str, const sf::Vector2f& pos, const sf::Color& text_color)
{
    TextDrawingParam l_Params;

    l_Params.m_Font = &m_Default_Font;
    l_Params.m_Screen_Pos = pos;
    l_Params.m_Color = text_color;

    mt_Draw_Text(str, l_Params);
}

void Renderer::mt_Draw_Text(const sf::String& str, const TextDrawingParam& params)
{
    sf::Text l_Text;
    sf::Font* l_Font(params.m_Font);

    if (m_GUI_Screen.getSize() != m_Wnd->getSize())
    {
        m_GUI_Screen.create(m_Wnd->getSize());
    }

    l_Text.setString(str);
    if (l_Font == nullptr)
    {
        l_Font = &m_Default_Font;
    }
    l_Text.setFont(*l_Font);
    l_Text.setCharacterSize(params.m_Char_Size);
    l_Text.setStyle(params.m_Style);
    l_Text.setFillColor(params.m_Color);
    params.m_Alignment.mt_Update_Origin(l_Text);
    l_Text.setPosition(params.m_Screen_Pos);

    sf::Shader::bind(&m_SFML_Shader);
    m_GUI_Screen.draw(l_Text, sf::RenderStates::Default);
}
