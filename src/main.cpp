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

#include "game/game.h"

int main(int argc, char *argv[]) {

	djnConfig conf;
	conf.UpdateFunction = &Update;
	conf.DrawFunction = &Draw;

	bool success = djnInit(conf);
	
	if (success)
	{
		Init();
		djnRun();
	}

	djnDeInit();

	return 0;
} //main