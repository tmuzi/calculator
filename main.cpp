// g++ main.cpp `pkg-config --cflags --libs sdl3`

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>

bool init();
bool key_events();

/* Constants */
//Screen dimension constants
constexpr int kScreenHeight{ 518 };
constexpr int kScreenWidth{ 320 };


/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWindow{ nullptr };

int main()
{
	if (!init())
		return 1;

	while (key_events());

	SDL_Quit();

	return 0;
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
