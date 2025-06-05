#pragma once
#include <cstdint>
#include <cmath>

namespace pez
{

// The easing function to use
enum class InterpolationFunction
{
    None,
    Linear,
    EaseInOutExponential,
    EaseInOutCirc,
    EaseInOutQuint,
    EaseOutBack,
    EaseInBack,
    Sigmoid
};

struct Interpolation
{
    static float flip(float x)
    {
        return 1 - x;
    }

    static float dumbPow(float x, uint32_t p)
    {
        float res = 1.0f;
        for (uint32_t i(p); i--;) {
            res *= x;
        }
        return res;
    }

    static float clampTime(float t)
    {
        return std::fmin(1.0f, std::fmax(t, 0.0f));
    }

    template<typename TValueType>
    static TValueType mix(const TValueType& a, const TValueType& b, float ratio)
    {
        return (1.0f - ratio) * a + ratio * b;
    }

    static float smoothStop(float t, uint32_t power)
    {
        const float t_ = clampTime(t);
        return flip(dumbPow(flip(t_), power));
    }

    static float smoothStart(float t, uint32_t power)
    {
        const float t_ = clampTime(t);
        return dumbPow(t_, power);
    }

    static float smoothStep(float t, uint32_t power)
    {
        return mix(smoothStart(t, power), smoothStop(t, power), t);
    }

    static float smoothStopF(float t, float power)
    {
        const float t_         = clampTime(t);
        const auto  base_power = static_cast<uint32_t>(power);
        return mix(flip(dumbPow(flip(t_), base_power)), flip(dumbPow(flip(t_), base_power + 1)), power - static_cast<float>(base_power));
    }

    static float smoothStartF(float t, float power)
    {
        const float t_         = clampTime(t);
        const auto  base_power = static_cast<uint32_t>(power);
        return mix(dumbPow(t_, base_power), dumbPow(t_, base_power + 1), power - static_cast<float>(base_power));
    }

    static float smoothStepF(float t, float power)
    {
        const float t_ = clampTime(t);
        return mix(smoothStartF(t_, power), smoothStopF(t_, power), t_);
    }

    static float sigmoid(float t, float speed = 1.0f)
    {
        const float s = 20.0f * speed;
        return 1.0f / (1.0f + exp(-(t - 0.5f) * s));
    }

    static float linear(float t, float speed = 1.0f)
    {
        return speed * t;
    }

    static float easeInOut(float t)
    {
        if (t < 0.5f) {
            return std::pow(2.0f, 20.0f * t - 10.0f) * 0.5f;
        }
        return (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) * 0.5f;
    }

    static float easeInOutCirc(float t)
    {
        if (t < 0.5f) {
            return (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) * 0.5f;
        }
        return (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) * 0.5f;
    }

    static float easeOutBack(float t)
    {
        constexpr float c1 = 1.70158f;
        constexpr float c3 = c1 + 1.0f;
        return 1.0f + c3 * dumbPow(t - 1.0f, 3) + c1 * dumbPow(t - 1.0f, 2);
    }

    static float easeInOutQuint(float t)
    {
        if (t < 0.5f) {
            return  16.0f * dumbPow(t, 5);
        }
        return 1.0f - dumbPow(-2.0f * t + 2, 5) * 0.5f;
    }

    static float easeInBack(float t) {
        float const c1 = 1.70158f;
        float const c3 = c1 + 1.0f;

        return c3 * t * t * t - c1 * t * t;

        }

    static float getInterpolationValue(float t, InterpolationFunction interpolation)
    {
        switch (interpolation) {
            case InterpolationFunction::None:
                return 1.0f;
            case InterpolationFunction::Linear:
                return t;
            case InterpolationFunction::EaseInOutExponential:
                return Interpolation::easeInOut(t);
            case InterpolationFunction::EaseInOutCirc:
                return Interpolation::easeInOutCirc(t);
            case InterpolationFunction::EaseInOutQuint:
                return Interpolation::easeInOutQuint(t);
            case InterpolationFunction::EaseOutBack:
                return Interpolation::easeOutBack(t);
            case InterpolationFunction::EaseInBack:
                return Interpolation::easeInBack(t);
            case InterpolationFunction::Sigmoid:
                return Interpolation::sigmoid(t);
        }
        // Default to linear
        return t;
    }
};

}
