#ifndef _TRANSFORM_HPP
#define _TRANSFORM_HPP 1

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <utility>
#include <ostream>

#include <cmath>

#define M_PI 3.14f
#define M_PI_2 2*M_PI

std::ostream& operator<<(std::ostream& o, const glm::vec3& v);
std::ostream& operator<<(std::ostream& o, const glm::vec2& v);
inline std::ostream& operator<<(std::ostream& o, const glm::mat4& m)
{
    o << m[0].x << ' ' << m[1].x << ' ' << m[2].x << ' ' << m[3].x << '\n';
    o << m[0].y << ' ' << m[1].y << ' ' << m[2].y << ' ' << m[3].y << '\n';
    o << m[0].z << ' ' << m[1].z << ' ' << m[2].z << ' ' << m[3].z << '\n';
    o << m[0].w << ' ' << m[1].w << ' ' << m[2].w << ' ' << m[3].w << '\n';
    return o;
}

inline float fn_Deg_To_Rad(float angle_deg)
{
    return M_PI * angle_deg / 180.0f;
}

inline glm::vec3 fn_Deg_To_Rad(glm::vec3 v)
{
    v.x = fn_Deg_To_Rad(v.x);
    v.y = fn_Deg_To_Rad(v.y);
    v.z = fn_Deg_To_Rad(v.z);
    return v;
}


class Transform
{
public:

    Transform(const glm::vec3& pos = {0.0f, 0.0f, 0.0f}, const glm::vec3& rot = {0.0f, 0.0f, 0.0f}, const glm::vec3& scale = {1.0f, 1.0f, 1.0f})
     :  m_origin(),
        m_position(),
        m_rotation(),
        m_scale(),
        m_model_matrix()
    {
        mt_Reset(pos, rot, scale);
    }

    void mt_Reset(const glm::vec3& pos = {0.0f, 0.0f, 0.0f}, const glm::vec3& rot = {0.0f, 0.0f, 0.0f}, const glm::vec3& scale = {1.0f, 1.0f, 1.0f})
    {
        m_origin = glm::vec3(0.0f);
        m_position = pos;
        m_rotation = rot;
        m_scale = scale;
        m_model_matrix.first = true;
    }

    void mt_Reset(const Transform& rhs)
    {
        m_origin = rhs.m_origin;
        m_position = rhs.m_position;
        m_rotation = rhs.m_rotation;
        m_scale = rhs.m_scale;
        m_model_matrix.first = true;
    }

    Transform& mt_Set_Position(const glm::vec3& position)
    {
        m_position = position;
        m_model_matrix.first = true;
        return *this;
    }
    Transform& mt_Set_Rotation(const glm::vec3& rotation_rad)
    {
        m_rotation = rotation_rad;
        m_model_matrix.first = true;
        return *this;
    }
    Transform& mt_Set_Scale(const glm::vec3& scale)
    {
        m_scale = scale;
        m_model_matrix.first = true;
        return *this;
    }
    Transform& mt_Set_Origin(const glm::vec3& origin)
    {
        m_origin = origin;
        m_model_matrix.first = true;
        return *this;
    }

    glm::vec3 mt_Get_Rotation(void) const
    {
        return m_rotation;
    }
    glm::vec3 mt_Get_Position(void) const
    {
        return m_position;
    }
    glm::vec3 mt_Get_Scale(void) const
    {
        return m_scale;
    }

    Transform& mt_Translate(const glm::vec3& translation)
    {
        mt_Set_Position(mt_Get_Position() + translation);
        return *this;
    }
    Transform& mt_Rotate(const glm::vec3& rotation_rad)
    {
        mt_Set_Rotation(mt_Get_Rotation() + rotation_rad);
        return *this;
    }
    Transform& mt_Scale(const glm::vec3& scale)
    {
        mt_Set_Scale(mt_Get_Scale() + scale);
        return *this;
    }

    glm::mat4 mt_Get_ModelMatrix(void) const
    {
        if (m_model_matrix.first == true)
        {
            glm::mat4 l_pos = glm::translate(m_position + m_origin);
            glm::mat4 l_scale = glm::scale(m_scale);
            glm::mat4 l_rot_x = glm::rotate(m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 l_rot_y = glm::rotate(m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 l_rot_z = glm::rotate(m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

            m_model_matrix.second = (l_pos * (l_rot_z * l_rot_y * l_rot_x) * l_scale);

            m_model_matrix.first = false;
        }

        return m_model_matrix.second;
    }

    glm::vec4 mt_Transform(const glm::vec3& v) const
    {
        return mt_Get_ModelMatrix() * glm::vec4(v, 1.0f);
    }

private:
    glm::vec3 m_origin;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    mutable std::pair<bool, glm::mat4> m_model_matrix;
};


#endif // _TRANSFORM_HPP
