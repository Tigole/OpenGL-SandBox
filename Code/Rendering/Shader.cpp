#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

void Shader::mt_Use(void)
{
    glUseProgram(m_Id);
}

bool Shader::mt_Create(const std::string& vertex_shader_file, const std::string& fragment_shader_file)
{
    bool l_b_Ret;
    unsigned int l_vertex_shader;
    unsigned int l_fragment_shader;

    mt_Compile_Shader(vertex_shader_file, GL_VERTEX_SHADER, l_vertex_shader);
    mt_Compile_Shader(fragment_shader_file, GL_FRAGMENT_SHADER, l_fragment_shader);

    m_Id = glCreateProgram();
    glAttachShader(m_Id, l_vertex_shader);
    glAttachShader(m_Id, l_fragment_shader);
    glLinkProgram(m_Id);

    l_b_Ret = mt_Check_Error(m_Id, "PROGRAM");

    glDeleteShader(l_vertex_shader);
    glDeleteShader(l_fragment_shader);

    return l_b_Ret;
}

void Shader::mt_Set_Uniform(const std::string& uniform_name, bool uniform_value)
{
    glUniform1i(glGetUniformLocation(m_Id, uniform_name.c_str()), uniform_value);
}

void Shader::mt_Set_Uniform(const std::string& uniform_name, int uniform_value)
{
    glUniform1i(glGetUniformLocation(m_Id, uniform_name.c_str()), uniform_value);
}

void Shader::mt_Set_Uniform(const std::string& uniform_name, unsigned int uniform_value)
{
    glUniform1ui(glGetUniformLocation(m_Id, uniform_name.c_str()), uniform_value);
}

void Shader::mt_Set_Uniform(const std::string& uniform_name, float uniform_value)
{
    glUniform1f(glGetUniformLocation(m_Id, uniform_name.c_str()), uniform_value);
}

void Shader::mt_Set_Uniform(const std::string& uniform_name, const sf::Color& uniform_value)
{
    glm::vec3 l_vec;
    l_vec.r = uniform_value.r / 255.0f;
    l_vec.g = uniform_value.g / 255.0f;
    l_vec.b = uniform_value.b / 255.0f;
    mt_Set_Uniform(uniform_name, l_vec);
}

void Shader::mt_Set_Uniform(const std::string& uniform_name, const glm::vec2& uniform_value)
{
    glUniform2fv(glGetUniformLocation(m_Id, uniform_name.c_str()), 1, &uniform_value[0]);
}

void Shader::mt_Set_Uniform(const std::string& uniform_name, const glm::vec3& uniform_value)
{
    glUniform3fv(glGetUniformLocation(m_Id, uniform_name.c_str()), 1, &uniform_value[0]);
}

void Shader::mt_Set_Uniform(const std::string& uniform_name, const glm::vec4& uniform_value)
{
    glUniform4fv(glGetUniformLocation(m_Id, uniform_name.c_str()), 1, &uniform_value[0]);
}

void Shader::mt_Set_Uniform(const std::string& uniform_name, const glm::mat2& uniform_value)
{
    glUniformMatrix2fv(glGetUniformLocation(m_Id, uniform_name.c_str()), 1, GL_FALSE, &uniform_value[0][0]);
}

void Shader::mt_Set_Uniform(const std::string& uniform_name, const glm::mat3& uniform_value)
{
    glUniformMatrix3fv(glGetUniformLocation(m_Id, uniform_name.c_str()), 1, GL_FALSE, &uniform_value[0][0]);
}

void Shader::mt_Set_Uniform(const std::string& uniform_name, const glm::mat4& uniform_value)
{
    glUniformMatrix4fv(glGetUniformLocation(m_Id, uniform_name.c_str()), 1, GL_FALSE, &uniform_value[0][0]);
}

bool Shader::mt_Compile_Shader(const std::string& shader_file, GLenum shader_type, unsigned int& shader_id)
{
    bool l_b_Ret(false);
    std::ifstream l_input_file;
    std::string l_shader_code;

    l_input_file.open(shader_file);

    if (l_input_file.is_open())
    {
        std::stringstream l_buffer;

        l_buffer << l_input_file.rdbuf();
        l_shader_code = l_buffer.str();

        l_input_file.close();

        l_b_Ret = true;
    }

    if (l_b_Ret == true)
    {
        const char* l_shader_code_c_str = l_shader_code.c_str();

        shader_id = glCreateShader(shader_type);
        glShaderSource(shader_id, 1, &l_shader_code_c_str, nullptr);
        glCompileShader(shader_id);
    }

    if (l_b_Ret)
    {
        l_b_Ret = mt_Check_Error(shader_id, "SHADER");
    }

    return l_b_Ret;
}

bool Shader::mt_Check_Error(unsigned int id, const std::string& type)
{
    bool l_b_Ret;
    GLchar l_buffer[512];
    int l_success;

    l_b_Ret = true;
    if (type == "PROGRAM")
    {
        glGetProgramiv(id, GL_LINK_STATUS, &l_success);
        if (l_success == 0)
        {
            glGetProgramInfoLog(id, sizeof(l_buffer), nullptr, l_buffer);
            std::cerr << "Linking failed: " << l_buffer << '\n';
        }
    }
    else
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &l_success);
        if (l_success == 0)
        {
            glGetShaderInfoLog(id, sizeof(l_buffer), nullptr, l_buffer);
            std::cerr << "Compilation failed: " << l_buffer << '\n';
            l_b_Ret = false;
        }
    }

    return l_b_Ret;
}
