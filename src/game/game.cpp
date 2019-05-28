#include <cstring>

#include "game.h"

#include "data.generated.h"
#include "djn/djn.h"
#include "cstdio"
#include <math.h>

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
	Vec2f Pos;
	Vec2f Vel;
	Vec2f Acc;

	uint8_t SprId;

	uint8_t PlayerId; // 0 = neutral, 1 = p1, 2 = p2
};

#define MAX_OBJECT 8

ksrObject Obj[MAX_OBJECT];

int x = 0;
int numberOfIter = -1;
int SelectedId = 0;
#define MAX_ITERS 60

#define RADIUS 8.0f

void UpdateWaitForPlayer()
{
	int deltaX = djnInputBtnPressed(djnBtn::RIGHT) - djnInputBtnPressed(djnBtn::LEFT);
	if (deltaX != 0)
	{
		for (int i = 0; i < MAX_OBJECT; ++i)
		{
			SelectedId = (SelectedId + MAX_OBJECT + deltaX) % MAX_OBJECT;
			if (Obj[SelectedId].Pos.x >= 0) break;
		}
	}

	int deltaY = djnInputBtnDown(djnBtn::DOWN) - djnInputBtnDown(djnBtn::UP);
	if (deltaY)
	{
		float x = Obj[SelectedId].Vel.x;
		float y = Obj[SelectedId].Vel.y;
		double angle = 0.1 * float(deltaY);
		float cs = cos(angle);
		float sn = sin(angle);

		Obj[SelectedId].Vel.x = x * cs - y * sn;
		Obj[SelectedId].Vel.y = x * sn + y * cs;
	}

	if (djnInputBtnPressed(djnBtn::A, 0))
	{
		gKsrGameState = GameState::END_TURN;
	}
}

void UpdateEndTurn()
{
	if (numberOfIter == -1)
		numberOfIter = MAX_ITERS;

	numberOfIter--;
	const float DeltaTime = 0.016;


	// Move each object
	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		ksrObject& o = Obj[i];
		if (o.Pos.x >= 0)
		{
			Vec2f NewAcc = o.Acc;

			// Drag
			//o.Acc = -o.Speed * 0.80f;

			o.Pos = 0.5 * o.Acc * DeltaTime * DeltaTime + o.Vel * DeltaTime + o.Pos;
			o.Vel = o.Vel + (o.Acc + NewAcc) / 2.0f * DeltaTime;

			o.Acc = NewAcc;
		}
	}

#define MAX_COLLISIONS 1
	ksrObject* Collisions[MAX_COLLISIONS][2];
	int CollisionPos = 0;

	// Resolve collision
	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		ksrObject& object = Obj[i];
		if (object.Pos.x >= 0.0f)
		{
			for (int j = 0; j < MAX_OBJECT; ++j)
			{
				if (i != j)
				{
					ksrObject& target = Obj[j];

					if (target.Pos.x >= 0.0f)
					{
						float distSqr = object.Pos.LengthSqr(target.Pos);
						if (distSqr <= (2 * RADIUS) * (2 * RADIUS))
						{
							if (CollisionPos < MAX_COLLISIONS)
							{
								Collisions[CollisionPos][0] = &object;
								Collisions[CollisionPos][1] = &target;
								CollisionPos++;
							}

							float Dist = sqrt(distSqr);
							float Overlap = 0.5f * (Dist - RADIUS - RADIUS);

							Vec2f MovVec = Overlap * (object.Pos - target.Pos) / Dist;

							object.Pos -= MovVec;
							target.Pos += MovVec;
							
						}
					}
					else
					{
						break;
					}


				}
			}
		}
		else
		{
			break;
		}
	}

	for (int i = 0; i < CollisionPos; ++i)
	{
		ksrObject& o1 = *Collisions[i][0];
		ksrObject& o2 = *Collisions[i][1];

		float dist = o1.Pos.Length(o2.Pos);

		Vec2f Normal = (o2.Pos - o1.Pos) / dist;
		Vec2f Tangent(-Normal.y, Normal.x);

		float dpTan1 = Tangent.Dot(o1.Vel);
		float dpTan2 = Tangent.Dot(o2.Vel);

		float dpNorm1 = Normal.Dot(o1.Vel);
		float dpNorm2 = Normal.Dot(o2.Vel);

		float m1 = dpNorm2;
		float m2 = dpNorm1;

		o1.Vel = Tangent * dpTan1 + Normal * m1;
		o2.Vel = Tangent * dpTan2 + Normal * m2;
	}

	if (numberOfIter <= 0)
	{
		numberOfIter = -1;
		printf("EndTurn\n");
		gKsrGameState = GameState::WAIT_FOR_PLAYER;
	}

}


void Update()
{
	if (djnInputBtnPressed(djnBtn::START))
	{
		Init();
		return;
	}

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
		if (o.Pos.x >= 0)
		{
			djnBlit(gGameSprites, gScreenBuffer, o.SprId * 16, 0, 16, 16, int(o.Pos.x-RADIUS), int(o.Pos.y - RADIUS));
			djnLine(gScreenBuffer, int(o.Pos.x), int(o.Pos.y), int(o.Pos.x + o.Vel.x), int(o.Pos.y + o.Vel.y), 0xFF);

			if (gKsrGameState == GameState::WAIT_FOR_PLAYER && i == SelectedId)
			{
				djnBlit(gGameSprites, gScreenBuffer, 5 * 16, 0, 16, 16, int(o.Pos.x - RADIUS), int(o.Pos.y - RADIUS));
			}
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
		Obj[i] = { Vec2f(-1.0f,-1.0f)};
	}

	Obj[0] = { Vec2f(8.0f,5.0f) * 16, Vec2f(90.0f,5.0f), Vec2f(0,0), 2, 1};
	Obj[1] = { Vec2f(8.0f,10.0f) * 16, Vec2f(0.0f,0.0f), Vec2f(0,0), 2, 1 };

	Obj[2] = { Vec2f(12.0f,5.0f) * 16, Vec2f(0.0f,0.0f), Vec2f(0,0), 3, 2};
	Obj[3] = { Vec2f(12.0f,8.0f) * 16, Vec2f(-60.0f,-90.0f), Vec2f(0,0), 3 , 2};


}