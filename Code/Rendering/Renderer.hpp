#ifndef _RENDERER_HPP
#define _RENDERER_HPP 1

#include <SFML/Graphics.hpp>
#include "glm/glm.hpp"
#include "OpenGLTranslation.hpp"

#include "Shader.hpp"
#include "Camera.hpp"

#include "VertexArray.hpp"

#include "Mesh.hpp"

class VertexArray;
class Shader;
class Material;

struct TextAlignment
{
    enum Align
    {
        X_Left = 0x01,
        X_Center = 0x02,
        X_Right = 0x03,
        Y_Top = 0x11,
        Y_Center = 0x12,
        Y_Bottom = 0x13,

        //Center_XY = X_Center | Y_Center;
    };

    TextAlignment() : m_X_Alignment(X_Left), m_Y_Alignment(Y_Top) {}
    TextAlignment(Align x_align, Align y_align) : m_X_Alignment(x_align), m_Y_Alignment(y_align) {}

    void mt_Set(TextAlignment::Align x, TextAlignment::Align y);

    int m_X_Alignment = TextAlignment::X_Left;
    int m_Y_Alignment = TextAlignment::Y_Top;

    sf::Vector2f mt_Update_Origin(sf::Text& txt) const;
};

struct TextDrawingParam
{
    int m_Char_Size = 30;
    sf::Font* m_Font = nullptr;
    sf::Vector2f m_Screen_Pos = sf::Vector2f(0.0f, 0.0f);
    TextAlignment m_Alignment;
    sf::Text::Style m_Style = sf::Text::Style::Regular;
    sf::Color m_Color = sf::Color::White;
};

struct MeshDrawParam
{
    PrimitiveType m_Primitive_Type = PrimitiveType::Triangles;
};

class Renderer
{
public:
    static Renderer& smt_Get(void);

    void mt_Initialise(sf::RenderWindow& wnd);

    void mt_Begin_Draw(void);
    void mt_End_Draw(void);

    void mt_Render_Terrain_Part(VertexArray* va, const glm::mat4& model_matrix);
    void mt_Render_Mesh(VertexArray* va, Material* material, const glm::mat4& model_matrix);
    void mt_Render_Mesh(VertexArray* va, const sf::Texture* texture, const glm::mat4& model_matrix);
    void mt_Render_CubeMap(VertexArray* va, unsigned int textures);

    void mt_Render_Particle(const sf::RectangleShape& particle);

    void mt_Draw_Drawable(const sf::Drawable& d, sf::RenderStates states);

    void mt_Draw_Text(const sf::String& str, const sf::Vector2f& pos, sf::Font* font = nullptr);
    void mt_Draw_Text(const sf::String& str, const sf::Vector2f& pos, const sf::Color& text_color);
    void mt_Draw_Text(const sf::String& str, const TextDrawingParam& params);


    Shader m_Shader;
    Camera m_Camera;

    Shader m_CubeMap_Shader;

    Shader m_Terrain_Shader;
    sf::Texture m_Terrain_Tileset;

private:
    sf::RenderWindow* m_Wnd;
    sf::Font m_Default_Font;
    sf::Shader m_SFML_Shader;

    std::map<std::pair<VertexArray*, Material*>, std::vector<glm::mat4>> m_Mesh_Material_Render_Buffer;
    template<typename T>
    inline void mt_Send_To_Shader(Shader* s, std::map<std::string, T>& m)
    {
        for (auto& v : m)
        {
            s->mt_Set_Uniform(v.first, v.second);
        }
    }




    std::map<std::pair<VertexArray*, const sf::Texture*>, std::vector<glm::mat4>> m_Mesh_Render_Buffer;
    std::map<std::pair<VertexArray*, const sf::Texture*>, std::vector<glm::mat4>> m_Particles;
    std::map<VertexArray*, std::vector<glm::mat4>> m_Terrain_Parts;
    std::pair<const VertexArray*, unsigned int> m_CubeMap;

    sf::RenderTexture m_GUI_Screen;


    std::unique_ptr<VertexArray> m_Particle_VA;
    std::unique_ptr<Mesh> m_Pane;
public:
    std::unique_ptr<VertexBuffer> m_Instanced_Buffer;

private:
    Renderer();
};

#endif // _RENDERER_HPP
