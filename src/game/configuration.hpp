#pragma once
#include "peztool/utils/vec.hpp"
#include "peztool/core/static_interface.hpp"


struct Configuration
{
    Vec2i const world_size{120, 120};
    Vec2f const world_size_f{static_cast<Vec2f>(world_size)};
};