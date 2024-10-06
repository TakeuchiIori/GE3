#include "MyGame.h"
#include "Framework.h"
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

//Windowsアプリのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Framework* game = new MyGame();

	game->Run();

	delete game;


	return 0;// main関数のリターン
}

