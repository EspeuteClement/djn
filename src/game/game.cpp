#include <cstring>

#include "data.generated.h"
#include "djn/djn.h"
#include "cstdio"

#define KSR_MAP_WIDTH 20
#define KSR_MAP_HEIGHT 15

#define KSR_CELL_SIZE 16

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

struct ArrowData
{
	uint8_t spr_id;
	djnBlitFlag BlitFlags;
} DirToArrow[] =
{
	{4, djnBlitFlag::FLIP_X},  
	{5, djnBlitFlag::NONE},	  
	{4, djnBlitFlag::NONE},		
	{3, djnBlitFlag::FLIP_X}, // <-
	{0, djnBlitFlag::NONE}, // none
	{3, djnBlitFlag::NONE}, // ->
	{4, djnBlitFlag::FLIP_ALL},
	{5, djnBlitFlag::FLIP_Y},
	{4, djnBlitFlag::FLIP_Y},
};

inline ArrowData VecToArrow(Vec2& vec)
{
	return DirToArrow[(1+vec.x) + (1+vec.y) * 3];
}

namespace Direction
{
	enum : uint8_t
	{
		NONE = 0,
		EAST = 1,
		NORTH = 2,
		WEST = 4,
		SOUTH = 8,
		NORTH_EAST = EAST | NORTH,
		NORTH_WEAST = WEST | NORTH,
		SOUTH_EAST = EAST | SOUTH,
		SOUTH_WEST = WEST | SOUTH
	};
}

inline Vec2 DirToVec(uint8_t dir)
{
	return {
				((dir & Direction::EAST) != 0) - ((dir & Direction::WEST) != 0),
				((dir & Direction::SOUTH) != 0) - ((dir & Direction::NORTH) != 0)
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

struct ksrObject
{
	int16_t x;
	int16_t y;

	uint8_t direction;

	uint8_t SprId;
};

#define MAX_OBJECT 8

ksrObject Obj[MAX_OBJECT];

int x = 0;

void UpdateWaitForPlayer()
{
	if (djnInputBtnPressed(djnBtn::A, 0))
	{
		gKsrGameState = GameState::END_TURN;
	}
}

void UpdateEndTurn()
{

	// Move each object
	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		ksrObject& o = Obj[i];
		if (o.x >= 0)
		{
			Vec2 v = DirToVec(o.direction);

			Vec2 NewDir()

			o.x += v.x * KSR_CELL_SIZE;
			o.y += v.y * KSR_CELL_SIZE;
		}
	}

	printf("EndTurn\n");
	gKsrGameState = GameState::WAIT_FOR_PLAYER;
}


void Update()
{
	switch (gKsrGameState)
	{
	case GameState::WAIT_FOR_PLAYER:
		UpdateWaitForPlayer();
		break;
	case GameState::END_TURN:
		UpdateEndTurn();
		break;
	default:
		break;
	}
}


void DrawTilemap()
{
	for (int y = 0; y < KSR_MAP_HEIGHT; ++y)
		for (int x = 0; x < KSR_MAP_WIDTH; ++x)
			djnBlit(gGameSprites, gScreenBuffer, 16 * ksrMAP[y][x], 0, 16, 16, x * KSR_CELL_SIZE, y * KSR_CELL_SIZE);
}

void DrawObjects()
{
	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		ksrObject& o = Obj[i];
		if (o.x >= 0)
		{
			djnBlit(gGameSprites, gScreenBuffer, o.SprId * 16, 0, 16, 16, o.x, o.y);
			Vec2 v = DirToVec(o.direction);
			ArrowData arr = VecToArrow(v);
			djnBlit(gGameSprites, gScreenBuffer, arr.spr_id * 16, 0, 16, 16, o.x + v.x * KSR_CELL_SIZE, o.y + v.y * KSR_CELL_SIZE, arr.BlitFlags);
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
		Obj[i] = { -1,-1};
	}

	Obj[0] = { 8 * 16, 5 * 16, Direction::NONE, 2 };
	Obj[0].direction = Direction::EAST;

	Obj[1] = { 12 * 16, 5 * 16, Direction::NONE, 2 };
	Obj[1].direction = Direction::WEST;
}