#ifndef _ANT_COLONY_HPP
#define _ANT_COLONY_HPP 1

#include "_Prototype.hpp"

#include "Math/Rand.hpp"

#include <cmath>

#include "windows.h"
#include "vfw.h"


namespace ant
{

struct Agent
{
    sf::Vector2f m_Pos;
    float m_Angle_360;
    float m_Speed;
    float m_Power;
};

float rand(float min, float max)
{
    return RandomGenerator::smt_Get().mt_Generate(min, max);
}

float fn_Angle_360_To_Radian(float angle_360)
{
    return angle_360 * 3.14f / 180.0f;
}

sf::Vector2f fn_Angle_To_Direction(float angle_360)
{
    sf::Vector2f l_Ret(0.0f, 0.0f);

    l_Ret.x = std::sin(fn_Angle_360_To_Radian(angle_360));
    l_Ret.y = std::cos(fn_Angle_360_To_Radian(angle_360));

    return l_Ret;
}

sf::Color fn_Color_Gradient(sf::Color c, float gradient)
{
    c.r = std::min(std::max(0.0f, c.r * gradient), 255.0f);
    c.g = std::min(std::max(0.0f, c.g * gradient), 255.0f);
    c.b = std::min(std::max(0.0f, c.b * gradient), 255.0f);

    return c;
}

template<std::size_t ImageWidth, std::size_t ImageHeight, std::size_t AgentCount>
class AntColony : public Prototype
{
public:
    AntColony() :Prototype("Ant Colony") {}
    ~AntColony(){}

    bool mt_Specialized_Init() override
    {
        for (std::size_t ii = 0; ii < m_Agents.size(); ii++)
        {
            m_Agents[ii].m_Pos.x = rand(0.0f, static_cast<float>(ImageWidth));
            m_Agents[ii].m_Pos.y = rand(0.0f, static_cast<float>(ImageHeight));
            m_Agents[ii].m_Angle_360 = rand(0.0f, 360.0f);
            m_Agents[ii].m_Speed = 10.0f;
            m_Agents[ii].m_Power = 1.0f;
        }

        m_Src_Buffer = 0;
        m_Tgt_Buffer = 1;

        for (std::size_t yy = 0; yy < ImageHeight; yy++)
        {
            for (std::size_t xx = 0; xx < ImageWidth; xx++)
            {
                m_Buffers[m_Src_Buffer][yy][xx] = 0.0f;
            }
        }
    }

    void mt_Handle_Event(const sf::Event& event) override
    {
        //
    }
    void mt_Update(float elapsed_time) override
    {
        mt_Updage_Agents_Angle(m_Src_Buffer);
        mt_Move_Agents(elapsed_time);
        mt_Apply_Agents_To_Buffer(m_Src_Buffer, m_Tgt_Buffer);
        mt_Blur(m_Tgt_Buffer, m_Src_Buffer);
        mt_Decay(elapsed_time, m_Src_Buffer);
    }
    void mt_Draw(sf::RenderTarget& target) override
    {
        constexpr float l_Square_Size = 3.0f;
        sf::Vector2f l_Shape_Size(l_Square_Size, l_Square_Size);
        sf::RectangleShape l_Shape;
        sf::Vector2f l_Cell;

        l_Shape.setSize(l_Shape_Size);
        l_Shape.setOutlineThickness(0.0f);
        l_Shape.setOutlineColor(sf::Color::Red);

        for (l_Cell.y = 0.0f; l_Cell.y < ImageHeight; l_Cell.y += 1.0f)
        {
            for (l_Cell.x = 0.0f; l_Cell.x < ImageWidth; l_Cell.x += 1.0f)
            {
                l_Shape.setFillColor(fn_Color_Gradient(sf::Color::White, mt_Get_Value(m_Src_Buffer, l_Cell)));
                l_Shape.setPosition(sf::Vector2f(l_Shape_Size.x * l_Cell.x, l_Shape_Size.y * l_Cell.y));

                target.draw(l_Shape);
            }
        }

        l_Shape.setOutlineThickness(0.0f);
        l_Shape.setOutlineColor(sf::Color::Blue);
        l_Shape.setFillColor(sf::Color::Red);
        for (std::size_t ii = 0; ii < m_Agents.size(); ii++)
        {
            l_Shape.setPosition(m_Agents[ii].m_Pos * l_Square_Size);

            target.draw(l_Shape);
        }
    }

private:

    float mt_Get_Value(std::size_t buffer_index, const sf::Vector2f& pos)
    {
        float l_Ret(0.0f);

        if (    (pos.x >= 0.0f)
            &&  (pos.x < static_cast<float>(ImageWidth))
            &&  (pos.y >= 0.0f)
            &&  (pos.y < static_cast<float>(ImageHeight)))
        {
            l_Ret = m_Buffers[buffer_index][static_cast<std::size_t>(pos.y)][static_cast<std::size_t>(pos.x)];
        }

        return l_Ret;
    }

