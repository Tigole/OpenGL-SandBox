#ifndef _VERTEX_ARRAY_HPP
#define _VERTEX_ARRAY_HPP 1

#include <cstdint>

#include <vector>
#include <initializer_list>
#include <string>
#include <map>
#include <memory>

#include "glm/glm.hpp"
//#include "ResourceManager/Resource.hpp"
#include "OpenGLTranslation.hpp"

class Mesh;

enum class ShaderDataType
{
    None,
    vec3,
    vec4,
    mat3,
    mat4,
    vec2,
};

std::istream& operator>>(std::istream& i, ShaderDataType& sdt);
std::ostream& operator<<(std::ostream& o, ShaderDataType& sdt);
int fn_ShaderDataType_To_Size(ShaderDataType type);
int fn_ShaderDataType_To_Type(ShaderDataType type);

struct VertexBufferLayoutElement
{
    VertexBufferLayoutElement() : m_Name(), m_Type(ShaderDataType::None), m_Offset(0), m_Byte_Size(0), m_Nomalized(false) {}
    VertexBufferLayoutElement(const std::string& name, ShaderDataType type, bool normalized)
     :  m_Name(name), m_Type(type), m_Offset(0), m_Byte_Size(fn_ShaderDataType_To_Size(type)), m_Nomalized(normalized)
    {}

    int mt_Get_Element_Count(void) const
    {
        switch(m_Type)
        {
            case ShaderDataType::mat3: return 3 * 3;
            case ShaderDataType::mat4: return 4 * 4;
            case ShaderDataType::vec3: return 3;
            case ShaderDataType::vec4: return 4;
            case ShaderDataType::vec2: return 2;
        }

        return 0;
    }

    std::string m_Name;
    ShaderDataType m_Type;
    int m_Offset;
    int m_Byte_Size;
    bool m_Nomalized;
};

class VertexBufferLayout
{
public:
    VertexBufferLayout() : m_Elements() {}
    VertexBufferLayout(std::initializer_list<VertexBufferLayoutElement> elements) : m_Elements(elements) { mt_Setup(); }
    VertexBufferLayout(const std::vector<VertexBufferLayoutElement>& elements) : m_Elements(elements) { mt_Setup(); }

    void mt_Set_Elements(const std::vector<VertexBufferLayoutElement>& elements)
    {
        m_Elements = elements;
        mt_Setup();
    }

    int mt_Get_Stride(void) const
    {
        return m_Stride;
    }

    const std::vector<VertexBufferLayoutElement>& mt_Get_Elements(void) const
    {
        return m_Elements;
    }

    std::vector<VertexBufferLayoutElement>::iterator begin() {return m_Elements.begin();}
    std::vector<VertexBufferLayoutElement>::iterator end() {return m_Elements.end();}

private:
    void mt_Setup(void)
    {
        int l_Offset = 0;

        for (std::size_t ii = 0; ii < m_Elements.size(); ii++)
        {
            m_Elements[ii].m_Offset = l_Offset;
            l_Offset += m_Elements[ii].m_Byte_Size;
        }

        m_Stride = l_Offset;
    }

    int m_Stride;
    std::vector<VertexBufferLayoutElement> m_Elements;
};

class IndexBuffer
{
public:
    IndexBuffer(const std::vector<uint32_t>& indices);
    ~IndexBuffer();
    void mt_Bind(void);
    void mt_Unbind(void);

    uint32_t mt_Get_Indices_Count(void) const {return m_Indices_Count;}

private:
    uint32_t m_Id;
    uint32_t m_Indices_Count;
};

class VertexBuffer
{
public:
    VertexBuffer(const std::vector<float>& vertices, const VertexBufferLayout& layout);  /// Add index buffer construction method
    VertexBuffer(const VertexBufferLayout& layout);
    ~VertexBuffer();

    void mt_Bind(void);
    void mt_Unbind(void);

    const VertexBufferLayout& mt_Get_Layout(void) const
    {
        return m_Layout;
    }

    IndexBuffer* mt_Get_Index_Buffer(void)
    {
        return m_Index_Buffer.get();
    }

    bool mt_Instanced(void) const
    {
        return m_Index_Buffer == nullptr;
    }

    void mt_Update_Data(const std::vector<float>& vertices, DrawingUsage usage);
    bool mt_Update_Data(const std::string& name, const void* data, std::size_t byte_count, DrawingUsage usage);

private:
    uint32_t m_Id;
    VertexBufferLayout m_Layout;
    std::unique_ptr<IndexBuffer> m_Index_Buffer;
};

struct VertexArray_CreationParams
{
    Mesh* m_Mesh;
};

class VertexArray
{
public:
    VertexArray();
    VertexArray(const VertexArray& rhs) = delete;
    VertexArray& operator=(const VertexArray& rhs) = delete;
    ~VertexArray();

    void mt_Add_Vertex_Buffer(VertexBuffer* buffer);
    void mt_Set_Index_Buffer(IndexBuffer* buffer);

    void mt_Bind(void) const;
    void mt_Unbind(void) const;

    bool mt_Update_Data(const std::string& name, const glm::mat4& matrix);
    bool mt_Update_Data(const std::string& name, const void* data, std::size_t byte_count, DrawingUsage usage);
private:
    std::map<std::string, VertexBuffer*> m_String_Data_Map;
    std::vector<VertexBuffer*> m_Vertex_Buffers;
    IndexBuffer* m_Index_Buffer;
    uint32_t m_Id;


    uint32_t m_Attrib_Index;

public:

    uint32_t mt_Indice_Count(void) const
    {
        return m_Index_Buffer->mt_Get_Indices_Count();
    }
};


#endif // _VERTEX_ARRAY_HPP
