#include "SFML_2D_Renderer.hpp"

Renderer2D Renderer2D::sm_Renderer;


void Renderer2D::smt_Render_Point(sf::RenderTarget& target,
                     const sf::Vector2f& pos_pix,
                     const sf::Color& color)
{
    sf::Vertex l_Vertex;

    l_Vertex.color = color;
    l_Vertex.position = pos_pix;

    target.draw(&l_Vertex, 1, sf::PrimitiveType::Points);
}


void Renderer2D::smt_Render_Rect(sf::RenderTarget& target, const sf::Vector2f& pos_pix, const sf::Vector2f& size_pix, const sf::Color& fill_color)
{
    smt_Render_Rect(target, pos_pix, size_pix, fill_color, 0.0f, sf::Color::Transparent);
}

void Renderer2D::smt_Render_Rect(sf::RenderTarget& target, const sf::Vector2f& pos_pix, const sf::Vector2f& size_pix, const sf::Color& fill_color, float outline_thikness, const sf::Color& outline_color)
{
    sf::RectangleShape l_Rect;

    l_Rect.setPosition(pos_pix);
    l_Rect.setSize(size_pix);
    l_Rect.setOrigin(size_pix / 2.0f);
    l_Rect.setFillColor(fill_color);
    l_Rect.setOutlineThickness(outline_thikness);
    l_Rect.setOutlineColor(outline_color);

    target.draw(l_Rect, sf::RenderStates::Default);
}

void Renderer2D::smt_Render_Line(sf::RenderTarget& target, const sf::Vector2f& start, const sf::Vector2f& end, const sf::Color& color)
{
    sf::Vertex l_Vertices[2];

    l_Vertices[0].color = color;
    l_Vertices[0].position = start;
    l_Vertices[1].color = color;
    l_Vertices[1].position = end;

    target.draw(l_Vertices, 2, sf::Lines);
}

void Renderer2D::smt_Render_Text(sf::RenderTarget& target, const char* text, const sf::Vector2f& start_pos)
{
    sf::Text l_Text;

    l_Text.setPosition(start_pos);
    l_Text.setFont(sm_Renderer.m_Font);
    l_Text.setString(text);

    target.draw(l_Text);
}

void Renderer2D::smt_Render_Text(sf::RenderTarget& target, const char* text, const sf::Vector2f& start_pos, unsigned int char_size, const sf::Color& color)
{
    sf::Text l_Text;

    l_Text.setCharacterSize(char_size);
    l_Text.setPosition(start_pos);
    l_Text.setColor(color);
    l_Text.setFont(sm_Renderer.m_Font);
    l_Text.setString(text);

    target.draw(l_Text);
}


void Renderer2D::smt_Render_Texture(sf::RenderTarget& target,
                                    const sf::Texture& texture,
                                    TextureMode mode)
{
    sf::Vector2f l_Size_Factor;
    sf::Vector2f l_Pos_Pix;

    if (mode == TextureMode::Fullscreen)
    {
        l_Size_Factor.x = target.getView().getSize().x / texture.getSize().x;
        l_Size_Factor.y = target.getView().getSize().y / texture.getSize().y;
    }
    else if (mode == TextureMode::EnsureVisible)
    {
        l_Size_Factor;
    }
    else if (mode == TextureMode::Centered)
    {
        //l_Pos_Pix =
    }
    else
    {
        l_Size_Factor = sf::Vector2f(1.0f, 1.0f);
    }
    smt_Render_Texture(target, texture, sf::Vector2f(0.0f, 0.0f), l_Size_Factor);
}

void Renderer2D::smt_Render_Texture(sf::RenderTarget& target,
                                    const sf::Texture& texture,
                                    const sf::Vector2f& pos_pix,
                                    const sf::Vector2f& size_factor)
{
    sf::Sprite l_Sprite;

    l_Sprite.setTexture(texture);
    l_Sprite.setPosition(pos_pix);
    l_Sprite.setScale(size_factor);

    target.draw(l_Sprite);
}
