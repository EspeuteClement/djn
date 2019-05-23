#ifndef _DJN_INCLUDE_
#define _DJN_INCLUDE_

#include <cstdint>

#define GAME_WIDTH 320
#define GAME_HEIGHT 240

#define WINDOW_WIDTH (GAME_WIDTH*2)
#define WINDOW_HEIGHT (GAME_HEIGHT*2)

enum class djnBtn : int8_t
{
	NONE,
	A,
	B,
	X,
	Y,
	LEFT,
	UP,
	RIGHT,
	DOWN,
	START,
	SELECT,
	COUNT
};

typedef uint16_t djnPixel;

typedef struct djnMapping* djnMappingRef;

struct djnImage
{
	int w;
	int h;
	djnPixel* data;

	inline djnPixel& get(int x, int y) { return data[x + y * w]; };
};

extern djnImage gScreenBuffer;

struct djnConfig
{
	void (*UpdateFunction)	()	= nullptr;
	void (*DrawFunction)	()	= nullptr;
};

bool djnInit(djnConfig Config);

void djnRun();

void djnDeInit();

void djnBlit(djnImage& source, djnImage& target, int sx, int sy, int sw, int sh, int tx, int ty);

int8_t djnInputBtnDown(djnBtn btn, int8_t player = 0);

int8_t djnInputBtnPressed(djnBtn btn, int8_t player = 0);

// Maps the corresponding keyboard key to the given button.
// Returns a reference to the given mapping
djnMappingRef djnInputRegisterKeyboard(int16_t Keycode, int8_t player, djnBtn button);

#endif