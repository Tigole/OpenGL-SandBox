#ifndef _SHADER_HPP
#define _SHADER_HPP 1

#include <GL/glew.h>
#include <string>

#include <SFML/Graphics.hpp>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

class Shader
{
public:
    void mt_Use(void);
    bool mt_Create(const std::string& vertex_shader_file, const std::string& fragment_shader_file);

    void mt_Set_Uniform(const std::string& uniform_name, bool uniform_value);
    void mt_Set_Uniform(const std::string& uniform_name, int uniform_value);
    void mt_Set_Uniform(const std::string& uniform_name, unsigned int uniform_value);
    void mt_Set_Uniform(const std::string& uniform_name, float uniform_value);
    void mt_Set_Uniform(const std::string& uniform_name, const sf::Color& uniform_value);
    void mt_Set_Uniform(const std::string& uniform_name, const glm::vec2& uniform_value);
    void mt_Set_Uniform(const std::string& uniform_name, const glm::vec3& uniform_value);
    void mt_Set_Uniform(const std::string& uniform_name, const glm::vec4& uniform_value);
    void mt_Set_Uniform(const std::string& uniform_name, const glm::mat2& uniform_value);
    void mt_Set_Uniform(const std::string& uniform_name, const glm::mat3& uniform_value);
    void mt_Set_Uniform(const std::string& uniform_name, const glm::mat4& uniform_value);

private:

    bool mt_Compile_Shader(const std::string& shader_file, GLenum shader_type, unsigned int& shader_id);
    bool mt_Check_Error(unsigned int id, const std::string& type);

    unsigned int m_Id;
};


#endif // _SHADER_HPP
