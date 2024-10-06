#include "MyGame.h"

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

//Windowsアプリのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	MyGame game;
	game.Initialize();

	// ウィンドウのxボタンが押されるまでループ
	while (true)
	{
		game.Update();
		if (game.isEndRequst()) {
			break;
		}



		game.Draw();



	}
	game.Finalize();


	return 0;// main関数のリターン
}

