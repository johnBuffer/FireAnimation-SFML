#pragma once
#include <SFML/System/Vector2.hpp>
#include "peztool/utils/math.hpp"

struct PhysicObject
{
	uint32_t index;
	// Verlet
	Vec2f position      = {0.0f, 0.0f};
	Vec2f last_position = {0.0f, 0.0f};
	Vec2f acceleration  = {0.0f, 0.0f};
    // Properties
    float temperature = 0.0f;
    float pressure    = 0.0f;

	PhysicObject()
		: index(0)
	{}

    explicit
    PhysicObject(Vec2f position_)
        : index(0)
        , position(position_)
        , last_position(position_)
    {}

	void setPosition(Vec2f pos)
	{
		position = pos;
		last_position = pos;
	}

	void accelerate(Vec2f a)
	{
		acceleration += a;
	}

	void update(float dt)
	{
        const Vec2f velocity = position - last_position;
        temperature -= temperature * 1.5f * dt;
		last_position = position;
        acceleration -= velocity * 250.0f;
		position      = position + velocity + acceleration * (dt * dt);
		// Reset acceleration
        acceleration = {};
        acceleration.y = -std::pow(1.0f + temperature, 2.0f) * pressure * 1200.0f;
        pressure *= 0.96f;
	}

	void stop()
	{
		last_position = position;
	}

	void slowdown(float ratio)
	{
		last_position = last_position + ratio * (position - last_position);
	}

	float getSpeed() const
	{
		return pez::length(position - last_position);
	}

	Vec2f getVelocity() const
	{
		return position - last_position;
	}

    void addTemperature(float temp)
    {
        temperature += temp;
        temperature = std::min(1.0f, temperature);
    }

	void addVelocity(Vec2f v)
	{
		last_position -= v;
	}

    void setPositionSameSpeed(Vec2f new_position)
    {
        const Vec2f to_last = last_position - position;
        position      = new_position;
        last_position = position + to_last;
    }
};
