#include "VertexArray.hpp"
#include "Mesh.hpp"
#include "GL/glew.h"


std::istream& operator>>(std::istream& i, ShaderDataType& sdt)
{
    std::string l_Str;

    i >> l_Str;

    if (l_Str == "vec3")
    {
        sdt = ShaderDataType::vec3;
    }
    else if (l_Str == "vec4")
    {
        sdt = ShaderDataType::vec4;
    }
    else
    {
        sdt = ShaderDataType::None;
    }

    return i;
}

std::ostream& operator<<(std::ostream& o, ShaderDataType& sdt)
{
    switch(sdt)
    {
    case ShaderDataType::vec2: o << "vec2"; break;
    case ShaderDataType::vec3: o << "vec3"; break;
    case ShaderDataType::vec4: o << "vec4"; break;
    }
    return o;
}


int fn_ShaderDataType_To_Size(ShaderDataType type)
{
    switch(type)
    {
        case ShaderDataType::mat3: return 4 * 3 * 3;
        case ShaderDataType::mat4: return 4 * 4 * 4;
        case ShaderDataType::vec3: return 4 * 3;
        case ShaderDataType::vec4: return 4 * 4;
        case ShaderDataType::vec2: return 4 * 2;
    }

    return 0;
}

int fn_ShaderDataType_To_Type(ShaderDataType type)
{
    switch(type)
    {
        case ShaderDataType::mat3: return GL_FLOAT;
        case ShaderDataType::mat4: return GL_FLOAT;
        case ShaderDataType::vec3: return GL_FLOAT;
        case ShaderDataType::vec4: return GL_FLOAT;
        case ShaderDataType::vec2: return GL_FLOAT;
    }

    return GL_FLOAT;
}



VertexBuffer::VertexBuffer(const std::vector<float>& vertices, const VertexBufferLayout& layout)
{
    glGenBuffers(1, &m_Id);

    glBindBuffer(GL_ARRAY_BUFFER, m_Id);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::vector<uint32_t> l_Indices;

    l_Indices.resize(vertices.size());
    for (std::size_t ii = 0; ii < l_Indices.size(); ii++)
    {
        l_Indices[ii] = ii;
    }
    m_Index_Buffer.reset(new IndexBuffer(l_Indices));

    m_Layout = layout;
}

VertexBuffer::VertexBuffer(const VertexBufferLayout& layout)
{
    glGenBuffers(1, &m_Id);

    glBindBuffer(GL_ARRAY_BUFFER, m_Id);

    glBufferData(GL_ARRAY_BUFFER, layout.mt_Get_Stride(), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_Layout = layout;
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_Id);
}

void VertexBuffer::mt_Bind(void)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
}

void VertexBuffer::mt_Unbind(void)
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::mt_Update_Data(const std::vector<float>& vertices, DrawingUsage usage)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), (GLenum)usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool VertexBuffer::mt_Update_Data(const std::string& name, const void* data, std::size_t byte_count, DrawingUsage usage)
{
    bool l_b_Ret(false);
    auto l_it = std::find_if(m_Layout.begin(), m_Layout.end(), [&](const VertexBufferLayoutElement& e){return e.m_Name == name;});

    if (l_it != m_Layout.end())
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_Id);
        glBufferData(GL_ARRAY_BUFFER, byte_count, data, (GLenum)usage);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        l_b_Ret = true;
    }

    return l_b_Ret;
}







IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices)
{
    glGenBuffers(1, &m_Id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_Indices_Count = indices.size();
}
IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_Id);
}

void IndexBuffer::mt_Bind(void)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
}

void IndexBuffer::mt_Unbind(void)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}








VertexArray::VertexArray()
 :  m_Vertex_Buffers(), m_Index_Buffer(), m_Attrib_Index(0)
{
    glGenVertexArrays(1, &m_Id);
    glBindVertexArray(0);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_Id);
}

