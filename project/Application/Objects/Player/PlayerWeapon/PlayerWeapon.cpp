#include "PlayerWeapon.h"
// Engine
#include "Loaders./Model./ModelManager.h"
#include "Object3D./Object3dCommon.h"

#ifdef _DEBUG
#include "imgui.h" 
#endif // _DEBUG

void PlayerWeapon::Initialize()
{
	// object3dの初期化
	weapon_ = std::make_unique<Object3d>();
	weapon_->Initialize();
	weapon_->SetModel("cube.obj");

	worldTransform_.Initialize();
	worldTransform_.translation_.z = -2.0f;

}

void PlayerWeapon::Update()
{
	worldTransform_.UpdateMatrix();
}

void PlayerWeapon::Draw()
{
	weapon_->Draw(worldTransform_);
}
