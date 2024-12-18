#pragma once

// C++
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <unordered_map>


class DirectXCommon;
class PipelineManager
{
public: 

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	//==================================================//
	/*				実際にパイプラインを作成					*/
	//==================================================//

	/// <summary>
	/// パイプラインのセット
	/// </summary>
	/// <param name="key"></param>
	void SetPipeline(const std::string& key);

	/// <summary>
	/// パイプラインの作成
	/// </summary>
	/// <param name="key"></param>
	void CreatePSO(const std::string& key);



private:

	//==================================================//
	/*				各パイプラインの作成					*/
	//==================================================//


	/// <summary>
	/// スプライト用パイプライン
	/// </summary>
	void CreatePSO_Sprite();

	/// <summary>
	/// オブジェクト用のパイプライン
	/// </summary>
	void CreatePSO_Object();

	/// <summary>
	/// ライン用のパイプライン
	/// </summary>
	void CreatePSO_Animation();

	/// <summary>
	/// ライン用のパイプライン
	/// </summary>
	void CreatePSO_Line();



private:

	DirectXCommon* dxCommon_ = nullptr;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelineStates_;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> rootSignatures_;

};

