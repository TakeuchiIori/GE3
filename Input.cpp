#include "Input.h"

void Input::Initialize(HINSTANCE hinstance, HWND hwnd)
{
	// DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput = nullptr;
	HRESULT result;
	result = DirectInput8Create(hinstance, DIRECTINPUT_HEADER_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	// キーボードデバイス生成
	ComPtr<IDirectInputDevice8> keyboard;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update()
{
	//keyboard->Acquire();
}
