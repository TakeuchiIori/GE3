#pragma once
// C++
#include <string>

// Engine
#include "ParticleManager.h"
#include "WorldTransform./WorldTransform.h"

// Math
#include  "Vector3.h"



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

private:

	/// <summary>
	/// パーティクルを発生
	/// </summary>
	void Emit();

	/// <summary>
	/// ImGui
	/// </summary>
	void ShowImGui();
public:

	void SetPosition(Vector3& position) { emitter_.transform = position; };
	void SetCount(uint32_t& setcount) { emitter_.count= setcount; };

private:

	/// <summary>
	/// エミッター構造体
	/// </summary>
	struct Emitter {
		std::string name; 
		Vector3 transform; 
		uint32_t count; 
		float frequency;
		float frequencyTime ;
	};

	// エミッター
	Emitter emitter_{};

	float deltaTime_ = 1.0f / 60.0f;

};

