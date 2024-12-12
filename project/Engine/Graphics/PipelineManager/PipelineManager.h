#pragma once

// C++
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <unordered_map>



// CRS = Craete Root Signature
// CGP = Create Graphics Pipeline


class PipelineManager
{
public: 

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// ルートシグネチャをセット
	/// </summary>
	/// <param name="key"></param>
	void SetRootSignature(const std::string& key);

	/// <summary>
	/// パイプラインのセット
	/// </summary>
	/// <param name="key"></param>
	void SetPipeline(const std::string& key);

	/// <summary>
	/// ルートシグネチャの作成
	/// </summary>
	/// <param name="key"></param>
	void CreateRootSignature(const std::string& key);

	/// <summary>
	/// パイプラインの作成
	/// </summary>
	/// <param name="key"></param>
	void CreatePipelineState(const std::string& key);

private:

	//==================================================//
	/*                ルートシグネチャ作成			        */
	//==================================================//

	/// <summary>
	/// スプライト用のルートシグネチャ
	/// </summary>
	void CRS_Sprite();

	/// <summary>
	/// オブジェクト用のルートシグネチャ
	/// </summary>
	void CRS_Object();

	/// <summary>
	/// ライン用のルートシグネチャ
	/// </summary>
	void CRS_Line();

private:

	//==================================================//
	/*					パイプライン作成					*/
	//==================================================//


	/// <summary>
	/// スプライト用パイプライン
	/// </summary>
	void CGP_Sprite();

	/// <summary>
	/// オブジェクト用のパイプライン
	/// </summary>
	void CGP_Object();

	/// <summary>
	/// ライン用のパイプライン
	/// </summary>
	void CGP_Line();



private:

	

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelineStates_;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> rootSignatures_;

};

