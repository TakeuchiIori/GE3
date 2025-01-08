#pragma once

// Engine
#include "Enemy.h"

// C++
#include <list>


class EnemeyManager
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


private:



private:

	// ポインタ
	std::vector<std::unique_ptr<Enemy>> enemies_;

	// 敵の数
	const uint32_t enemyCount_ = 10; 

};

