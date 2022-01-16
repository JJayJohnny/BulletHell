#include "game.h"
#include <iostream>

SDL_Surface* Game::LoadGraphic(char path[])
{
	SDL_Surface* graphic;
	graphic = SDL_LoadBMP(path);
	if (graphic == NULL) {
		printf("Image load error: %s\n", SDL_GetError());
		ExitGame();
	}
	return graphic;
}

// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void Game::DrawString(int x, int y, const char* text) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};

// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void Game::DrawSurface(SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};

// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};

// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void Game::DrawLine(int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};

// rysowanie prostok¹ta o d³ugoœci boków l i k
// draw a rectangle of size l by k
void Game::DrawRectangle(int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) 
{
	int i;
	DrawLine(x, y, k, 0, 1, outlineColor);
	DrawLine(x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(x, y, l, 1, 0, outlineColor);
	DrawLine(x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(x + 1, i, l - 2, 1, 0, fillColor);
};


Game::Game()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		ExitGame();
	}
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		&window, &renderer);
	if (rc != 0) {
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		ExitGame();
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Bullet HELL");

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_ShowCursor(SDL_DISABLE);

	charset = LoadGraphic("./sprites/cs8x8.bmp");
	SDL_SetColorKey(charset, true, 0x000000);
	backgrounds = new SDL_Surface*[3];
	backgrounds[0] = SDL_LoadBMP(background1Path);
	backgrounds[1] = SDL_LoadBMP(background2Path);
	backgrounds[2] = SDL_LoadBMP(background3Path);
	deathScreen = SDL_LoadBMP(deathScreenPath);
	victoryScreen = SDL_LoadBMP(victoryScreenPath);
	healthPack = SDL_LoadBMP(healthPackPath);
	healthPacks = new HealthPack[maxHealthPackCount];
	healthPackLastPlaced = 0;

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	nextLevel = 0;
	srand(time(NULL));
}

bool Game::Update()
{
	while (!quit) {
		t2 = SDL_GetTicks();

		// w tym momencie t2-t1 to czas w milisekundach,
		// jaki uplyna³ od ostatniego narysowania ekranu
		// delta to ten sam czas w sekundach
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		worldTime += delta;

		if (GetInput() == 'n')
		{
			ExitGame();
			return true;
		}
		player.Move(delta);
		player.MoveBullets(delta);
		player.InvincibilityCountdown(worldTime);

		enemy.Shoot(worldTime, delta);

		enemy.Move(worldTime, delta);

		ManageBullets();
		ManageHealthPacks();

		SDL_FillRect(screen, NULL, CZARNY);
		DrawSurface(backgrounds[currLevel - 1], LEVEL_WIDTH / 2 + player.xOff, LEVEL_HEIGHT / 2 + player.yOff); //tlo
		DrawSurface(player.activeSprite, player.x + player.xOff, player.y + player.yOff); //gracz
		if (player.invincible)
			DrawSurface(player.shield, player.x + player.xOff, player.y + player.yOff);//tarcza
		DrawSurface(enemy.sprite, enemy.x + player.xOff, enemy.y + player.yOff); //przeciwnik
		DisplayBullets();
		DisplayHealthPacks();
		DisplayHealthBar(player.health, player.maxHealth, player.x + player.xOff, player.y + player.yOff - player.height/2 - 30);
		DisplayHealthBar(enemy.health, enemy.maxHealth, enemy.x + player.xOff, enemy.y + player.yOff - enemy.height/2 - 30);

		fpsTimer += delta;
		if (fpsTimer > 0.5) {
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		};

		DrawInterface();		
		frames++;
	};
	if (player.health <= 0)
	{
		DeathScreen();
		ExitGame();
		return true;
	}
	else if (enemy.health <= 0)
	{
		VictoryScreen();
		ExitGame();
		return true;
	}
	else
	{
		ExitGame();
		return false;
	}
}

