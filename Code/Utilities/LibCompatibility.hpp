#ifndef _LIB_COMPATIBILITY_HPP
#define _LIB_COMPATIBILITY_HPP 1


#include "glm/glm.hpp"
#include <SFML/Graphics.hpp>

inline glm::mat4 fn_SFML_Transform_To_GLM(const sf::Transform& t)
{
    const float* l_Matrix = t.getMatrix();
    return glm::mat4(l_Matrix[0], l_Matrix[1], l_Matrix[2], l_Matrix[3],
                     l_Matrix[4], l_Matrix[5], l_Matrix[6], l_Matrix[7],
                     l_Matrix[8], l_Matrix[9], l_Matrix[10], l_Matrix[11],
                     l_Matrix[12], l_Matrix[13], l_Matrix[14], l_Matrix[15]);
}


#endif // _LIB_COMPATIBILITY_HPP
