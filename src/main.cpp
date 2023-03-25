#include <SDL.h>
#include <Game.h>


int main(int argc, char* argv[])
{
	Nobody::Game game;
	bool success = game.Initialize();
	if (success)
	{
		game.Loop();
	}
	game.Shutdown();

	return 0;
}