#pragma once
#include "DirectXCommon.h"

// 3Dオブジェクト共通部
class DirectXCommon;
class Object3dCommon
{
public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 共通部描画設定
	/// </summary>
	void DrawPreference();

public: // アクセッサ
	DirectXCommon* GetDxCommon() const { return dxCommon_; }



private:
	/// <summary>
	/// ルートシグネチャの作成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// グラフィックスパイプラインの生成
	/// </summary>
	void CreateGraphicsPipeline();

	/// <summary>
	/// ルートシグネチャをセット
	/// </summary>
	void SetRootSignature();

	/// <summary>
	/// グラフィックスパイプラインをセット
	/// </summary>
	void SetGraphicsCommand();

	/// <summary>
	/// プリミティブトポロジーをセット
	/// </summary>
	void SetPrimitiveTopology();

private:
	DirectXCommon* dxCommon_;
};

