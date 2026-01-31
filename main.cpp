// g++ main.cpp `pkg-config --cflags --libs sdl3`

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>

bool init();

bool key_events();

void draw();
void drawLayout();

void close();

/* Constants */
//Screen dimension constants
constexpr int kScreenHeight{ 518 };
constexpr int kScreenWidth{ 320 };


/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWindow{ nullptr };

//The surface contained by the window
SDL_Surface* gScreenSurface{ nullptr };
SDL_Renderer* gRenderer{ nullptr };

int main()
{
	if (!init())
		return 1;

	while (key_events())
		draw();

	close();

	return 0;
}

void draw()
{
	//Fill the surface white
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	drawLayout();

	SDL_RenderPresent(gRenderer);
}

void drawLayout()
{
	/*
		layout:

		screen 25%:
		-------------
		|			|
		|			|
		board 75%:
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		-------------
	*/
	//screen
	float startX { 0 };
	float startY { 0 };

	float screenHeight { kScreenHeight * 0.25f };
	float screenWidth { kScreenWidth };

	SDL_FRect screenRect { startX, startY, screenWidth, screenHeight };

	SDL_SetRenderDrawColor(gRenderer, 0xD3, 0xD3, 0xD3, 0xFF); // light gray
	SDL_RenderFillRect(gRenderer, &screenRect);

	//	write to screen
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_SetRenderScale(gRenderer, 3.5f, 3.5f);
    SDL_RenderDebugText(gRenderer, 10, 20, "0000000000");

	SDL_SetRenderScale(gRenderer, 1.0f, 1.0f); // reset scale
	//board
	startY += screenHeight;

	float boardHeight { kScreenHeight * 0.75f };
	float boardWidth { kScreenWidth };

	float buttonHeight { boardHeight * 0.1667f };
	float buttonWidth { boardWidth * 0.25f };

	SDL_FRect boardRect { startX, startY, boardWidth, boardHeight };

	SDL_SetRenderDrawColor(gRenderer, 0xA9, 0xA9, 0xA9, 0xFF); // dark gray (board background)
	SDL_RenderFillRect(gRenderer, &boardRect);

	//buttons

	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);

	SDL_FRect buttonRect { startX, startY, buttonWidth - 1.0f, buttonHeight - 1.0f }; // button with gap

	for (int row = 0; row < 6; row++)
	{
		buttonRect.y = startY + buttonHeight * row;
		for (int col = 0; col < 4; col++)
		{
			buttonRect.x = startX + buttonWidth * col;

			if (row == 0) // top row different color
				 SDL_SetRenderDrawColor(gRenderer, 0xA9, 0xA9, 0xA9, 0xFF); // dark gray (board background)
			else
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);

			SDL_RenderFillRect(gRenderer, &buttonRect);

			//	write to button
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF); // dark gray
			//SDL_SetRenderScale(gRenderer, 3.5f, 3.5f);
			SDL_RenderDebugText(
				gRenderer,
				(buttonRect.x + buttonWidth / 2) - 4,
				(buttonRect.y + buttonHeight / 2) - 4,
				std::to_string((row * (4)) + col + 1).c_str()
			); // default char dim 8x8 hence -4 to center
		}
		buttonRect.x = startX;
	}
}

bool init()
{


	if (SDL_Init(SDL_INIT_VIDEO) == false)
	{
		SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());

		return false;
	}
	
	if (!SDL_CreateWindowAndRenderer("Hello SDL3", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer))
	{
		SDL_Log("Window/Renderer could not be created! SDL error: %s\n", SDL_GetError());

		return false;
    }

	return true;
}

bool key_events()
{
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		switch (e.type)
			case SDL_EVENT_QUIT:
				return false;
	}
	return true;
}

void close()
{

	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = nullptr;
	gScreenSurface = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
}
