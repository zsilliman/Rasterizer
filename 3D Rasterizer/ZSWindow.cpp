#include "ZSWindow.h"
#include "stdio.h"

ZSWindow::ZSWindow(std::string Title, int Width, int Height)
{
	AppTitle = Title;
	ScreenWidth = Width;
	ScreenHeight = Height;
}

bool ZSWindow::init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	//Create window
	window = shared_ptr<SDL_Window>(
		SDL_CreateWindow(AppTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN),
		[](SDL_Window* wind) {SDL_DestroyWindow(wind); });
	if (window.get() == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	//Create Renderer
	renderer = shared_ptr<SDL_Renderer>(SDL_CreateRenderer(window.get(), -1, 0), [](SDL_Renderer* rend) {SDL_DestroyRenderer(rend); });
	if (renderer.get() == NULL) {
		printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	return success;
}

void ZSWindow::ClearWindow() {
	SDL_RenderClear(renderer.get());
}

void ZSWindow::RenderSceneTexture(shared_ptr<SceneTexture> tex) {
	SDL_RenderCopy(renderer.get(), tex->texture.get(), NULL, NULL);
}

void ZSWindow::Present() {
	SDL_RenderPresent(renderer.get());
}

void ZSWindow::close()
{
	//Quit SDL subsystems
	SDL_Quit();
}