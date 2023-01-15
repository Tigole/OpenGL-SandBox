#include "CatmullSpline.hpp"
#include "Rendering/SFML_Specific/SFML_2D_Renderer.hpp"

struct Spline2
{
    std::vector<sf::Vector2f> m_Points;

    sf::Vector2f mt_Get_Spline_Point(float t, bool looped)
    {
        int p0, p1, p2, p3;
        float tt;
        float ttt;
        float q0, q1, q2, q3;
        sf::Vector2f l_Ret(0.0f, 0.0f);

        if (looped == false)
        {
            p1 = static_cast<int>(t) + 1;
            p2 = p1 + 1;
            p3 = p2 + 1;
            p0 = p1 - 1;
        }
        else
        {
            p1 = static_cast<int>(t);
            p2 = (p1 + 1) % m_Points.size();
            p3 = (p2 + 1) % m_Points.size();
            p0 = p1 >= 1 ? p1 - 1 : m_Points.size() - 1;
        }

        t = t - static_cast<int>(t);
        tt = t * t;
        ttt = tt * t;

        q0 = -ttt + 2.0f * tt - t;
        q1 = 3.0f * ttt - 5.0f * tt + 2.0f;
        q2 = -3.0f * ttt + 4.0f * tt + t;
        q3 = ttt - tt;

        l_Ret = 0.5f * (m_Points[p0] * q0 + m_Points[p1] * q1 + m_Points[p2] * q2 + m_Points[p3] * q3);

        return l_Ret;
    }

    sf::Vector2f mt_Get_Spline_Gradient(float t, bool looped)
    {
        int p0, p1, p2, p3;
        float tt;
        float ttt;
        float q0, q1, q2, q3;
        sf::Vector2f l_Ret(0.0f, 0.0f);

        if (looped == false)
        {
            p1 = static_cast<int>(t) + 1;
            p2 = p1 + 1;
            p3 = p2 + 1;
            p0 = p1 - 1;
        }
        else
        {
            p1 = static_cast<int>(t);
            p2 = (p1 + 1) % m_Points.size();
            p3 = (p2 + 1) % m_Points.size();
            p0 = p1 >= 1 ? p1 - 1 : m_Points.size() - 1;
        }

        t = t - static_cast<int>(t);
        tt = t * t;
        ttt = tt * t;

        q0 = -3.0f * tt + 4.0f * t - 1.0f;
        q1 = 9.0f * tt - 10.0f * t;
        q2 = -9.0f * tt + 8.0f * t + 1.0f;
        q3 = 3.0f * tt - 2.0f * t;

        l_Ret = 0.5f * (m_Points[p0] * q0 + m_Points[p1] * q1 + m_Points[p2] * q2 + m_Points[p3] * q3);

        return l_Ret;
    }
};






void fn_CatmullSpline(sf::RenderWindow& l_Wnd,
                       sf::Event& l_Event,
                       Camera& l_Camera,
                       float l_Elapsed_Time,
                       float l_Accumulated_Time,
                       sf::Clock& l_Clk,
                       sf::Context& context)
{
    Spline2 l_Spline;
    int l_Selected_Point = 0;
    sf::Vector2f l_Velocity(0.0f, 0.0f);
    float l_Speed = 50.0f;
    bool l_Looped = false;
    float l_t_End;


    for (int ii = 0; ii < 10; ii++)
    {
        l_Spline.m_Points.push_back(sf::Vector2(30.0f + 40.0f * ii, 40.0f));
    }

    l_Looped = false;
    l_t_End = l_Spline.m_Points.size() - 3.0f;


    l_Wnd.create(sf::VideoMode(800, 600), "Catmull-ROM Spline");

    while(l_Wnd.isOpen())
    {
        l_Elapsed_Time = l_Clk.restart().asSeconds();

        while(l_Wnd.pollEvent(l_Event))
        {
            if (l_Event.type == sf::Event::Closed)
            {
                l_Wnd.close();
            }
            else if (l_Event.type == sf::Event::KeyReleased)
            {
                if (l_Event.key.code == sf::Keyboard::Subtract)
                {
                    l_Selected_Point--;
                    if (l_Selected_Point < 0)
                    {
                        l_Selected_Point = l_Spline.m_Points.size() - 1;
                    }
                }
                else if (l_Event.key.code == sf::Keyboard::Add)
                {
                    l_Selected_Point++;
                    if (l_Selected_Point >= l_Spline.m_Points.size())
                    {
                        l_Selected_Point = 0;
                    }
                }
                else if (l_Event.key.code == sf::Keyboard::Up || l_Event.key.code == sf::Keyboard::Down)
                {
                    l_Velocity.y = 0.0f;
                }
                else if (l_Event.key.code == sf::Keyboard::Left || l_Event.key.code == sf::Keyboard::Right)
                {
                    l_Velocity.x = 0.0f;
                }
                else if (l_Event.key.code == sf::Keyboard::L)
                {
                    l_Looped = !l_Looped;

                    if (l_Looped)
                    {
                        l_t_End = l_Spline.m_Points.size();
                    }
                    else
                    {
                        l_t_End = l_Spline.m_Points.size() - 3.0f;
                    }
                }
            }
            else if (l_Event.type == sf::Event::KeyPressed)
            {
                if (l_Event.key.code == sf::Keyboard::Up)
                {
                    l_Velocity.y = -1.0f;
                }
                else if (l_Event.key.code == sf::Keyboard::Down)
                {
                    l_Velocity.y = 1.0f;
                }
                else if (l_Event.key.code == sf::Keyboard::Left)
                {
                    l_Velocity.x = -1.0f;
                }
                else if (l_Event.key.code == sf::Keyboard::Right)
                {
                    l_Velocity.x = 1.0f;
                }
            }
        }

        l_Spline.m_Points[l_Selected_Point] += l_Velocity * l_Elapsed_Time * l_Speed;


        l_Wnd.clear();

        for (std::size_t ii = 0; ii < l_Spline.m_Points.size(); ii++)
        {
            Renderer2D::smt_Render_Rect(l_Wnd, l_Spline.m_Points[ii], sf::Vector2f(10, 10), sf::Color::Transparent, 1.0f, sf::Color::Red);
        }
        Renderer2D::smt_Render_Rect(l_Wnd, l_Spline.m_Points[l_Selected_Point], sf::Vector2f(10, 10), sf::Color::Transparent, 1.0f, sf::Color::Yellow);

        for (float t = 0.0f; t < l_t_End; t += 0.005f)
        {
            Renderer2D::smt_Render_Rect(l_Wnd, l_Spline.mt_Get_Spline_Point(t, l_Looped), sf::Vector2f(2, 2), sf::Color::White, 0.0f, sf::Color::Transparent);
        }


        Renderer2D::smt_Render_Line(l_Wnd, sf::Vector2f(50.0f, 50.0f), sf::Vector2f(75.0f, 60.0f), sf::Color::Green);


        l_Wnd.display();
    }
}
