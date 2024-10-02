#pragma once

class DirectXCommon;
class SrvManager;
class ParticleManager {
public:
    // シングルトンインスタンスの取得
    static ParticleManager* GetInstance();

    // コピーコンストラクタと代入演算子を削除して、複製を防ぐ
        // コンストラクタをプライベートにして、外部からの直接生成を防ぐ
    ParticleManager() = default;
    ~ParticleManager() = default;
    ParticleManager(const ParticleManager&) = delete;
    ParticleManager& operator=(const ParticleManager&) = delete;

public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon,SrvManager* srvManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


private: // メンバ変数
	DirectXCommon* dxCommon_;
	SrvManager* srvManager_;

};