    void mt_Updage_Agents_Angle(std::size_t buffer_index)
    {
        float l_Front;
        float l_Left;
        float l_Right;

        float l_Antena_Distance = 1.5f;
        float l_Antena_Angle_360 = 20.0f;

        for (std::size_t ii = 0; ii < m_Agents.size(); ii++)
        {
            l_Front = mt_Get_Value(buffer_index, m_Agents[ii].m_Pos + fn_Angle_To_Direction(m_Agents[ii].m_Angle_360) * l_Antena_Distance);
            l_Left = mt_Get_Value(buffer_index, m_Agents[ii].m_Pos + fn_Angle_To_Direction(m_Agents[ii].m_Angle_360 - l_Antena_Angle_360) * l_Antena_Distance);
            l_Right = mt_Get_Value(buffer_index, m_Agents[ii].m_Pos + fn_Angle_To_Direction(m_Agents[ii].m_Angle_360 + l_Antena_Angle_360) * l_Antena_Distance);

            if ((l_Front > l_Left) && (l_Front > l_Right))
            {
                /// Nothing
            }
            else if (l_Left > l_Right)
            {
                m_Agents[ii].m_Angle_360 -= l_Antena_Angle_360;
            }
            else if (l_Right > l_Left)
            {
                m_Agents[ii].m_Angle_360 += l_Antena_Angle_360;
            }
            else
            {
                m_Agents[ii].m_Angle_360 += rand(-l_Antena_Angle_360, l_Antena_Angle_360);
            }

            if (ii == 0)
            {
                //std::cout << ii << ": " << m_Agents[ii].m_Angle_360 << '\n';
            }
        }
    }

    void mt_Move_Agents(float elapsed_time)
    {
        for (std::size_t ii = 0; ii < m_Agents.size(); ii++)
        {
            sf::Vector2f l_Next_Pos;

            l_Next_Pos = m_Agents[ii].m_Pos + fn_Angle_To_Direction(m_Agents[ii].m_Angle_360) * m_Agents[ii].m_Speed * elapsed_time;

            if ((l_Next_Pos.x < 0.0f) || (l_Next_Pos.x >= ImageWidth))
            {
                m_Agents[ii].m_Angle_360 = rand(0.0f, 360.0f);

                if (l_Next_Pos.x < 0.0f)
                {
                    l_Next_Pos.x = 0.0f;
                }
                else
                {
                    l_Next_Pos.x = static_cast<float>(ImageWidth - 1);
                }
            }

            if ((l_Next_Pos.y < 0.0f) || (l_Next_Pos.y >= ImageHeight))
            {
                m_Agents[ii].m_Angle_360 = rand(0.0f, 360.0f);

                if (l_Next_Pos.y < 0.0f)
                {
                    l_Next_Pos.y = 0.0f;
                }
                else
                {
                    l_Next_Pos.y = static_cast<float>(ImageHeight - 1);
                }
            }

            m_Agents[ii].m_Pos = l_Next_Pos;

            if (ii == 0)
            {
                //std::cout << ii << ": [" << l_Next_Pos.x << " : " << l_Next_Pos.y << "]\n";
            }
        }
    }

    void mt_Apply_Agents_To_Buffer(std::size_t src_index, std::size_t tgt_index)
    {
        sf::Vector2i l_Pos;
        for (std::size_t ii = 0; ii < m_Agents.size(); ii++)
        {
            l_Pos.x = m_Agents[ii].m_Pos.x;
            l_Pos.y = m_Agents[ii].m_Pos.y;

            float l_Dbg = m_Buffers[src_index][l_Pos.y][l_Pos.x];

            m_Buffers[tgt_index][l_Pos.y][l_Pos.x] = m_Buffers[src_index][l_Pos.y][l_Pos.x] + m_Agents[ii].m_Power;

            l_Dbg = m_Buffers[tgt_index][l_Pos.y][l_Pos.x];
            l_Dbg = 0.0f;
        }
    }

    void mt_Blur(std::size_t src_index, std::size_t tgt_buffer)
    {
        float l_Sum;

        for (std::size_t yy = 1; yy < (ImageHeight - 1); yy++)
        {
            for (std::size_t xx = 1; xx < (ImageWidth - 1); xx++)
            {
                l_Sum = 0.0f;

                l_Sum += m_Buffers[src_index][yy - 1][xx - 1];
                l_Sum += m_Buffers[src_index][yy - 1][xx + 0];
                l_Sum += m_Buffers[src_index][yy - 1][xx + 1];
                l_Sum += m_Buffers[src_index][yy - 0][xx - 1];
                l_Sum += m_Buffers[src_index][yy - 0][xx + 0];
                l_Sum += m_Buffers[src_index][yy - 0][xx + 1];
                l_Sum += m_Buffers[src_index][yy + 1][xx - 1];
                l_Sum += m_Buffers[src_index][yy + 1][xx + 0];
                l_Sum += m_Buffers[src_index][yy + 1][xx + 1];

                l_Sum /= 9.0f;

                m_Buffers[tgt_buffer][yy][xx] = l_Sum;
            }
        }
    }

    void mt_Decay(float elapsed_time, std::size_t buffer_index)
    {
        float l_Decay_Speed = 10.0f;
        for (std::size_t yy = 0; yy < ImageHeight; yy++)
        {
            for (std::size_t xx = 0; xx < ImageWidth; xx++)
            {
                m_Buffers[buffer_index][yy][xx] -= l_Decay_Speed * elapsed_time;

                m_Buffers[buffer_index][yy][xx] = std::max(0.0f, m_Buffers[buffer_index][yy][xx]);
            }
        }
    }

    std::array<Agent, AgentCount> m_Agents;
    std::array<std::array<std::array<float, ImageWidth>, ImageHeight>, 2> m_Buffers;
    std::size_t m_Src_Buffer;
    std::size_t m_Tgt_Buffer;
};

}





#endif // _ANT_COLONY_HPP
