#include "Ground.h"

void Ground::Initialize()
{
	// Object3dの生成
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("Ground.obj");

	worldTransform_.Initialize();
}

void Ground::Update()
{
	worldTransform_.UpdateMatrix();
}

void Ground::Draw()
{
	obj_->Draw(worldTransform_);
}
