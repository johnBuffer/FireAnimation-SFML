#pragma once
#include <cstdint>
#include "peztool/utils/grid.hpp"


constexpr uint8_t max_cell_capacity = 5;


struct CollisionCell
{
	// Overlap workaround
	uint8_t objects_count;
	uint32_t objects[max_cell_capacity];

	CollisionCell()
		: objects_count(0u)
	{}

	bool addAtom(uint32_t atom)
	{
		// Overflow safe, but dataloss
		objects[objects_count] = atom;
		objects_count += objects_count < (max_cell_capacity - 1);
		return objects_count == 1;
	}

	void clear()
	{
		objects_count = 0u;
	}
};

struct CollisionGrid : public pez::Grid<CollisionCell>
{
	std::vector<uint32_t> used;
	uint32_t used_count;

	CollisionGrid()
		: Grid<CollisionCell>()
		, used(0)
		, used_count(0)
	{}

	CollisionGrid(int32_t width, int32_t height)
		: Grid<CollisionCell>(width, height)
		, used(width * height, 0)
		, used_count(0)
	{}

	bool addAtom(uint32_t x, uint32_t y, uint32_t atom)
	{
		const uint32_t id = x + y * width;
		CollisionCell& cell = data[id];
		// Add to grid
		const bool first = data[id].addAtom(atom);
		used[used_count] += first * id;
		used_count += first;
		return true;
	}

	void clear()
	{
		for (uint32_t i(used_count); i--;) {
			data[used[i]].clear();
			used[i] = 0;
		}
		used_count = 0;
	}

    template<typename Vec2Type, typename TCallback>
    HitPoint getFirstHit(Vec2Type p, Vec2Type d, TCallback&& callback, float max_dist)
    {
        HitPoint intersection;
        Vec2i cell_p(to<int32_t>(p.x), to<int32_t>(p.y));
        const Vec2i step(d.x < 0.0f ? -1 : 1, d.y < 0.0f ? -1 : 1);
        const Vec2f inv_d(1.0f / d.x, 1.0f / d.y);
        const float t_dx = std::abs(inv_d.x);
        const float t_dy = std::abs(inv_d.y);
        float t_max_x = ((cell_p.x + (step.x > 0)) - p.x) * inv_d.x;
        float t_max_y = ((cell_p.y + (step.y > 0)) - p.y) * inv_d.y;
        while (intersection.dist < max_dist) {
            const uint32_t b = t_max_x < t_max_y;
            // Advance in grid
            intersection.dist = b * t_max_x + (!b) * t_max_y;
            t_max_x += t_dx * b;
            t_max_y += t_dy * (!b);
            cell_p.x += step.x * b;
            cell_p.y += step.y * (!b);

            // Wrap rays
            if (cell_p.x < 0         ) { cell_p.x = width - 1; }
            if (cell_p.x > width - 1 ) { cell_p.x = 0; }
            if (cell_p.y < 0         ) { cell_p.y = height - 1; }
            if (cell_p.y > height - 1) { cell_p.y = 0; }

            CollisionCell& cell = get(cell_p);
            if (callback(cell)) {
                intersection.cell = &cell;
                return intersection;
            }
        }
        return intersection;
    }
};
