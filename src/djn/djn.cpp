#include "djn.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>	// Used very sparingly for : Input, window creation, Window final blit
#include <cstring>	// Memset
#include <cstdio>

// SDL stuff
static SDL_Window* gSDLWindow = nullptr;
static SDL_Surface* gSDLGameSurface = nullptr;

static SDL_Texture* gSDLTexture = nullptr;
static SDL_Renderer* gSDLRenderer = nullptr;

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
	bmask = 0b0000000000011111;
	gmask = 0b0000001111100000;
	rmask = 0b0111110000000000;
	amask = 0b0000000000000000;
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

	gSDLTexture = SDL_CreateTexture(gSDLRenderer, SDL_PIXELFORMAT_RGB555, SDL_TEXTUREACCESS_STREAMING, gScreenBuffer.w, gScreenBuffer.h);
	if (!gSDLTexture)
	{
		SDL_Log("SDL_CreateTexture() failed: %s", SDL_GetError());
		return false;
	}
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
	
	gSDLRenderer = SDL_CreateRenderer(gSDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!gSDLRenderer)
	{
		SDL_Log("Failed to create renderer");
		return false;
	}

	if (!CreateSurface())
	{
		SDL_Log("Failed to create surface");
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

	int pitch;
	void* pixels;
	int success = SDL_LockTexture(gSDLTexture, NULL, &pixels, &pitch);
	if (success != 0)
	{
		SDL_Log("Couldn't lock texture : %s\n", SDL_GetError());
	}
	gScreenBuffer.data = (djnPixel*) pixels;
	gConfig.DrawFunction();
	SDL_UnlockTexture(gSDLTexture);

	SDL_RenderClear(gSDLRenderer);
	SDL_RenderCopy(gSDLRenderer, gSDLTexture, NULL, NULL);
	SDL_RenderPresent(gSDLRenderer);

	//SDL_UpdateWindowSurface(gSDLWindow);
	printf("frame %7.2f\n", (gframe/60.0f));
	gframe++;
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


void djnBlit(djnImage& source, djnImage& target, uint16_t sx, uint16_t sy, uint16_t sw, uint16_t sh, uint16_t tx, uint16_t ty,djnBlitFlag flags)
{
	int dir_x = 1;
	int dir_y = 1;
	if (flags & djnBlitFlag::FLIP_X)
	{
		dir_x = -1;
		sx += sw - 1;
	}

	if (flags & djnBlitFlag::FLIP_Y)
	{
		dir_y = -1;
		sy += sh - 1;
	}


	for (uint16_t y = 0; y < sh; ++y)
	{
		for (uint16_t x = 0; x < sw; ++x)
		{
			djnPixel Pixel = source.get(sx + x*dir_x, sy + y*dir_y);
			if (Pixel & 0x8000)
				target.get(tx+x,ty+y) = Pixel;
		}
	}
}

void djnBlit(djnTile& source, djnImage& target, uint16_t tx, uint16_t ty, djnBlitFlag flags)
{
	djnBlit(*(source.Image), target, source.ox, source.oy, source.tw, source.th, tx, ty);
}