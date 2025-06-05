#include "peztool/peztool.hpp"
#include "game.hpp"
#include "configuration.hpp"

int32_t main()
{
    // Create application context
    Vec2u const window_size{2560, 1440};
	pez::App app(window_size, window_size, "Fire", sf::State::Windowed);
    // Load resources
    // Initialize agents
    pez::Singleton<Configuration>::create();
    app.addScene<Simulation>();
    app.run();
	return 0;
}
