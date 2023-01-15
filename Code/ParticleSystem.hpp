#ifndef _PARTICLE_SYSTEM_HPP
#define _PARTICLE_SYSTEM_HPP 1

#include "_Prototype.hpp"
#include "../Submodules/swarm/include/swarm.hpp"


namespace particle_system
{

template <std::size_t MaxSize, std::size_t CirclePointCount>
struct ParticleContainer
{
    ParticleContainer() : m_Swarm(16)
    {}

    void mt_Init(const sf::Vector2u& max_pos)
    {
        m_Max_Pos = max_pos;

        m_VA.setPrimitiveType(sf::PrimitiveType::Triangles);
        m_VA.resize(3 * CirclePointCount * MaxSize);

        log_debug("m_VA.getVertexCount(): %d", m_VA.getVertexCount());

        for (std::size_t ii = 0; ii < MaxSize; ii++)
        {
            m_Radius[ii] = RandomGenerator::smt_Get().mt_Generate(10.0f, 20.0f);
            m_Color[ii].r = RandomGenerator::smt_Get().mt_Generate(0.0f, 255.0f);
            m_Color[ii].g = RandomGenerator::smt_Get().mt_Generate(0.0f, 255.0f);
            m_Color[ii].b = RandomGenerator::smt_Get().mt_Generate(0.0f, 255.0f);
            m_Pos[ii].x = RandomGenerator::smt_Get().mt_Generate(0.0f, m_Max_Pos.x);
            m_Pos[ii].y = RandomGenerator::smt_Get().mt_Generate(0.0f, m_Max_Pos.y);
            m_Remaining_Life[ii] = (MaxSize - ii) / 100.0f + 5.0f;
        }

        m_Alive_Particles = MaxSize;
    }

    void mt_Resize(std::size_t size)
    {
        size = std::min(size, MaxSize);
    }

    void mt_Update(float elapsed_time)
    {
        for (int ii = m_Alive_Particles - 1; ii >= 0; ii--)
        {
            if (m_Remaining_Life[ii] > 0.0f)
            {
                m_Alive_Particles = ii + 1;
                m_VA.resize(m_Alive_Particles * 3 * CirclePointCount);
                break;
            }
        }
        auto l_Group = m_Swarm.execute([&](uint32_t worker_id, uint32_t worker_count){mt_Update_Particle(worker_id, worker_count, elapsed_time);}, (m_Alive_Particles > 16) ? 16 : 1);
        l_Group.waitExecutionDone();
    }

    void mt_Update_Particle(uint32_t worker_id, uint32_t worker_count, float elapsed_time)
    {
        uint32_t l_Step = m_Alive_Particles / worker_count;
        uint32_t l_Start_Index = worker_id * l_Step;
        uint32_t l_End_Index = (worker_id < worker_count - 1) ? l_Start_Index + l_Step : m_Alive_Particles;

        constexpr const float l_Max_Speed = 1.0f;

        l_End_Index = std::min(l_End_Index, (uint32_t)m_Alive_Particles);

        for (uint32_t ii = l_Start_Index; ii < l_End_Index; ii++)
        {
            m_Speed[ii].x += l_Max_Speed * RandomGenerator::smt_Get().mt_Generate(-1.0f, 1.0f);
            m_Speed[ii].y += l_Max_Speed * RandomGenerator::smt_Get().mt_Generate(-1.0f, 1.0f);

            m_Pos[ii] += m_Speed[ii] * elapsed_time;

            if ((m_Pos[ii].x <= 0.0f) || (m_Pos[ii].x >= m_Max_Pos.x))
            {
                m_Speed[ii].x = -m_Speed[ii].x;
            }
            if ((m_Pos[ii].y <= 0.0f) || (m_Pos[ii].y >= m_Max_Pos.y))
            {
                m_Speed[ii].y = -m_Speed[ii].y;
            }

            m_Remaining_Life[ii] -= elapsed_time;

            mt_Update_Particle(ii);
        }
    }
    void mt_Update_Particle(std::size_t particle_id)
    {
        std::size_t l_VA_Id = particle_id * 3 * CirclePointCount;

        for (std::size_t ii = 0; ii < CirclePointCount; ii++)
        {
            float l_Angle1 = (ii + 0) * 360.0f / CirclePointCount;
            float l_Angle2 = (ii + 1) * 360.0f / CirclePointCount;

            l_Angle1 = l_Angle1 * 2.0f * 3.14f / 360.0f;
            l_Angle2 = l_Angle2 * 2.0f * 3.14f / 360.0f;

            m_VA[l_VA_Id + (ii * 3) + 0].position = m_Pos[particle_id];
            m_VA[l_VA_Id + (ii * 3) + 1].position = m_Pos[particle_id] + m_Radius[particle_id] * sf::Vector2f(std::cos(l_Angle1), std::sin(l_Angle1));
            m_VA[l_VA_Id + (ii * 3) + 2].position = m_Pos[particle_id] + m_Radius[particle_id] * sf::Vector2f(std::cos(l_Angle2), std::sin(l_Angle2));

            m_VA[l_VA_Id + (ii * 3) + 0].color = m_Color[particle_id];
            m_VA[l_VA_Id + (ii * 3) + 1].color = m_Color[particle_id];
            m_VA[l_VA_Id + (ii * 3) + 2].color = m_Color[particle_id];
        }
    }

