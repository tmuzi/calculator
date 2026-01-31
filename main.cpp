// g++ main.cpp `pkg-config --cflags --libs sdl3`

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

bool init();
bool key_events();
void draw();
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

	for (int row = 1; row < 6; row++) // start from 1 to skip first row to see color diff
	{
		buttonRect.y = startY + buttonHeight * row;
		for (int col = 0; col < 4; col++)
		{
			buttonRect.x = startX + buttonWidth * col;
			SDL_RenderFillRect(gRenderer, &buttonRect);
		}
		buttonRect.x = startX;
	}

	SDL_RenderPresent(gRenderer);
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
