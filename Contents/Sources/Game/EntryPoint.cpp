
#include<windows.h>
#include"BaseSystem.h"

int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Prizm::BaseSystem game;
	if(game.Initialize()) game.Run();
	game.Finalize();
	return 0;
}