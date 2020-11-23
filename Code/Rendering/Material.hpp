#ifndef _MATERIAL_HPP
#define _MATERIAL_HPP 1

#include <map>
#include <string>

#include <glm/glm.hpp>

namespace sf
{
class Texture;
}

class Material
{
public:
    Shader* m_Shader;

    std::vector<sf::Texture*> m_Textures;
    std::vector<std::string> m_Textures_Names;

    std::map<std::string, glm::vec2> m_Vec2;
    std::map<std::string, glm::vec3> m_Vec3;
    std::map<std::string, glm::vec4> m_Vec4;

    std::map<std::string, glm::mat2> m_Mat2;
    std::map<std::string, glm::mat3> m_Mat3;
    std::map<std::string, glm::mat4> m_Mat4;
};

#endif // _MATERIAL_HPP
