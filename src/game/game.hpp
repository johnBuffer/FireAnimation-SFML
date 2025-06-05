#pragma once
#include "configuration.hpp"
#include "peztool/core/scene.hpp"
#include "peztool/utils/number_generator.hpp"
#include "physics/physics.hpp"
#include "simulation/renderer.hpp"

struct Simulation final : public pez::Scene<pez::EntityPack<>, pez::SystemPack<PhysicSolver>, pez::SystemPack<SimulationRenderer>>
{
    Simulation()
    {
        m_resources.registerTexture("res/particle.png", "circle");
    }

    void registerEvents(pez::EventHandler& handler) override
    {
        handler.onKeyPressed(sf::Keyboard::Key::Escape, [this](sf::Event::KeyPressed const&) {
            pez::App::exit();
        });

        handler.onKeyPressed(sf::Keyboard::Key::S, [this](sf::Event::KeyPressed const&) {
            pez::App::toggleMaxFramerate();
        });
    }

    void onInitialized() override
    {
        auto const& conf = pez::Singleton<Configuration>::get();
        auto& solver = getProcessor<PhysicSolver>();
        solver.gravity = {0.0f, 20.0f};

        const uint32_t object_count = 22000;
        for (uint32_t i{object_count}; i--;) {
            solver.createObject({
                pez::RNGf::getRange(1.0f, conf.world_size_f.x - 1.0f),
                pez::RNGf::getRange(1.0f, conf.world_size_f.y - 1.0f)
            });
        }

        m_render_context->getWorldLayer().setViewPosition(conf.world_size_f * 0.5f);
        m_render_context->getWorldLayer().setZoom(1440.0f / conf.world_size_f.y);
    }
};


