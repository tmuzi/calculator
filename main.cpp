#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>

/* Constants */
//Screen dimension constants
constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };


/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWindow{ nullptr };

int main()
{


	if (SDL_Init(SDL_INIT_VIDEO) == false)
	{
		SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());

		return 1;
	}
	
	if(gWindow = SDL_CreateWindow("Hello SDL3", kScreenWidth, kScreenHeight, 0); gWindow == nullptr)
	{
		SDL_Log( "Window could not be created! SDL error: %s\n", SDL_GetError() );

		return 1;
	}

	SDL_Delay(10000);

	SDL_Quit();

	return 0;
}
