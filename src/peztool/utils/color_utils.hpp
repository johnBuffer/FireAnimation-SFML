#pragma once
#include <cstdint>
#include "SFML/Graphics/Color.hpp"


namespace pez
{

template<typename TVec4>
TVec4 getVec(sf::Color color)
{
    return {static_cast<float>(color.r),
            static_cast<float>(color.g),
            static_cast<float>(color.b),
            static_cast<float>(color.a)
    };
}

template<typename TVec4>
sf::Color getColor(TVec4 vec)
{
    return {static_cast<uint8_t>(std::clamp(vec.x, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(vec.y, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(vec.z, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(vec.w, 0.0f, 255.0f)),
    };
}

inline sf::Color setAlpha(sf::Color color, uint8_t alpha)
{
    return {color.r, color.g, color.b, alpha};
}

}