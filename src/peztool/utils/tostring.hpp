#pragma once
#include <cstdint>
#include <sstream>
#include <iomanip>

namespace pez
{

template<typename T>
std::string toString(const T& v, const uint8_t decimals = 2)
{
    std::stringstream sx;
    sx << std::setprecision(decimals) << std::fixed << v;
    return sx.str();
}

}