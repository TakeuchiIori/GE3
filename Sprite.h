#pragma once

class SpriteCommon;
class Sprite
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SpriteCommon* spriteCommon);


private: // メンバ関数

	SpriteCommon* spriteCommon_ = nullptr;

};

