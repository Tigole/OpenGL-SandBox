#include "Skeleton.hpp"







Skeleton::Skeleton()
 :  Prototype("2D - Skeleton")
{}

bool Skeleton::mt_Specialized_Init()
{
    std::vector<std::pair<const char*, const char*>> l_Textures =
    {
        std::make_pair("Body", "../../Assets/SkeletonSprites/Golem_1/PNG/Vector Parts/Body.png"),
        std::make_pair("Head", "../../Assets/SkeletonSprites/Golem_1/PNG/Vector Parts/Head.png"),
        std::make_pair("Right Arm", "../../Assets/SkeletonSprites/Golem_1/PNG/Vector Parts/Right Arm.png"),
        std::make_pair("Right Hand", "../../Assets/SkeletonSprites/Golem_1/PNG/Vector Parts/Right Hand.png"),
        std::make_pair("Right Leg", "../../Assets/SkeletonSprites/Golem_1/PNG/Vector Parts/Right Leg.png"),
        std::make_pair("Left Arm", "../../Assets/SkeletonSprites/Golem_1/PNG/Vector Parts/Left Arm.png"),
        std::make_pair("Left Hand", "../../Assets/SkeletonSprites/Golem_1/PNG/Vector Parts/Left Hand.png"),
        std::make_pair("Left Leg", "../../Assets/SkeletonSprites/Golem_1/PNG/Vector Parts/Left Leg.png"),
        std::make_pair("Sword", "../../Assets/SkeletonSprites/Golem_1/PNG/Vector Parts/Sword.png"),
    };
    std::vector<std::pair<int, const char*>> l_Entities =
    {
        std::make_pair(0, "Body"),
        std::make_pair(1, "Head"),
        std::make_pair(2, "Right Arm"),
        std::make_pair(3, "Right Hand"),
        std::make_pair(8, "Right Leg"),
    };
    sf::Texture* l_Texture;

    for (std::size_t ii = 0; ii < l_Textures.size(); ii++)
    {
        l_Texture = new sf::Texture;

        if (l_Texture->loadFromFile(l_Textures[ii].second))
        {
            m_Textures.emplace(l_Textures[ii].first, std::unique_ptr<sf::Texture>(l_Texture));
        }
        else
        {
            log_error("Failed loading texture: '%s'", l_Textures[ii].second);
            delete l_Texture;
        }
    }

    for (std::size_t ii = 0; ii < l_Entities.size(); ii++)
    {
        sk::Entity* l_Entity = new sk::Entity;

        l_Entity->m_Id = l_Entities[ii].first;

        l_Texture = m_Textures[l_Entities[ii].second].get();
        l_Entity->m_Sprite.setTexture(l_Texture, true);
        l_Entity->m_Sprite.setSize(l_Texture->getSize());
        l_Entity->m_Sprite.setOutlineThickness(-1.0f);
        l_Entity->m_Sprite.setOutlineColor(sf::Color::Yellow);

        m_Entities.emplace(l_Entity->m_Id, std::unique_ptr<sk::Entity>(l_Entity));
    }


    return true;
}

void Skeleton::mt_Handle_Event(const sf::Event& e)
{
    //
}

void Skeleton::mt_Update(float elapsed_time)
{
    //
}

void Skeleton::mt_Draw(sf::RenderTarget& target)
{
    for (auto& e : m_Entities)
    {
        target.draw(e.second->m_Sprite);
    }

    sf::CircleShape l_Circle;

    l_Circle.setRadius(10.0f);
    l_Circle.setFillColor(sf::Color::Blue);
    l_Circle.setOrigin(l_Circle.getRadius(), l_Circle.getRadius());

    for (auto& e : m_Entities)
    {
        l_Circle.setPosition(e.second->m_Sprite.getOrigin());

        target.draw(l_Circle, e.second->m_Sprite.getTransform());
    }
}

