#ifndef _DJN_INCLUDE_
#define _DJN_INCLUDE_

#include <cstdint>

#define GAME_WIDTH 320
#define GAME_HEIGHT 240

#define WINDOW_WIDTH (GAME_WIDTH*2)
#define WINDOW_HEIGHT (GAME_HEIGHT*2)

typedef uint16_t djnPixel;

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

#endif