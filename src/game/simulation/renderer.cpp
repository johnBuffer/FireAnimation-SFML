#include "./renderer.hpp"

#include "game/configuration.hpp"
#include "game/physics/physics.hpp"
#include "peztool/core/static_interface.hpp"


SimulationRenderer::SimulationRenderer()
    : objects_va(sf::PrimitiveType::Triangles)
{
}

void SimulationRenderer::render(pez::RenderContext& context)
{
    auto& solver = getProcessor<PhysicSolver>();
    auto& objects = solver.objects;
    objects_va.resize(6 * objects.size());
    uint32_t i{0};
    for (const PhysicObject& obj : objects) {
        addToVA(objects_va, i, obj);
        i += 6;
    }

    sf::RenderStates states;
    states.texture = texture;
    states.blendMode = sf::BlendAdd;
    context.getWorldLayer().draw(objects_va, states);
}

sf::Color SimulationRenderer::getColor(const PhysicObject& obj) {
    const auto r = static_cast<uint8_t>(255.0f * std::min(1.0f, obj.temperature * 3.0f));
    const auto g = static_cast<uint8_t>(255.0f * std::clamp((obj.temperature - 0.2f) * 3.0f, 0.0f, 1.0f));
    const auto b = static_cast<uint8_t>(255.0f * std::clamp((obj.temperature - 0.3f) * 3.0f, 0.0f, 1.0f));
    return {r, g, b};
}

void SimulationRenderer::addToVA(sf::VertexArray& va, size_t index, const PhysicObject& obj)
{
    float radius = obj.temperature * 2.5f;

    sf::Color const c = getColor(obj);

    const Vec2f velocity = obj.getVelocity();
    const float speed   = pez::length(velocity);
    const Vec2f dir      = velocity / speed;
    const Vec2f nrm      = pez::normal(dir);
    const Vec2f l = (1.0f + 40.0f * speed) * dir * radius;
    const Vec2f h = nrm * radius;

    va[index + 0].position = obj.position + l + h;
    va[index + 1].position = obj.position + l - h;
    va[index + 2].position = obj.position - l - h;

    va[index + 3].position = obj.position - l - h;
    va[index + 4].position = obj.position - l + h;
    va[index + 5].position = obj.position + l + h;

    va[index + 0].color = c;
    va[index + 1].color = c;
    va[index + 2].color = c;
    va[index + 3].color = c;
    va[index + 4].color = c;
    va[index + 5].color = c;

    va[index + 0].texCoords = {0.0f   , 0.0f};
    va[index + 1].texCoords = {1024.0f, 0.0f};
    va[index + 2].texCoords = {1024.0f, 1024.0f};
    va[index + 3].texCoords = {1024.0f, 1024.0f};
    va[index + 4].texCoords = {0.0f   , 1024.0f};
    va[index + 5].texCoords = {0.0f   , 0.0f};
}
