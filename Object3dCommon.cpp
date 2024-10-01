#include "Object3dCommon.h"

void Object3dCommon::Initialize(DirectXCommon* dxCommon)
{
	// 引数で受け取ってメンバ変数に記録する
	dxCommon_ = dxCommon;

	// パイプラインの生成
	CreateGraphicsPipeline();
}

void Object3dCommon::CreateRootSignature()
{

}

void Object3dCommon::CreateGraphicsPipeline()
{

}

void Object3dCommon::DrawPreference()
{
	// ルートシグネチャをセット
	SetRootSignature();

	// パイプラインをセット
	SetGraphicsCommand();

	// プリミティブトポロジーをセット
	SetPrimitiveTopology();
}

void Object3dCommon::SetRootSignature()
{
}

void Object3dCommon::SetGraphicsCommand()
{
}

void Object3dCommon::SetPrimitiveTopology()
{
}
