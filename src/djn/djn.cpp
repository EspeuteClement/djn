#include "djn.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>	// Used very sparingly for : Input, window creation, Window final blit
#include <cstring>	// Memset
#include <cstdio>

// SDL stuff
static SDL_Window* gSDLWindow = nullptr;
static SDL_Surface* gSDLWindowSurface = nullptr;
static SDL_Surface* gSDLGameSurface = nullptr;

// Static djn stuff
static djnConfig gConfig;
static bool gQuitting = false;
static bool gInit = false;
djnImage gScreenBuffer;

uint32_t gframe = 0;

static bool CreateSurface()
{
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#error "Not implemented yet"
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0b0000000000011111;
	gmask = 0b0000001111100000;
	bmask = 0b0111110000000000;
	amask = 0b1000000000000000;
#endif

	gSDLGameSurface = SDL_CreateRGBSurface(0, 320, 240, 16,
		rmask, gmask, bmask, amask);
	if (gSDLGameSurface == nullptr) {
		SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
		return false;
	}

	gScreenBuffer.w = 320;
	gScreenBuffer.h = 240;
	gScreenBuffer.data = (uint16_t*)gSDLGameSurface->pixels;
	return true;
}

bool djnInit(djnConfig config)
{
	SDL_assert(config.UpdateFunction != nullptr); // Update function must be configured
	SDL_assert(config.DrawFunction != nullptr);	  // Draw function must be configured

	gConfig = config;
	gInit = false;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}

	gSDLWindow = SDL_CreateWindow("title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (!gSDLWindow)
	{
		SDL_Log("Failed to initialize SDL window: %s", SDL_GetError());
		return false;
	}
	gSDLWindowSurface = SDL_GetWindowSurface(gSDLWindow);
	if (!CreateSurface())
	{
		SDL_Log("Failed to initialize Surface: %s", SDL_GetError());
		return false;
	}

	gInit = true;
	gQuitting = false;

	return true;
}

static void _djnStep()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			gQuitting = true;
		}
	}

	gConfig.UpdateFunction();

	SDL_LockSurface(gSDLGameSurface);
	gConfig.DrawFunction();
	SDL_UnlockSurface(gSDLGameSurface);

	//SDL_LockSurface(gSDLWindowSurface);
	SDL_BlitScaled(gSDLGameSurface, &SDL_Rect({ 0,0,GAME_WIDTH, GAME_HEIGHT }), gSDLWindowSurface, &SDL_Rect({ 0,0,WINDOW_WIDTH, WINDOW_HEIGHT }));
	//SDL_UnlockSurface(gSDLWindowSurface);

	SDL_UpdateWindowSurface(gSDLWindow);
	printf("frame %d", gframe++);
	// Todo : True sync
	//SDL_Delay(1);
}


void djnRun()
{
	if (gInit)
	{
#ifdef __EMSCRIPTEN__
		// void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
		emscripten_set_main_loop(_djnStep, 60, 1);
#else
		while (!gQuitting) {
			_djnStep();
		}
#endif
	}
}

void DestroySurface()
{
	if (gSDLGameSurface)
	{
		SDL_FreeSurface(gSDLGameSurface);
	}

	gSDLGameSurface = nullptr;
}

// Exit as cleanly as possible
void djnDeInit()
{
	DestroySurface();

	if (gSDLWindow)
	{
		SDL_DestroyWindow(gSDLWindow);
	}

	SDL_Quit();
}

#define PIXEL(buffer, x,y,w) (buffer[(x)+(y)*(w)])

void djnBlit(djnImage& source, djnImage& target, int sx, int sy, int sw, int sh, int tx, int ty)
{
	for (int y = 0; y < sh; ++y)
	{
		for (int x = 0; x < sw; ++x)
		{
			djnPixel Pixel = source.get(sx + x, sy + y);
			if (Pixel & 0x8000)
				target.get(tx+x,ty+y) = Pixel;
		}
	}
}
