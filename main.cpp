// g++ main.cpp `pkg-config --cflags --libs sdl3`

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>

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
	SDL_FillSurfaceRect(gScreenSurface, nullptr, SDL_MapSurfaceRGB(gScreenSurface, 0xFF, 0xFF, 0xFF));

	//Update the surface
	SDL_UpdateWindowSurface(gWindow);
}

bool init()
{


	if (SDL_Init(SDL_INIT_VIDEO) == false)
	{
		SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());

		return false;
	}
	
	if(gWindow = SDL_CreateWindow("Hello SDL3", kScreenWidth, kScreenHeight, 0); gWindow == nullptr)
	{
		SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());

		return false;
	}
	else
		gScreenSurface = SDL_GetWindowSurface(gWindow); //Get window surface

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
