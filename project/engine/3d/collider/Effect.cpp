#include "Effect.h"
#include "MathFunc.h"
#include "ModelManager.h"

void Effect::Initialize() {
	
	worldTransform_.Initialize();
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("float_body.obj");
}

void Effect::Update() {
	float t = (10.0f - timer_) * 0.1f;
	float scale = Lerp(1.0f, 3.0f, t);

	worldTransform_.scale_ = { scale * 2, scale * 2, scale * 2 };

	float alpha = 1.0f;

	alpha = 1.0f - Lerp(1.0f, 0.0f, timer_ / 10.0f);
	//obj_->SetAlpha(alpha);

	worldTransform_.UpdateMatrix();
	timer_--;
	if (timer_ < 0) {
		isAlive_ = false;
	}

}

void Effect::Draw() {
	obj_->Draw(worldTransform_);
}
