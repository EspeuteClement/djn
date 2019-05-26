#include <cstring>

#include "data.generated.h"
#include "djn/djn.h"

#define KSR_MAP_WIDTH 20
#define KSR_MAP_HEIGHT 15

static uint8_t ksrMAP[KSR_MAP_HEIGHT][KSR_MAP_WIDTH]
=
{
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
{0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

namespace Direction
{
	enum : uint8_t
	{
		NONE,
		EAST,
		NORTH,
		WEST,
		SOUTH,
		NORTH_EAST = EAST | NORTH,
		NORTH_WEAST = WEST | NORTH,
		SOUTH_EAST = EAST | SOUTH,
		SOUTH_WEST = WEST | SOUTH
	};
}

namespace GameState
{
	enum : uint8_t
	{
		WAIT_FOR_PLAYER,
		END_TURN,
	};
}

static int gKsrGameState = GameState::WAIT_FOR_PLAYER;

struct Object
{
	int16_t x;
	int16_t y;

	uint8_t direction;

	uint8_t SprId;
};

#define MAX_OBJECT 8

Object Obj[MAX_OBJECT];

int x = 0;
void Update()
{
	switch (gKsrGameState)
	{
	case GameState::WAIT_FOR_PLAYER:

		break;
	case GameState::END_TURN

		break;
	default:
		break;
	}
}


void DrawTilemap()
{
	for (int y = 0; y < KSR_MAP_HEIGHT; ++y)
		for (int x = 0; x < KSR_MAP_WIDTH; ++x)
			djnBlit(gGameSprites, gScreenBuffer, 16 * ksrMAP[y][x], 0, 16, 16, x * 16, y * 16);
}

void DrawObjects()
{
	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		Object& o = Obj[i];
		if (o.x >= 0)
		{
			djnBlit(gGameSprites, gScreenBuffer, o.SprId * 16, 0, 16, 16, o.x, o.y);
		}
	}
}

void Draw() {
	memset(gScreenBuffer.data, 0x8F, GAME_WIDTH * GAME_HEIGHT * 2);

	DrawTilemap();
	
	DrawObjects();

} //render

void Init()
{
	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		Obj[i] = { -1,-1 };
	}

	Obj[0] = { 8 * 16, 5 * 16, Direction::NONE, 2 };
}