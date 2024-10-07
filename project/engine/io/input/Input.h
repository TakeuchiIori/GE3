#pragma once
#include <Windows.h>
#include<wrl.h>
#include "WinApp.h"
#define DIRECTINPUT_VERSION		0x0800
#include "dinput.h"
#include "assert.h"
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
/// <summary>
/// 入力
/// </summary>
class Input
{
public:

	static Input* GetInstance();
	void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(WinApp* winApp);

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号( DIK_0 等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber);

private:
	static Input* instance;
	Input() = default;
	~Input() = default;
	Input(Input&) = delete;
	Input& operator = (Input&) = delete;
	ComPtr<IDirectInput8> directInput;
	ComPtr<IDirectInputDevice8> keyboard;
	// 全キーの入力情報を取得する
	BYTE key[256] = {};
	// 前回のキーの状態
	BYTE keyPre[256] = {};
	// WindowsAPI
	WinApp* winApp_ = nullptr;
};