void Game::DrawInterface()
{
	char text[128];
	// tekst informacyjny / info text
	DrawRectangle(4, 4, SCREEN_WIDTH/4, 20, ZIELONY, CZARNY);
	DrawRectangle(SCREEN_WIDTH/2, 4, SCREEN_WIDTH/2 - 10, 20, ZIELONY, CZARNY);
	DrawRectangle(5, screen->h - 24, SCREEN_WIDTH / 2 - 40, 20, ZIELONY, CZARNY);
	//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
	sprintf(text, "Bullet Hell Game, poziom %i", currLevel);
	DrawString(10, 10, text);
	sprintf(text, "czas trwania = %.1lf s  %.0lf klatek / s", worldTime, fps);
	DrawString(screen->w / 2 + 5, 10, text);
	//	      "Esc - exit, \030 - faster, \031 - slower"
	sprintf(text, "Esc - wyjscie, \030 \031 \032 \033 - movement, N - new game, W A S D - shooting");
	DrawString(10, screen->h - 20, text);
	sprintf(text, "PLAYER X: %f Y: %f", player.x, player.y);
	DrawString(50, 50, text);
	sprintf(text, "Points: %i, Multiplier: %i", player.score, player.multiplier);
	DrawString(50, 90, text);

	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	//SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

char Game::GetInput()
{
	// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
	while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				if (event.key.keysym.sym == SDLK_n) return 'n';
				if (event.key.keysym.sym == SDLK_UP) player.yVel = -1;
				if (event.key.keysym.sym == SDLK_DOWN) player.yVel = 1;
				if (event.key.keysym.sym == SDLK_LEFT)
				{
					player.xVel = -1;
					player.activeSprite = player.spriteL;
				}
				if (event.key.keysym.sym == SDLK_RIGHT)
				{
					player.xVel = 1;
					player.activeSprite = player.spriteR;
				}
				if (event.key.keysym.sym == SDLK_w) player.Shoot(worldTime, 'N');
				if (event.key.keysym.sym == SDLK_d) player.Shoot(worldTime, 'E');
				if (event.key.keysym.sym == SDLK_s) player.Shoot(worldTime, 'S');
				if (event.key.keysym.sym == SDLK_a) player.Shoot(worldTime, 'W');
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_UP && player.yVel < 0) player.yVel = 0;
				if (event.key.keysym.sym == SDLK_DOWN && player.yVel > 0) player.yVel = 0;
				if (event.key.keysym.sym == SDLK_LEFT && player.xVel < 0) player.xVel = 0;
				if (event.key.keysym.sym == SDLK_RIGHT && player.xVel > 0) player.xVel = 0;
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
	};
}

