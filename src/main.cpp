#define _USE_MATH_DEFINES
#include "Game.hpp"
int main() {
	Game game;

	while (game.isActive()) {
		game.update();
		game.render();
	}
	return EXIT_SUCCESS;
}
