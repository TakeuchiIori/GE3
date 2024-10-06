#pragma once

// ゲーム全体
class Framework
{
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 終了フラグのチェック
	/// </summary>
	/// <returns></returns>
	virtual bool isEndRequst() { return endRequst_; }

	// 呼び出さないとリークするぞ
	virtual ~Framework() = default;

private:
	// ゲーム終了フラグ
	bool endRequst_ = false;

};