void Game::ExitGame()
{
	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_FreeSurface(backgrounds[0]);
	SDL_FreeSurface(backgrounds[1]);
	SDL_FreeSurface(backgrounds[2]);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

void Game::SelectLevel()
{
	if (nextLevel == 0)
	{
		PrintTitleScreen();

		bool start = false;
		while (!start)
		{
			while (SDL_WaitEvent(&event))
			{
				if (event.type == SDL_KEYDOWN)
				{
					start = true;
					switch (event.key.keysym.sym)
					{
					case SDLK_1:
						currLevel = 1;
						break;
					case SDLK_2:
						currLevel = 2;
						break;
					case SDLK_3:
						currLevel = 3;
						break;
					case SDLK_ESCAPE:
						ExitGame();
						exit(0);
					default:
						start = false;
					}
				}
				if (start)
					break;
			}
		}
	}
	else
	{
		currLevel = nextLevel;
		nextLevel = 0;
	}
}

void Game::PrintTitleScreen()
{
	SDL_FillRect(screen, NULL, CZARNY);

	DrawRectangle(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ZIELONY, CZARNY);
	char text[120];
	sprintf(text, "Bullet Hell - The Game");
	DrawString(SCREEN_WIDTH / 2 - strlen(text) * 4, SCREEN_HEIGHT / 4 + 10, text);
	sprintf(text, "By Jan Barczewski - 188679");
	DrawString(SCREEN_WIDTH / 2 - strlen(text) * 4, SCREEN_HEIGHT / 4 + 30, text);
	sprintf(text, "Select level: ");
	DrawString(SCREEN_WIDTH / 2 - strlen(text) * 4, SCREEN_HEIGHT / 4 + 90, text);
	sprintf(text, "Level 1");
	DrawString(SCREEN_WIDTH / 2 - strlen(text) * 4, SCREEN_HEIGHT / 4 + 110, text);
	sprintf(text, "Level 2");
	DrawString(SCREEN_WIDTH / 2 - strlen(text) * 4, SCREEN_HEIGHT / 4 + 130, text);
	sprintf(text, "Level 3");
	DrawString(SCREEN_WIDTH / 2 - strlen(text) * 4, SCREEN_HEIGHT / 4 + 150, text);
	sprintf(text, "Esc - Wyjscie z gry");
	DrawString(SCREEN_WIDTH / 2 - strlen(text) * 4, SCREEN_HEIGHT / 4 + 190, text);

	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	//SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void Game::SetupLevel()
{
	enemy.type = currLevel;
	
	switch (currLevel)
	{
	case 1:
		enemy.x = LEVEL_WIDTH / 7;
		enemy.y = LEVEL_HEIGHT / 6;
		enemy.width = 178;
		enemy.height = 285;
		enemy.cooldown = 1;
		enemy.sprite = SDL_LoadBMP(enemy1SpritePath);
		enemy.health = 10;
		enemy.maxHealth = enemy.health;
		break;
	case 2:
		enemy.x = LEVEL_WIDTH / 6 * 4;
		enemy.y = LEVEL_HEIGHT / 6;
		enemy.width = 250;
		enemy.height = 250;
		enemy.cooldown = 0.15;
		enemy.sprite = SDL_LoadBMP(enemy2SpritePath);
		enemy.health = 15;
		enemy.maxHealth = enemy.health;
		break;
	case 3:
		enemy.x = LEVEL_WIDTH / 2;
		enemy.y = LEVEL_HEIGHT / 4;
		enemy.width = 250;
		enemy.height = 310;
		enemy.cooldown = 1;
		enemy.speed = 200;
		enemy.sprite = SDL_LoadBMP(enemy3SpritePath);
		enemy.health = 25;
		enemy.maxHealth = enemy.health;
		break;
	}
}

void Game::ManageBullets()
{
	//pociski przeciwnika
	for (int i = 0; i < enemy.bulletCount; i++)
	{
		if (enemy.bullets[i].active)
		{
			if (enemy.bullets[i].CheckHitbox(player.x, player.y, player.width, player.height) && player.invincible == false)//gracz dostal
			{
				player.CalcScore('-');
				enemy.bullets[i].active = false;
				player.invincible = true;
				player.timeOfHit = worldTime;
				player.health--;
				if (player.health <= 0)
					quit = 1;
			}
		}
	}

	//pociski gracza
	for (int i = 0; i < player.bulletCount; i++)
	{
		if (player.bullets[i].active)
		{
			if (player.bullets[i].CheckHitbox(enemy.x, enemy.y, enemy.width, enemy.height))
			{
				player.CalcScore('+');
				player.bullets[i].active = false;
				enemy.health--;
				if (enemy.health <= 0)
					quit = 1;
			}
		}
	}
}

void Game::DisplayBullets()
{
	for(int i=0; i<enemy.bulletCount; i++)
		if(enemy.bullets[i].active)
			DrawSurface(enemy.bulletSprite, enemy.bullets[i].x + player.xOff, enemy.bullets[i].y + player.yOff);
	for(int i=0; i<player.bulletCount; i++)
		if(player.bullets[i].active)
			DrawSurface(player.bulletSprite, player.bullets[i].x + player.xOff, player.bullets[i].y + player.yOff);
}

void Game::DisplayHealthBar(int health, int maxHealth, double x, double y)
{
	int barLength = 110;
	double step = barLength / maxHealth;
	if (health >= 0 && maxHealth >= 0 && x - maxHealth*step/2 > 0 && x+maxHealth*step/2 < SCREEN_WIDTH && y>0 && y<SCREEN_HEIGHT)
	{
		for (int i = 0; i < maxHealth; i++)
		{
			for (int j = 0; j < step; j++)
			{
				if (i < health)//czerwony
				{
					DrawLine(x - maxHealth*step / 2 + i * step + j, y, 10, 0, 1, CZERWONY);
				}
				else//czarny
				{
					DrawLine(x - maxHealth*step / 2 + i * step + j, y, 10, 0, 1, CZARNY);
				}
			}
		}
	}
}

void Game::DeathScreen()
{
	DrawSurface(deathScreen, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	DrawRectangle(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 *3, SCREEN_WIDTH / 2, 30, ZIELONY, CZARNY);
	char text[120];
	sprintf(text, "Press Enter to retry, Esc to Exit");
	DrawString(SCREEN_WIDTH / 2 - strlen(text) * 4, SCREEN_HEIGHT / 4 *3 + 15, text);
	DrawInterface();
	bool cont = false;
	while (!cont)
	{
		while (SDL_WaitEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				cont = true;
				switch (event.key.keysym.sym)
				{
				case SDLK_RETURN:
					nextLevel = currLevel;
					break;
				case SDLK_ESCAPE:
					nextLevel = 0;
					break;
				default:
					cont = false;
				}
			}
			if (cont)
				break;
		}
	}
}

void Game::VictoryScreen()
{
	DrawSurface(victoryScreen, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	DrawRectangle(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 * 3, SCREEN_WIDTH / 2, 70, ZIELONY, CZARNY);
	char text[120];
	sprintf(text, "!!!!Your Score: %i!!!!", player.score);
	DrawString(SCREEN_WIDTH / 2 - strlen(text) * 4, SCREEN_HEIGHT / 4 * 3 + 15, text);
	sprintf(text, "Enter - go to the next level, Esc - Exit");
	DrawString(SCREEN_WIDTH / 2 - strlen(text) * 4, SCREEN_HEIGHT / 4 * 3 + 40, text);
	DrawInterface();
	bool cont = false;
	while (!cont)
	{
		while (SDL_WaitEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				cont = true;
				switch (event.key.keysym.sym)
				{
				case SDLK_RETURN:
					nextLevel = currLevel%3+1;
					break;
				case SDLK_ESCAPE:
					nextLevel = 0;
					break;
				default:
					cont = false;
				}
			}
			if (cont)
				break;
		}
	}
}

void Game::ManageHealthPacks()
{
	if (worldTime - healthPackLastPlaced >= healthPackCooldown)
	{
		for (int i = 0; i < maxHealthPackCount; i++)
		{
			if (healthPacks[i].active == false)
			{
				healthPackLastPlaced = worldTime;
				healthPacks[i].x = rand() % LEVEL_WIDTH;
				healthPacks[i].y = rand() % LEVEL_HEIGHT;
				healthPacks[i].active = true;
				break;
			}
		}
	}
	for (int i = 0; i < maxHealthPackCount; i++)
	{
		if (healthPacks[i].CheckHitbox(player.x, player.y, player.width, player.height) && player.health < player.maxHealth)
		{
			player.health++;
			if (player.health > player.maxHealth)
				player.health = player.maxHealth;
			healthPacks[i].active = false;
		}
	}
}

void Game::DisplayHealthPacks()
{
	for (int i = 0; i < maxHealthPackCount; i++)
	{
		if (healthPacks[i].active)
			DrawSurface(healthPack, healthPacks[i].x + player.xOff, healthPacks[i].y + player.yOff);
	}
}