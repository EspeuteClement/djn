#include "djn/djn.h"
#include <cstring>
/*
Temporary Styleguide :

Functions	: CamelCase
Globals		: gCamelCase
Constants	: FULL_CAPS
Locals		: camelCaseWithNoCapsAtTheStart
Members		: mCamelCase (unless 1 letter long)
Private Members : _CamelCase

Indent with tabs
*/

#include "data.generated.h"

int x = 0;
void Update()
{
	x += 1;
	x = x % 304;
}

void Draw() {
	memset(gScreenBuffer.data, 0x8F, GAME_WIDTH * GAME_HEIGHT * 2);
	djnBlit(gGameSprites, gScreenBuffer, 16, 0, 16,16, x, 0);
} //render

int main(int argc, char *argv[]) {

	djnConfig conf;
	conf.UpdateFunction = &Update;
	conf.DrawFunction = &Draw;

	bool success = djnInit(conf);
	
	if (success)
	{
		djnRun();
	}

	djnDeInit();

	return 0;
} //main