#ifndef _SFML_2D_RENDERER_HPP
#define _SFML_2D_RENDERER_HPP 1

#include <SFML/Graphics.hpp>

class Renderer2D
{
private:
    Renderer2D(){ m_Font.loadFromFile("../../Assets/pkmndp.ttf"); }

    static Renderer2D sm_Renderer;

public:

    enum class TextureMode
    {
        None,
        Fullscreen,
        Centered,
        EnsureVisible
    };

    static void smt_Render_Point(sf::RenderTarget& target,
                                 const sf::Vector2f& pos_pix,
                                 const sf::Color& color);

    static void smt_Render_Line(sf::RenderTarget& target,
                                const sf::Vector2f& start_pix,
                                const sf::Vector2f& end_pix,
                                const sf::Color& color);

    static void smt_Render_Rect(sf::RenderTarget& target,
                                const sf::Vector2f& pos_pix,
                                const sf::Vector2f& size_pix,
                                const sf::Color& fill_color);
    static void smt_Render_Rect(sf::RenderTarget& target,
                                const sf::Vector2f& pos_pix,
                                const sf::Vector2f& size_pix,
                                const sf::Color& fill_color,
                                float outline_thikness,
                                const sf::Color& outline_color);

    static void smt_Render_Text(sf::RenderTarget& target,
                                const char* text,
                                const sf::Vector2f& start_pos);
    static void smt_Render_Text(sf::RenderTarget& target,
                                const char* text,
                                const sf::Vector2f& start_pos,
                                unsigned int char_size,
                                const sf::Color& color);

    static void smt_Render_Texture(sf::RenderTarget& target,
                                   const sf::Texture& texture,
                                   TextureMode mode);
    static void smt_Render_Texture(sf::RenderTarget& target,
                                   const sf::Texture& texture,
                                   const sf::Vector2f& pos_pix,
                                   const sf::Vector2f& size_factor);

private:

    sf::Font m_Font;
};





#endif // _SFML_2D_RENDERER_HPP
