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

struct djnMapping
{
	int16_t player;
	int16_t identifier;
	djnBtn btn;
};

#define MAX_MAPPINGS 16
static djnMapping KeyboardMappings[MAX_MAPPINGS];
static djnMapping JoystickMappings[MAX_MAPPINGS];

#define DJN_BTN_HISTORY_COUNT 2
static uint16_t gDjnBtnHistory[DJN_BTN_HISTORY_COUNT];
static uint8_t gDjnBtnFrame;

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

	for (int i = 0; i < MAX_MAPPINGS; ++i)
	{
		KeyboardMappings[i] = { -1,-1, djnBtn::NONE};
	}

	for (int i = 0; i < DJN_BTN_HISTORY_COUNT; ++i)
	{
		gDjnBtnHistory[i] = 0;
	}
	gDjnBtnFrame = 0;

	djnInputRegisterKeyboard(SDL_SCANCODE_UP, 0, djnBtn::UP);
	djnInputRegisterKeyboard(SDL_SCANCODE_DOWN, 0, djnBtn::DOWN);
	djnInputRegisterKeyboard(SDL_SCANCODE_LEFT, 0, djnBtn::LEFT);
	djnInputRegisterKeyboard(SDL_SCANCODE_RIGHT, 0, djnBtn::RIGHT);

	djnInputRegisterKeyboard(SDL_SCANCODE_X, 0, djnBtn::A);
	djnInputRegisterKeyboard(SDL_SCANCODE_C, 0, djnBtn::B);
	djnInputRegisterKeyboard(SDL_SCANCODE_Q, 0, djnBtn::X);
	djnInputRegisterKeyboard(SDL_SCANCODE_S, 0, djnBtn::Y);





	return true;
}

static void _djnUpdateBtn()
{
	gDjnBtnFrame = (gDjnBtnFrame + 1) % DJN_BTN_HISTORY_COUNT;
	const Uint8* kbd = SDL_GetKeyboardState(NULL);

	gDjnBtnHistory[gDjnBtnFrame] = 0;
	
	for (int i = 0; i < MAX_MAPPINGS; ++i)
	{
		djnMapping& map = KeyboardMappings[i];
		if (map.btn != djnBtn::NONE)
		{
			gDjnBtnHistory[gDjnBtnFrame] |= uint16_t(kbd[map.identifier]) << int16_t(map.btn);
		}
		else
		{
			break;
		}
	}

	for (int i = 0; i < uint8_t(djnBtn::COUNT); ++i)
	{
		printf("%d", djnInputBtnDown(djnBtn(i)));
	}
	printf("\n");

}

static void _djnStep()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			gQuitting = true;
		}
	}

	_djnUpdateBtn();

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
	//printf("frame %7.2f\n", (gframe/60.0f));
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

inline int8_t _djnRawInputDown(djnBtn btn, int8_t player, uint8_t frame)
{
	return (gDjnBtnHistory[frame] & (1 << int16_t(btn))) != 0;

}

int8_t djnInputBtnDown(djnBtn btn, int8_t player /*= 0*/)
{
	return _djnRawInputDown(btn, player, gDjnBtnFrame);
}


int8_t djnInputBtnPressed(djnBtn btn, int8_t player /*= 0*/)
{
	return _djnRawInputDown(btn, player, gDjnBtnFrame) && !_djnRawInputDown(btn, player, (gDjnBtnFrame + DJN_BTN_HISTORY_COUNT - 1) % DJN_BTN_HISTORY_COUNT);
}

djnMappingRef djnInputRegisterKeyboard(int16_t Keycode, int8_t player, djnBtn button)
{
	SDL_assert(Keycode >= 0 && Keycode <= SDL_NUM_SCANCODES);

	for (int i = 0; i < MAX_MAPPINGS; ++i)
	{
		if (KeyboardMappings[i].btn == djnBtn::NONE)
		{
			
			KeyboardMappings[i] = { player, int16_t(Keycode), button };
			return &KeyboardMappings[i];
		}
	}
	return nullptr;
}
