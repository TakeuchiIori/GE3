#include "Input.h"

Input* Input::instance = nullptr;
Input* Input::GetInstance()
{
	if (instance == nullptr) {
		instance = new Input;
	}
	return instance;
}

void Input::Finalize()
{
	delete instance;
	instance = nullptr;
}

void Input::Initialize(WinApp* winApp)
{
	// 借りてきたwinAooのインスタンスを記録
	this->winApp_ = winApp;
	// DirectInputのインスタンス
	HRESULT result;
	result = DirectInput8Create(winApp_->Gethinstance(), DIRECTINPUT_HEADER_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	// キーボードデバイス生成

	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(winApp_->Gethwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	// マウスデバイス生成
	result = directInput->CreateDevice(GUID_SysMouse, &devMouse_, NULL);
	assert(SUCCEEDED(result));

	// マウスデータ形式のセット
	result = devMouse_->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = devMouse_->SetCooperativeLevel(winApp_->Gethwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));
}

void Input::Update(WinApp* winApp)
{
	HRESULT result;
	memcpy(keyPre, key, sizeof(key));
	// キーボード情報の取得開始
	result = keyboard->Acquire();
	result = keyboard->GetDeviceState(sizeof(key), key);

	// マウスの更新処理
	mousePre_ = mouse_; // 前回のマウス状態を保存
	result = devMouse_->Acquire();
	result = devMouse_->GetDeviceState(sizeof(DIMOUSESTATE2), &mouse_);

	// マウスの位置を更新する
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(winApp_->Gethwnd(), &point);
	mousePosition_.x = static_cast<float>(point.x);
	mousePosition_.y = static_cast<float>(point.y);
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

/// <summary>
/// マウスの押下をチェック
/// </summary>
/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
/// <returns>押されているか</returns>
bool Input::IsPressMouse(int32_t buttonNumber) const {
	if (buttonNumber < 0 || buttonNumber >= 8) {
		return false;
	}
	return (mouse_.rgbButtons[buttonNumber] & 0x80) != 0;
}

/// <summary>
/// マウスのトリガーをチェック。押した瞬間だけtrueになる
/// </summary>
/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
/// <returns>トリガーか</returns>
bool Input::IsTriggerMouse(int32_t buttonNumber) const {
	if (buttonNumber < 0 || buttonNumber >= 8) {
		return false;
	}
	return ((mouse_.rgbButtons[buttonNumber] & 0x80) != 0) && ((mousePre_.rgbButtons[buttonNumber] & 0x80) == 0);
}

/// <summary>
/// 全マウス情報取得
/// </summary>
/// <returns>マウス情報</returns>
const DIMOUSESTATE2& Input::GetAllMouse() const {
	return mouse_;
}

/// <summary>
/// マウス移動量を取得
/// </summary>
/// <returns>マウス移動量</returns>
Input::MouseMove Input::GetMouseMove() {
	MouseMove move = {};
	move.lX = mouse_.lX;
	move.lY = mouse_.lY;
	move.lZ = mouse_.lZ;
	return move;
}

/// <summary>
/// ホイールスクロール量を取得する
/// </summary>
/// <returns>ホイールスクロール量。奥側に回したら+。Windowsの設定で逆にしてたら逆</returns>
int32_t Input::GetWheel() const {
	return mouse_.lZ;
}

/// <summary>
/// マウスの位置を取得する（ウィンドウ座標系）
/// </summary>
/// <returns>マウスの位置</returns>
const Vector2& Input::GetMousePosition() const {
	return mousePosition_;
}
