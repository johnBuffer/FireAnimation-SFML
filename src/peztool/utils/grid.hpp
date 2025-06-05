#pragma once
#include <vector>
#include <array>


namespace pez
{
template<typename T>
struct Grid
{
    struct HitPoint
    {
        const T* cell;
        float dist;

        HitPoint()
            : cell(nullptr)
            , dist(0.0f)
        {}
    };

    int32_t width, height;
    std::vector<T> data;

    Grid()
        : width(0)
        , height(0)
    {}

    Grid(int32_t width_, int32_t height_)
        : width(width_)
        , height(height_)
    {
        data.resize(width * height);
    }

    template<typename Vec2Type>
    bool checkCoords(const Vec2Type& v) const
    {
        return checkCoords(static_cast<int32_t>(v.x), static_cast<int32_t>(v.y));
    }

    bool checkCoords(int32_t x, int32_t y) const
    {
        return (x > 0) && (x < (width - 1)) &&
               (y > 0) && (y < (height - 1));
    }

    const T& get(int32_t x, int32_t y) const
    {
        return data[y * width + x];
    }

    template<typename Vec2Type>
    T& get(const Vec2Type& v)
    {
        return get(static_cast<int32_t>(v.x), static_cast<int32_t>(v.y));
    }

    template<typename Vec2Type>
    const T& get(const Vec2Type& v) const
    {
        return get(static_cast<int32_t>(v.x), static_cast<int32_t>(v.y));
    }

    template<typename Vec2Type>
    const T& getWrap(Vec2Type v) const
    {
        return getWrap(v.x, v.y);
    }

    T& get(int32_t x, int32_t y)
    {
        return data[y * width + x];
    }

    template<typename Vec2Type>
    void set(const Vec2Type& v, const T& obj)
    {
        set(v.x, v.y, obj);
    }

    void set(int32_t x, int32_t y, const T& obj)
    {
        data[y * width + x] = obj;
    }
};
}