    void mt_Draw(sf::RenderTarget& target)
    {
        sf::RenderStates l_State = sf::RenderStates::Default;

        l_State.blendMode = sf::BlendMode(sf::BlendMode::Factor::One, sf::BlendMode::Factor::One, sf::BlendMode::Equation::Add);

        target.draw(m_VA, l_State);
    }

    void mt_Swap(std::size_t aa, std::size_t bb)
    {
        std::swap(m_Remaining_Life[aa], m_Remaining_Life[bb]);
        std::swap(m_Radius[aa], m_Radius[bb]);
        std::swap(m_Color[aa], m_Color[bb]);
        std::swap(m_Pos[aa], m_Pos[bb]);
        std::swap(m_Speed[aa], m_Speed[bb]);
    }

    std::array<float, MaxSize> m_Remaining_Life;
    std::array<float, MaxSize> m_Radius;
    std::array<sf::Color, MaxSize> m_Color;
    std::array<sf::Vector2f, MaxSize> m_Pos;
    std::array<sf::Vector2f, MaxSize> m_Speed;

    sf::VertexArray m_VA;

    swrm::Swarm m_Swarm;
    sf::Vector2f m_Max_Pos;

    std::size_t m_Alive_Particles;
};

}


class ParticleSystem : public Prototype
{
public:

    ParticleSystem() : Prototype("Particle System")
    {}

    bool mt_Specialized_Init() override
    {
        m_Particles.mt_Init(mt_Get_Windows_Size());
        return true;
    }

    void mt_Update(float elapsed_time) override
    {
        m_FPS = 1.0f / elapsed_time;

        sf::Clock l_Clk;
        m_Particles.mt_Update(elapsed_time);
        m_Particles_Update_Time = l_Clk.getElapsedTime().asMilliseconds();
    }

    void mt_Draw(sf::RenderTarget& target) override
    {
        char l_Buffer[128] = {0};

        sf::Clock l_Clk;
        m_Particles.mt_Draw(target);
        m_Particles_Draw_Time = l_Clk.getElapsedTime().asMilliseconds();

        sprintf(l_Buffer,
                "FPS: %d\nAlive: %d\nUpdate (ms): %f\nDraw (ms): %f",
                static_cast<int>(m_FPS),
                m_Particles.m_Alive_Particles,
                m_Particles_Update_Time,
                m_Particles_Draw_Time);

        Renderer2D::smt_Render_Text(target, l_Buffer, sf::Vector2f(0.0f, 0.0f), 30, sf::Color::Red);
    }

private:

    particle_system::ParticleContainer<10000, 30> m_Particles;
    float m_FPS;
    float m_Particles_Update_Time;
    float m_Particles_Draw_Time;
};



#endif // _PARTICLE_SYSTEM_HPP
