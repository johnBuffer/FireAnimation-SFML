#pragma once
#include "peztool/core/system.hpp"
#include "game/physics/physics.hpp"


struct SimulationRenderer : public pez::Renderer<pez::RequiredEntity<>, pez::RequiredSystems<PhysicSolver>>
{
    sf::VertexArray objects_va;
    sf::Texture const* texture;

    explicit
    SimulationRenderer();

    void onInitialized() override
    {
        texture = getTexture("circle");
    }

    void render(pez::RenderContext& context) override;

private:
    static sf::Color getColor(const PhysicObject& obj);
    static void addToVA(sf::VertexArray& va, size_t index, const PhysicObject& obj);
};
