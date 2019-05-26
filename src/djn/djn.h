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
	uint16_t w;
	uint16_t h;
	djnPixel* data;

	inline djnPixel& get(int x, int y) { return data[x + y * w]; };
};

struct djnTile
{
	djnImage* Image;
	uint16_t ox;
	uint16_t oy;
	uint16_t tw;
	uint16_t th;
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

enum djnBlitFlag : uint8_t
{
	NONE = 0,
	FLIP_X = 1,
	FLIP_Y = 2,
	FLIP_ALL = FLIP_X | FLIP_Y
};

void djnBlit(djnImage& source, djnImage& target, uint16_t sx, uint16_t sy, uint16_t sw, uint16_t sh, uint16_t tx, uint16_t ty, djnBlitFlag flags = djnBlitFlag::NONE);

void djnBlit(djnTile& source, djnImage& target, uint16_t tx, uint16_t ty, djnBlitFlag flags = djnBlitFlag::NONE);


int8_t djnInputBtnDown(djnBtn btn, int8_t player = 0);

int8_t djnInputBtnPressed(djnBtn btn, int8_t player = 0);

// Maps the corresponding keyboard key to the given button.
// Returns a reference to the given mapping
djnMappingRef djnInputRegisterKeyboard(int16_t Keycode, int8_t player, djnBtn button);

#endif