void VertexArray::mt_Add_Vertex_Buffer(VertexBuffer* buffer)
{
    glBindVertexArray(m_Id);

    const VertexBufferLayout* l_Layout = &buffer->mt_Get_Layout();

    m_Vertex_Buffers.push_back(buffer);

    buffer->mt_Bind();

    for (int index = 0; index < l_Layout->mt_Get_Elements().size(); index++)
    {
        const VertexBufferLayoutElement& l_Element = l_Layout->mt_Get_Elements()[index];

        if (    (l_Element.m_Type == ShaderDataType::mat4)
            ||  (l_Element.m_Type == ShaderDataType::mat3))
        {
            uint8_t l_Sub_Attrib_Count = l_Element.mt_Get_Element_Count() / 4;
            for (int jj = 0; jj < l_Sub_Attrib_Count; jj++)
            {
                glEnableVertexAttribArray(m_Attrib_Index);
                glVertexAttribPointer(m_Attrib_Index,
                                      l_Sub_Attrib_Count,
                                      fn_ShaderDataType_To_Type(l_Element.m_Type),
                                      l_Element.m_Nomalized ? GL_TRUE : GL_FALSE,
                                      l_Layout->mt_Get_Stride(),
                                      (const void*)(sizeof(float) * jj * l_Sub_Attrib_Count));
                glVertexAttribDivisor(m_Attrib_Index, 1);
                m_Attrib_Index++;
            }
        }
        else
        {
            glEnableVertexAttribArray(m_Attrib_Index);
            glVertexAttribPointer(m_Attrib_Index,
                                  l_Element.mt_Get_Element_Count(),
                                  fn_ShaderDataType_To_Type(l_Element.m_Type),
                                  l_Element.m_Nomalized ? GL_TRUE : GL_FALSE,
                                  l_Layout->mt_Get_Stride(),
                                  (const void*)l_Element.m_Offset);
            m_Attrib_Index++;
        }
        m_String_Data_Map[l_Element.m_Name] = m_Vertex_Buffers.back();
    }

    glBindVertexArray(0);
}

void VertexArray::mt_Set_Index_Buffer(IndexBuffer* buffer)
{
    glBindVertexArray(m_Id);

    buffer->mt_Bind();

    m_Index_Buffer = buffer;

    glBindVertexArray(0);
}

#if 0
void VertexArray::mt_Create(const VertexArray_CreationParams& cp)
{
    m_Delete = true;

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_Instanced_VBO);
    glGenBuffers(1, &m_EBO);

    if (cp.m_Mesh != nullptr)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, cp.m_Mesh->m_Vertices.size() * sizeof(cp.m_Mesh->m_Vertices[0]), cp.m_Mesh->m_Vertices.data(), GL_STATIC_DRAW);

        int l_Attrib_Index = 0;
        for (int index = 0; index < cp.m_VB_Layout.mt_Get_Elements().size(); index++)
        {
            const VertexBufferLayoutElement& l_Element = cp.m_VB_Layout.mt_Get_Elements()[index];
            glEnableVertexAttribArray(index);
            if (l_Element.m_Type == ShaderDataType::mat4)
            {
                l_Attrib_Index += 4;
            }
            else
            {
                l_Attrib_Index++;
            }
            glVertexAttribPointer(index, l_Element.mt_Get_Element_Count(), fn_ShaderDataType_To_Type(l_Element.m_Type), l_Element.m_Nomalized ? GL_TRUE : GL_FALSE, cp.m_VB_Layout.mt_Get_Stride(), (const void*)l_Element.m_Offset);
            m_String_Data_Map[l_Element.m_Name] = m_VBO;
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_Instanced_VBO);
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(0*sizeof(glm::vec4)));
        glVertexAttribDivisor(5, 1);
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(1*sizeof(glm::vec4)));
        glVertexAttribDivisor(6, 1);
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2*sizeof(glm::vec4)));
        glVertexAttribDivisor(7, 1);
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3*sizeof(glm::vec4)));
        glVertexAttribDivisor(8, 1);


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, cp.m_Mesh->m_Indices.size() * sizeof(cp.m_Mesh->m_Indices[0]), cp.m_Mesh->m_Indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_Indice_Count = cp.m_Mesh->m_Indices.size();
    }

    glBindVertexArray(0);
}
#endif
void VertexArray::mt_Bind(void) const
{
    glBindVertexArray(m_Id);
}

void VertexArray::mt_Unbind(void) const
{
    glBindVertexArray(0);
}

bool VertexArray::mt_Update_Data(const std::string& name, const glm::mat4& matrix)
{
    bool l_b_Ret = false;
    auto l_it = m_String_Data_Map.find(name);

    if (l_it != m_String_Data_Map.end())
    {
        l_b_Ret = true;
    }

    return l_b_Ret;
}

bool VertexArray::mt_Update_Data(const std::string& name, const void* data, std::size_t byte_count, DrawingUsage usage)
{
    bool l_b_Ret = false;
    auto l_it = m_String_Data_Map.find(name);

    if (l_it != m_String_Data_Map.end())
    {
        l_b_Ret = l_it->second->mt_Update_Data(name, data, byte_count, usage);
    }

    return l_b_Ret;
}
