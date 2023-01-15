#ifndef _SKELETON_HPP
#define _SKELETON_HPP 1

#include "_Prototype.hpp"

namespace sk
{

struct KeyFrame
{
    sf::Vector2f m_Pos;
    float m_Angle;
};

class KeyFrameManager
{
    const KeyFrame* mt_Get_Next_KeyFrame_Id(void)
    {
        const KeyFrame* l_Ret = nullptr;

        m_Current_Frame_Index++;
        if (m_Current_Frame_Index > m_Frames.size())
        {
            m_Current_Frame_Index = 0;
        }

        l_Ret = &m_Frames[m_Current_Frame_Index];

        return l_Ret;
    }

    std::vector<KeyFrame> m_Frames;
    std::size_t m_Current_Frame_Index;
};

struct Bone
{
    int m_Parent_Id;
    int m_Child_Id;
};

struct Entity
{
    int m_Id;
    int m_Parent_Id;

    sf::RectangleShape m_Sprite;
};

}


class Skeleton : public Prototype
{
public:
    Skeleton();

protected:
    bool mt_Specialized_Init() override;

    void mt_Handle_Event(const sf::Event& e) override;
    void mt_Update(float elapsed_time) override;
    void mt_Draw(sf::RenderTarget& target) override;

    std::map<int, std::unique_ptr<sk::Entity>> m_Entities;
    std::map<std::string, std::unique_ptr<sf::Texture>> m_Textures;
};

#endif // _SKELETON_HPP
