#include "Camera.hpp"

Camera::Camera()
 :  m_Data(),
    m_Position(),
    m_Pitch(0.0f),
    m_Yaw(-90.0f)
{}

void Camera::mt_Set(const glm::vec3& position, const CameraData& data)
{
    m_Data = data;
    m_Position = position;

    mt_Recalculate_Perspective();
    m_Fwd_Dir = glm::vec3(0.0f, 0.0f, -1.0f);
    m_Up_Dir = glm::vec3(0.0f, 1.0f, 0.0f);
    m_World_Up = glm::vec3(0.0f, 1.0f, 0.0f);

    mt_Update_Vectors();
}

void Camera::mt_Set(const glm::vec3& position, float pitch, float yaw, const CameraData& data)
{
    m_Yaw = yaw;
    m_Pitch = pitch;

    m_Pitch = std::min(m_Pitch, 89.0f);
    m_Pitch = std::max(m_Pitch, -89.0f);

    mt_Set(position, data);
}

glm::mat4 Camera::mt_Get_ViewProjection_Matrix(void) const
{
    glm::mat4 l_ret;

    l_ret = glm::lookAt(m_Position, m_Position + m_Fwd_Dir, m_Up_Dir);
    l_ret = m_Perspective_Matrix * l_ret;

    return l_ret;
}

glm::vec3 Camera::mt_Get_Position(void) const
{
    return m_Position;
}

glm::vec3 Camera::mt_Get_Fwd() const
{
    return m_Fwd_Dir;
}

float Camera::mt_Get_Yaw(void) const
{
    return m_Yaw;
}

float Camera::mt_Get_Pitch(void) const
{
    return m_Pitch;
}

glm::mat4 Camera::mt_Get_View_Matrix(void) const
{
    return glm::lookAt(m_Position, m_Position + m_Fwd_Dir, m_Up_Dir);
}

glm::mat4 Camera::mt_Get_Projection_Matrix(void) const
{
    return m_Perspective_Matrix;
}

void Camera::mt_MoveBy(CameraDirection direction, float magnitude)
{
    if (direction == CameraDirection::Fwd)
    {
        m_Position += m_Fwd_Dir * magnitude;
    }
    else if (direction == CameraDirection::Back)
    {
        m_Position -= m_Fwd_Dir * magnitude;
    }
    else if (direction == CameraDirection::Up)
    {
        m_Position += m_Up_Dir * magnitude;
    }
    else if (direction == CameraDirection::Down)
    {
        m_Position -= m_Up_Dir * magnitude;
    }
    else if (direction == CameraDirection::Left)
    {
        m_Position -= m_Right_Dir * magnitude;
    }
    else if (direction == CameraDirection::Right)
    {
        m_Position += m_Right_Dir * magnitude;
    }
}

void Camera::mt_MoveBy(const glm::vec3& direction)
{
    m_Position += m_Right_Dir * direction.x;
    m_Position += m_Up_Dir * direction.y;
    m_Position += m_Fwd_Dir * direction.z;
}

void Camera::mt_Rotate(float x_pitch, float y_yaw)
{
    m_Pitch += x_pitch;
    m_Yaw += y_yaw;

    if (1)
    {
        m_Pitch = std::min(m_Pitch, 89.0f);
        m_Pitch = std::max(m_Pitch, -89.0f);
    }

    mt_Update_Vectors();
}

void Camera::mt_Recalculate_Perspective(void)
{
    m_Perspective_Matrix = glm::perspective(m_Data.m_fov, m_Data.m_aspect, m_Data.m_frustum_near, m_Data.m_frustum_far);
}

void Camera::mt_Update_Vectors(void)
{
    glm::vec3 l_Fwd;
    l_Fwd.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    l_Fwd.y = sin(glm::radians(m_Pitch));
    l_Fwd.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

    m_Fwd_Dir = glm::normalize(l_Fwd);
    m_Right_Dir = glm::normalize(glm::cross(m_Fwd_Dir, m_World_Up));
    m_Up_Dir = glm::normalize(glm::cross(m_Right_Dir, m_Fwd_Dir));
}
