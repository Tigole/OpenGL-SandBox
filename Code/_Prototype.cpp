#include "_Prototype.hpp"

#include "windows.h"

Prototype::Prototype(const char* title) : m_Title(title), m_Mode(sf::VideoMode::getDesktopMode()), m_Maximized(true)
{}

Prototype::Prototype(const char* title, unsigned int wnd_width, unsigned int wnd_height) : m_Title(title), m_Mode(wnd_width, wnd_height), m_Maximized(false)
{}

Prototype::~Prototype()
{}

bool Prototype::mt_Init()
{
    m_Wnd.create(m_Mode, m_Title, sf::Style::Default, sf::ContextSettings(24, 0, 8, 4, 6));

    if (m_Maximized == true)
    {
        ShowWindow(m_Wnd.getSystemHandle(), SW_MAXIMIZE);
    }

    m_Accumulated_Time = 0.0f;

    mt_Specialized_Init();
}

void Prototype::mt_Run(void)
{
    sf::Event l_Event;
    sf::Clock l_Clk;
    float l_Elapsed_Time;

    l_Clk.restart();
    while(m_Wnd.isOpen())
    {
        while(m_Wnd.pollEvent(l_Event))
        {
            if (l_Event.type == sf::Event::Closed)
            {
                m_Wnd.close();
            }
            else if (l_Event.type == sf::Event::Resized)
            {
                sf::View l_View = m_Wnd.getView();

                l_View.setSize(l_Event.size.width, l_Event.size.height);
                l_View.setCenter(l_View.getSize() / 2.0f);

                m_Wnd.setView(l_View);
            }

            mt_Handle_Event(l_Event);
        }

        l_Elapsed_Time = l_Clk.restart().asSeconds();

        m_Accumulated_Time += l_Elapsed_Time;

        mt_Update(l_Elapsed_Time);

        if (m_Wnd.isOpen())
        {
            m_Wnd.clear();

            mt_Draw(m_Wnd);

            m_Wnd.display();
        }
    }
}

bool Prototype::mt_Specialized_Init()
{
    return true;
}

void Prototype::mt_Handle_Event(const sf::Event& e)
{
    //
}

void Prototype::mt_Update(float elapsed_time)
{
    //
}

void Prototype::mt_Draw(sf::RenderTarget& target)
{
    //
}
