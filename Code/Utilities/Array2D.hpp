#ifndef _ARRAY_2D_HPP
#define _ARRAY_2D_HPP 1


template<typename T>
struct Array2D
{
    Array2D(int w, int h) : m_Buffer(nullptr), m_Width(0), m_Height(0)
    {
        mt_Resize(w, h);
    }

    ~Array2D()
    {
        mt_Clean();
    }

    T& operator()(int x, int y)
    {
        return m_Buffer[y * m_Width + x];
    }

    int mt_Get_Width(void) const
    {
        return m_Width;
    }

    int mt_Get_Height(void) const
    {
        return m_Height;
    }

    void mt_Resize(int w, int h)
    {
        if (m_Width != w || m_Height != h)
        {
            mt_Clean();
            m_Buffer = new T[w * h];
            m_Width = w;
            m_Height = h;
        }
    }

private:

    void mt_Clean()
    {
        if (m_Buffer != nullptr)
        {
            delete [] m_Buffer;

            m_Buffer = nullptr;
        }
    }

    T* m_Buffer;
    int m_Width;
    int m_Height;
};



#endif // _ARRAY_2D_HPP
