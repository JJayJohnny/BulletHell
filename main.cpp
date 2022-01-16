#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

#include "game.h"
#include "common.h"

// main
#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char **argv) {
	
	
	int nextLevel = 0;
	// tryb pe³noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	while (true)
	{
		Game game;
		game.nextLevel = nextLevel;
		game.SelectLevel();
		game.SetupLevel();
		if (game.Update() == false)
		{
			break;
		}
		if (game.nextLevel != 0)
			nextLevel = game.nextLevel;
		else
			nextLevel = 0;
	}
	
	return 0;
};
