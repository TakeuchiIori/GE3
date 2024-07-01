#include "Input.h"

void Input::Initialize(HINSTANCE hinstance, HWND hwnd)
{
	// DirectInputのインスタンス

	HRESULT result;
	result = DirectInput8Create(hinstance, DIRECTINPUT_HEADER_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	// キーボードデバイス生成

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
	HRESULT result;
	memcpy(keyPre, key, sizeof(key));
	// キーボード情報の取得開始
	result = keyboard->Acquire();
	result = keyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE keyNumber)
{
	// 指定したキーを押していればtrueを返す
	if (key[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	if (key[keyNumber] && !keyPre[keyNumber]) {
		return true;
	}
	return false;
}
