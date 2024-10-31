#pragma once
#include "ParticleManager.h"
#include "WorldTransform.h"
#include "Vector3.h"
#include <string>


class ParticleEmitter
{
public: // メンバ関数


	/// <summary>
	/// 既定のコンストラクタ
	/// </summary>
	ParticleEmitter() = default;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleEmitter(const std::string& name, const Vector3& transform, uint32_t count);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


	/// <summary>
	/// パーティクルを発生
	/// </summary>
	void Emit();



	void SetPosition(Vector3& position) { emitter_.transform = position; };

private:

	/// <summary>
	/// エミッター構造体
	/// </summary>
	struct Emitter {
		std::string name; 
		Vector3 transform; 
		uint32_t count; 
		float frequency; 
		float frequencyTime; 
	};

	// エミッター
	Emitter emitter_{};

	float deltaTime_ = 1.0f / 60.0f;

};

