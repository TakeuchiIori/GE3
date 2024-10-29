#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(const std::string& name, const Vector3& transform, uint32_t count)
    : emitter_{ name, Transform{transform}, count }{}


void ParticleEmitter::Update()
{
	emitter_.frequencyTime += deltaTime_;
	if (emitter_.frequency <= emitter_.frequencyTime) {
		// パーティクルを生成してグループに追加
		Emit();
		emitter_.frequencyTime -= emitter_.frequency;
	}

}

void ParticleEmitter::Emit()
{
	ParticleManager::GetInstance()->Emit(emitter_.name, emitter_.transform.translate, emitter_.count);
}
