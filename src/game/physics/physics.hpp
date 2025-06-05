#pragma once
#include "peztool/core/system.hpp"
#include "collision_cell.hpp"
#include "physic_object.hpp"
#include "game/configuration.hpp"


struct PhysicSolver : public pez::Processor<pez::RequiredEntity<>>
{
    Configuration const& conf{pez::Singleton<Configuration>::get()};
	siv::Vector<PhysicObject> objects;
	CollisionGrid grid;

	// Simulation solving pass count
	uint32_t solver_iterations;
	uint32_t sub_steps;
	Vec2f gravity  = {0.0f, 0.0f};
    float frame_dt = 0.0f;

	// Parameters
	float response_coef = 0.1f;
	float vel_coef      = 0.1f;

	PhysicSolver()
		: grid(conf.world_size.x, conf.world_size.y)
		, solver_iterations(1)
		, sub_steps(4)
	{}

	// Checks if two atoms are colliding and if so create a new contact
	void checkContact(uint32_t atom_1, uint32_t atom_2)
	{
		if (atom_1 == atom_2) {
			return;
		}
        auto& obj_1 = objects[atom_1];
        auto& obj_2 = objects[atom_2];

		const Vec2f o2_o1 = obj_1.position - obj_2.position;
		const float dist2 = o2_o1.x * o2_o1.x + o2_o1.y * o2_o1.y;
		if (dist2 < 1.0f) {
			const float dist  = std::max(0.001f, sqrt(dist2));

            const float temp_response_coef = response_coef * (0.25f + 0.75f * (std::max(obj_1.temperature, obj_2.temperature)));
			const float delta = temp_response_coef * 0.5f * (1.0f - dist);
			const Vec2f col_vec = o2_o1 / dist;

			obj_1.position += col_vec * delta;
			obj_2.position -= col_vec * delta;

            obj_1.pressure += delta * obj_1.temperature;
            obj_2.pressure += delta * obj_2.temperature;

            const float temp_mean = (obj_1.temperature + obj_2.temperature) * 0.5f;

            const Vec2f delta_v = obj_1.getVelocity() - obj_2.getVelocity();
            const float cur_vel_coef = 0.01f * (temp_mean);
			obj_1.addVelocity(-delta_v * cur_vel_coef);
			obj_2.addVelocity( delta_v * cur_vel_coef);

            const float temp_coef = 1.5f;
            obj_1.temperature += (temp_mean - obj_1.temperature) * temp_coef * frame_dt;
            obj_2.temperature += (temp_mean - obj_2.temperature) * temp_coef * frame_dt;
		}
	}

	// Checks an atom collision against a cell
	void checkAtomsCollisions(uint32_t a, CollisionCell& cell)
	{
		for (uint8_t i(cell.objects_count); i--;) {
			checkContact(a, cell.objects[i]);
		}
	}

	void checkAtomCellCollisions(uint32_t id, CollisionCell& cell, uint32_t i)
	{
		checkAtomsCollisions(cell.objects[i], cell);
		checkAtomsCollisions(cell.objects[i], grid.data[id + 1]);
		checkAtomsCollisions(cell.objects[i], grid.data[id + 1 + grid.width]);
		checkAtomsCollisions(cell.objects[i], grid.data[id + grid.width]);
		checkAtomsCollisions(cell.objects[i], grid.data[id + 1 - grid.width]);
	}
	
	// Given an atom checks its collisions with close area
	void checkCellCollisions(uint32_t id)
	{
		for (uint8_t i(grid.data[id].objects_count); i--;) {
			checkAtomCellCollisions(id, grid.data[id], i);
		}
	}
	
	// Finds colliding atoms
	void findContacts()
    {
		for (uint32_t i(grid.used_count); i--;) {
			checkCellCollisions(grid.used[i]);
		}
	}

	// Applies impulse to solve collisions
	void solveCollisions()
	{
		for (uint32_t i(solver_iterations); i--;) {
			findContacts();
		}
	}

	// Add a new object to the solver
	uint64_t addObject(const PhysicObject& object)
	{
		const uint64_t object_id = objects.push_back(object);
		PhysicObject& new_object = objects[object_id];
		new_object.index = static_cast<uint32_t>(object_id);
		return object_id;
	}

    // Add a new object to the solver
    uint64_t createObject(Vec2f pos)
    {
        const siv::ID object_id  = objects.emplace_back(pos);
        PhysicObject& new_object = objects[object_id];
        new_object.index         = static_cast<uint32_t>(object_id);
        return object_id;
    }
    
	void update(float dt) override
	{
        frame_dt = dt / float(sub_steps);
        for (uint32_t i(sub_steps); i--;) {
            solveBorderCollisions();
            addObjectsToGrid();
            solveCollisions();
            updateObjects();
        }
	}

    void solveBorderCollisions()
    {
        constexpr float eps     = 0.1f;
        constexpr Vec2f min_pos = {1.0f + eps, 1.0f + eps};
        const Vec2f     max_pos = conf.world_size_f - min_pos;
        for (PhysicObject& obj : objects) {
            if (obj.position.x < min_pos.x) {
                obj.position.x = min_pos.x;
            } else if (obj.position.x > max_pos.x) {
                obj.position.x = max_pos.x;
            }

            if (obj.position.y < min_pos.y) {
                obj.position.y = min_pos.y;
            } else if (obj.position.y > max_pos.y) {
                obj.position.y = max_pos.y;
            }

            if ((obj.position.y - conf.world_size_f.y + 12.0f) > std::abs(conf.world_size_f.x * 0.5f - obj.position.x) * 0.2f) {
                obj.addTemperature(0.7f * frame_dt);
            }

            /*const Vec2 ball_pos = world_size * 0.5f;
            if (MathVec2::length(obj.position - ball_pos) < 20.0f && MathVec2::length(obj.position - ball_pos) > 10.0f) {
                obj.addTemperature(0.5f * frame_dt);
            }*/
        }
    }

	void addObjectsToGrid()
	{
		grid.clear();
		for (PhysicObject& obj : objects) {
		    auto& o{obj};
            grid.addAtom(static_cast<uint32_t>(o.position.x), static_cast<uint32_t>(o.position.y), o.index);
        }
	}

	void updateObjects()
	{
		for (PhysicObject& obj : objects) {
            obj.accelerate(gravity);
			obj.update(frame_dt);
		}
	}
};
