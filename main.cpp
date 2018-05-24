#include "Game/Game.h"

int WINAPI WinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nShowCmd)
{
	th5w::CGame game;
	game.Run();
	return 0;
}
