#include "ParticleEmitter.h"
#include "imgui.h"
ParticleEmitter::ParticleEmitter(const std::string& name, const Vector3& transform, uint32_t count)
    : emitter_{ name, Vector3{transform}, count,2.0f,0.0f }{}

void ParticleEmitter::Update()
{
	emitter_.frequencyTime += deltaTime_;
	if (emitter_.frequency <= emitter_.frequencyTime) {
		// パーティクルを生成してグループに追加
		Emit();
		emitter_.frequencyTime -= emitter_.frequency;
	}

	ShowImGui();
}

void ParticleEmitter::Emit()
{
	ParticleManager::GetInstance()->Emit(emitter_.name, emitter_.transform, emitter_.count);
}

void ParticleEmitter::ShowImGui()
{
#ifdef _DEBUG
	ImGui::Begin("Particle");

	if(ImGui::Button("Add Particle")) {
		Emit();
	}
	ImGui::End();
#endif // _DEBUG

}
