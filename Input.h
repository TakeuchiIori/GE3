#pragma once
#include <Windows.h>
#include<wrl.h>
#define DIRECTINPUT_VERSION		0x0800
#include "dinput.h"
#include "assert.h"
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace Microsoft::WRL;
/// <summary>
/// 入力
/// </summary>
class Input
{
public:
	// 初期化
	void Initialize(HINSTANCE hinstance,HWND hwnd);
	// 更新
	void Update();

private:

	
};

