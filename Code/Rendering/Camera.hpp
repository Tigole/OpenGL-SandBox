#ifndef _CAMERA_HPP
#define _CAMERA_HPP 1

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <SFML/Graphics.hpp>

struct CameraData
{
    CameraData() {}
    CameraData(float fov, const sf::Vector2u& window_size, float near = 1.0f, float far = 1000.0f)
     :  m_fov(fov),
        m_aspect(static_cast<float>(window_size.x) / window_size.y),
        m_frustum_near(near),
        m_frustum_far(far)
    {}

    float m_fov;
    float m_aspect;
    float m_frustum_near;
    float m_frustum_far;
};

enum class CameraDirection
{
    None,
    Up,
    Down,
    Left,
    Right,
    Fwd,
    Back,
};

struct CameraControl
{
    sf::Vector2i m_Old_Mouse_Pos;
    float m_Pitch_Magnitude;
    float m_Yaw_Magnitude;
    float m_Mouvement_Speed;
    glm::vec3 m_Direction;
};

class Camera
{
public:
    Camera();

    void mt_Set(const glm::vec3& position, const CameraData& data);
    void mt_Set(const glm::vec3& position, float pitch, float yaw, const CameraData& data);
    glm::mat4 mt_Get_ViewProjection_Matrix(void) const;
    glm::mat4 mt_Get_View_Matrix(void) const;
    glm::mat4 mt_Get_Projection_Matrix(void) const;
    glm::vec3 mt_Get_Position(void) const;
    glm::vec3 mt_Get_Fwd() const;

    float mt_Get_Yaw(void) const;
    float mt_Get_Pitch(void) const;

    void mt_MoveBy(CameraDirection direction, float magnitude);
    void mt_MoveBy(const glm::vec3& direction);
    void mt_Rotate(float x_pitch, float y_yaw);

private:

    void mt_Recalculate_Perspective(void);

    void mt_Update_Vectors(void);

    CameraData m_Data;
    glm::vec3 m_Position;
    glm::vec3 m_Fwd_Dir;
    glm::vec3 m_Up_Dir;
    glm::vec3 m_Right_Dir;
    glm::vec3 m_World_Up;
    glm::mat4 m_Perspective_Matrix;
    float m_Pitch;
    float m_Yaw;
};

#endif // _CAMERA_HPP
