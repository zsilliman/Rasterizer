#pragma once
#include <SDL.h>
#include <memory>
#include "SceneTexture.h"

using namespace std;

class ZSWindow
{

public:

	shared_ptr<SDL_Window> window = nullptr;
	shared_ptr<SDL_Renderer> renderer = nullptr;
	int ScreenWidth = 0;
	int ScreenHeight = 0;
	string AppTitle;

	ZSWindow(string Title, int Width, int Height);

	bool init();

	void ClearWindow();

	void RenderSceneTexture(shared_ptr<SceneTexture> tex);

	void Present();

	void close();

};

