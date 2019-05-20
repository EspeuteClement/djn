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



void Update()
{
	
}

void Draw() {
	memset(gScreenBuffer, 0x8F, GAME_WIDTH * GAME_HEIGHT * 2);
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