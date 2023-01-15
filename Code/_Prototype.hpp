#ifndef _PROTOTYPE_HPP
#define _PROTOTYPE_HPP 1

#include "Utilities/Log.hpp"

#include <SFML/Graphics.hpp>
#include "Rendering/SFML_Specific/SFML_2D_Renderer.hpp"

class Prototype
{
public:
    Prototype(const char* title);
    Prototype(const char* title, unsigned int wnd_width, unsigned int wnd_height);
    virtual ~Prototype();

    bool mt_Init();

    void mt_Run(void);

    sf::Vector2u mt_Get_Windows_Size(void) const
    {
        return m_Wnd.getSize();
    }

protected:

    virtual bool mt_Specialized_Init();

    virtual void mt_Handle_Event(const sf::Event& e);
    virtual void mt_Update(float elapsed_time);
    virtual void mt_Draw(sf::RenderTarget& target);

private:

    sf::RenderWindow m_Wnd;
    const char* m_Title;
    sf::VideoMode m_Mode;
    bool m_Maximized;

protected:
    float m_Accumulated_Time;
};


#endif // _PROTOTYPE_HPP
