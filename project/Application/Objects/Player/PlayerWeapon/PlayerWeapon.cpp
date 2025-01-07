#include "PlayerWeapon.h"
// Engine
#include "Loaders./Model./ModelManager.h"
#include "Object3D./Object3dCommon.h"
#include "Collision/GlobalVariables.h"



// C++
#include <fstream>
#include <iostream>


// ImGui
#ifdef _DEBUG
#include "imgui.h" 
#endif // _DEBUG



/// <summary>
/// 初期化
/// </summary>
void PlayerWeapon::Initialize()
{
	// object3dの初期化
	weapon_ = std::make_unique<Object3d>();
	weapon_->Initialize();
	weapon_->SetModel("weapon.obj");

	// インプット
	input_ = Input::GetInstance();

	worldTransform_.Initialize();
	worldTransform_.translation_.y = 0.0f;
	worldTransform_.translation_.z = -2.0f;

#pragma region 各モーションの初期化
	// 通常攻撃モーション
	attackMotions_.push_back({
		0.5f, 0.2f, 0.8f, {
			{0.0f, {0, 4, 0}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({1, 0, 0}, 0)},   // 初期状態
			{0.25f, {0, 2, 2}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({1, 0, 0}, 45)}, // 振り下ろし途中
			{0.5f, {0, 0, 2}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({1, 0, 0}, 90)} // 完了
		}
		});
	// ダッシュ攻撃モーション
	attackMotions_.push_back({
		0.5f, 0.2f, 0.8f, {
			{0.0f, {-4.0f, 6.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({90, 0, 45})},   // スタート
			{0.25f, {0.0f, 0.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({90, 0, 45})},   // 中央
			{0.5f, {4.0f, -6.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({90, 0, 45})}    // フィニッシュ
		}
		});

	attackMotions_.push_back({
	0.5f, 0.2f, 0.8f, {
		{0.0f, {4.0f, 6.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({90, 0, -45})},   // スタート
		{0.25f, {0.0f, 0.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({90, 0, -45})},   // 中央
		{0.5f, {-4.0f, -6.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({90, 0, -45})}    // フィニッシュ
		}
		});

	// ジャンプ
	attackMotions_.push_back({
		1.0f, 0.2f, 0.8f, {
		{0.0f, {0.0f, 0.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({90 + 360 * 0.0f , 0, 0})},   // スタート
		{0.5f, {0.0f, 0.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({90 + 360 * 0.5f, 0, 0})},   // 中央
		{1.0f, {0.0f, 0.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({90 + 360 * 1.0f, 0, 0})}    // フィニッシュ
		}
		});

	attackMotions_.push_back({
		0.5f, 0.2f, 0.8f, {
		{0.0f, {0.0f, 6.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({90 + 360 * 0.0f , 0, 0})},   // スタート
		{0.5f, {0.0f, 3.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({90 + 360 * 0.5f, 0, 0})},   // 中央
		{1.0f, {0.0f, 0.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, MakeRotateAxisAngleQuaternion({90 + 360 * 1.0f, 0, 0})}    // フィニッシュ
		}
		});


#pragma endregion


	// グループを追加
	Collider::Initialize();
	//
	SaveGlobalVariables();
	// TypeIDの設定
	Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayerWeapon));
}

void PlayerWeapon::SaveGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "PlayerWeapon";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	// 武器の位置・回転・スケール
	globalVariables->AddItem(groupName, "Translation", worldTransform_.translation_);
	globalVariables->AddItem(groupName, "Rotation", worldTransform_.rotation_);
	globalVariables->AddItem(groupName, "Scale", worldTransform_.scale_);

	// クールダウン時間
	globalVariables->AddItem(groupName, "CooldownTime", cooldownTime_);

	// コンボ猶予時間
	globalVariables->AddItem(groupName, "ComboWindow", comboWindow_);

	// 各モーションのパラメータ
	for (size_t i = 0; i < attackMotions_.size(); ++i) {
		std::string motionName = "AttackMotion_" + std::to_string(i);

		globalVariables->AddItem(groupName, motionName + "_Duration", attackMotions_[i].duration);
		globalVariables->AddItem(groupName, motionName + "_HitStartTime", attackMotions_[i].hitStartTime);
		globalVariables->AddItem(groupName, motionName + "_HitEndTime", attackMotions_[i].hitEndTime);

		for (size_t j = 0; j < attackMotions_[i].srtKeyframes.size(); ++j) {
			std::string keyframeName = motionName + "_Keyframe_" + std::to_string(j);

			globalVariables->AddItem(groupName, keyframeName + "_Time", attackMotions_[i].srtKeyframes[j].time);
			globalVariables->AddItem(groupName, keyframeName + "_Position", attackMotions_[i].srtKeyframes[j].position);
			globalVariables->AddItem(groupName, keyframeName + "_Scale", attackMotions_[i].srtKeyframes[j].scale);
			globalVariables->AddItem(groupName, keyframeName + "_Rotation", attackMotions_[i].srtKeyframes[j].rotation);
		}
	}

	// 保存
	//globalVariables->SaveFile(groupName);
}


/// <summary>
/// 更新
/// </summary>
void PlayerWeapon::Update()
{
	// 全状態の初期化
	InitializeState();

	// 全状態の更新処理
	UpdateState();

	if (state_ != PlayerWeapon::WeaponState::Idle && 
		state_ != PlayerWeapon::WeaponState::Cooldown) {
		Collider::SetRadiusFloat(2.0f);
	}

	//ApplyGlobalVariables();

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();

#ifdef _DEBUG
	DrawDebugUI();
#endif // _DEBUG

}

/// <summary>
/// 描画
/// </summary>
void PlayerWeapon::Draw()
{
	weapon_->Draw(worldTransform_);
}

/// <summary>
/// ImGui
/// </summary>
void PlayerWeapon::DrawDebugUI() {
#ifdef _DEBUG
	if (ImGui::Begin("Weapon Debug")) {
		// 現在の状態
		ImGui::Text("Current State: %s",
			state_ == WeaponState::Idle ? "Idle" :
			state_ == WeaponState::Attacking ? "Attacking" :
			state_ == WeaponState::Dashing ? "Dashing" : "Cooldown");

		// クールダウンの時間
		ImGui::SliderFloat("Cooldown Time", &cooldownTime_, 0.1f, 5.0f, "%.2f");

		// 経過クールダウン時間
		ImGui::Text("Elapsed Cooldown Time: %.2f", elapsedCooldownTime_);

		// 攻撃モーションの進行度
		ImGui::Text("Attack Progress: %.2f", attackProgress_);


		// デバッグ用の状態遷移ボタン
		if (ImGui::Button("Set to Idle")) {
			stateRequest_ = WeaponState::Idle;
		}
		if (ImGui::Button("Set to Attacking")) {
			stateRequest_ = WeaponState::Attacking;
		}
		if (ImGui::Button("Set to Left Swing")) {
			stateRequest_ = WeaponState::LSwing;
		}
		if (ImGui::Button("Set to Right Swing")) {
			stateRequest_ = WeaponState::RSwing;
		}
		if (ImGui::Button("Set to Jump Attacking")) {
			stateRequest_ = WeaponState::JumpAttack;
		}
		if (ImGui::Button("Set to Dashing")) {
			stateRequest_ = WeaponState::Dashing;
		}
		if (ImGui::Button("Set to Cooldown")) {
			stateRequest_ = WeaponState::Cooldown;
		}
	}

	// SRTの変更
	ImGui::SliderFloat3("Position", &worldTransform_.translation_.x, 0.0f, 100.0f, "%.2f");
	ImGui::SliderFloat3("Rotation", &worldTransform_.rotation_.x, 0.0f, 360.0f, "%.2f");
	ImGui::SliderFloat3("Scale", &worldTransform_.scale_.x, 0.0f, 100.0f, "%.2f");

	// 各モーションのパラメータ調整
	for (size_t i = 0; i < attackMotions_.size(); ++i) {
		ImGui::PushID(i);
		if (ImGui::CollapsingHeader(("Motion " + std::to_string(i)).c_str())) {
			ImGui::Text("Motion Parameters:");
			ImGui::SliderFloat("Duration", &attackMotions_[i].duration, 0.1f, 50.0f, "%.2f");
			ImGui::SliderFloat("Hit Start Time", &attackMotions_[i].hitStartTime, 0.0f, attackMotions_[i].duration, "%.2f");
			ImGui::SliderFloat("Hit End Time", &attackMotions_[i].hitEndTime, attackMotions_[i].hitStartTime, attackMotions_[i].duration);
			if (i == 3) {
				ImGui::Checkbox("Jump Frag", &isJumpAttack_);
			}
			// キーフレームの調整
			for (size_t j = 0; j < attackMotions_[i].srtKeyframes.size(); ++j) {
				std::string label;
				if (j == 0) {
					label = "Start Position";
				}
				else if (j == attackMotions_[i].srtKeyframes.size() - 1) {
					label = "End Position";
				}
				else {
					label = "Mid Position " + std::to_string(j);
				}

				if (ImGui::TreeNode((label + " (Keyframe " + std::to_string(j) + ")").c_str())) {
					ImGui::Text("%s", label.c_str());
					ImGui::SliderFloat("Time", &attackMotions_[i].srtKeyframes[j].time, 0.0f, attackMotions_[i].duration, "%.2f");
					ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&attackMotions_[i].srtKeyframes[j].position), -10.0f, 10.0f);
					ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&attackMotions_[i].srtKeyframes[j].scale), 0.1f, 5.0f);
					ImGui::SliderFloat3("Rotation", reinterpret_cast<float*>(&attackMotions_[i].srtKeyframes[j].rotation), -360.0f, 360.0f);
					ImGui::TreePop();
				}
			}
		}
		ImGui::PopID();
	}

	// 保存と読み込み
	if (ImGui::Button("Save Settings")) {
		SaveToFile("Resources./Json./weapon_settings.json");
	}

	if (ImGui::Button("Load Settings")) {
		LoadFromFile("Resources./Json./weapon_settings.json");
	}
	ImGui::End();
#endif

}

json PlayerWeapon::ToJson() const {
	json motionData = json::array();
	for (const auto& motion : attackMotions_) {
		json keyframes = json::array();
		for (const auto& keyframe : motion.srtKeyframes) {
			keyframes.push_back({
				{"time", keyframe.time},
				{"position", {keyframe.position.x, keyframe.position.y, keyframe.position.z}},
				{"scale", {keyframe.scale.x, keyframe.scale.y, keyframe.scale.z}},
				{"rotation", {keyframe.rotation.x, keyframe.rotation.y, keyframe.rotation.z}}
				});
		}
		motionData.push_back({
			{"duration", motion.duration},
			{"hitStartTime", motion.hitStartTime},
			{"hitEndTime", motion.hitEndTime},
			{"keyframes", keyframes}
			});
	}

	return {
		{"currentState", static_cast<int>(state_)},
		{"worldTransform", {
			{"translation", {worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z}},
			{"rotation", {worldTransform_.rotation_.x, worldTransform_.rotation_.y, worldTransform_.rotation_.z}},
			{"scale", {worldTransform_.scale_.x, worldTransform_.scale_.y, worldTransform_.scale_.z}}
		}},
		{"cooldownTime", cooldownTime_},
		{"elapsedCooldownTime", elapsedCooldownTime_},
		{"attackMotions", motionData},
		{"canCombo", canCombo_},
		{"elapsedComboTime", elapsedComboTime_},
		{"comboWindow", comboWindow_},
		{"idleTime", idleTime_},
		{"attackProgress", attackProgress_},
		{"isJumpAttack", isJumpAttack_}
	};
}

void PlayerWeapon::FromJson(const json& data)
{
	state_ = static_cast<WeaponState>(data["currentState"].get<int>());

	worldTransform_.translation_ = {
		data["worldTransform"]["translation"][0].get<float>(),
		data["worldTransform"]["translation"][1].get<float>(),
		data["worldTransform"]["translation"][2].get<float>()
	};
	worldTransform_.rotation_ = {
		data["worldTransform"]["rotation"][0].get<float>(),
		data["worldTransform"]["rotation"][1].get<float>(),
		data["worldTransform"]["rotation"][2].get<float>()
	};
	worldTransform_.scale_ = {
		data["worldTransform"]["scale"][0].get<float>(),
		data["worldTransform"]["scale"][1].get<float>(),
		data["worldTransform"]["scale"][2].get<float>()
	};

	cooldownTime_ = data["cooldownTime"].get<float>();
	elapsedCooldownTime_ = data["elapsedCooldownTime"].get<float>();
	canCombo_ = data["canCombo"].get<bool>();
	elapsedComboTime_ = data["elapsedComboTime"].get<float>();
	comboWindow_ = data["comboWindow"].get<float>();
	idleTime_ = data["idleTime"].get<float>();
	attackProgress_ = data["attackProgress"].get<float>();
	isJumpAttack_ = data["isJumpAttack"].get<bool>();

	attackMotions_.clear();
	for (const auto& motion : data["attackMotions"]) {
		AttackMotion newMotion;
		newMotion.duration = motion["duration"].get<float>();
		newMotion.hitStartTime = motion["hitStartTime"].get<float>();
		newMotion.hitEndTime = motion["hitEndTime"].get<float>();

		for (const auto& keyframe : motion["keyframes"]) {
			newMotion.srtKeyframes.push_back({
				keyframe["time"].get<float>(),
				{keyframe["position"][0].get<float>(), keyframe["position"][1].get<float>(), keyframe["position"][2].get<float>()},
				{keyframe["scale"][0].get<float>(), keyframe["scale"][1].get<float>(), keyframe["scale"][2].get<float>()},
				{keyframe["rotation"][0].get<float>(), keyframe["rotation"][1].get<float>(), keyframe["rotation"][2].get<float>()}
				});
		}
		attackMotions_.push_back(newMotion);
	}
}

/// <summary>
/// JSONに保存
/// </summary>
/// <param name="filename"></param>
void PlayerWeapon::SaveToFile(const std::string& filename) const
{
	std::ofstream file(filename);
	if (file.is_open()) {
		file << ToJson().dump(4); // JSONをフォーマット付きで保存
		file.close();
	}
}

/// <summary>
/// JSONファイル読み込み
/// </summary>
/// <param name="filename"></param>
void PlayerWeapon::LoadFromFile(const std::string& filename)
{
	// ファイルを開く
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return;
	}

	// JSONデータを読み込む
	json data;
	file >> data;
	file.close();

	// JSONデータをPlayerWeaponに適用
	FromJson(data);
	std::cout << "File loaded successfully: " << filename << std::endl;
}

/// <summary>
/// コンボ可能かのフラグ
/// </summary>
/// <returns></returns>
bool PlayerWeapon::IsComboAvailable() const
{
	return canCombo_ && elapsedComboTime_ <= comboWindow_;
}

/// <summary>
/// 全状態の初期化
/// </summary>
void PlayerWeapon::InitializeState()
{
	if (stateRequest_) {
		// 状態の変更（リクエストの変更）
		state_ = stateRequest_.value();
		switch (state_)
		{
		case PlayerWeapon::WeaponState::Idle:
			InitIdle();
			break;
		case PlayerWeapon::WeaponState::Attacking:
			InitAttack();
			break;
		case PlayerWeapon::WeaponState::LSwing:
			InitLeftHorizontalSwing();
			break;
		case PlayerWeapon::WeaponState::RSwing:
			InitRightHorizontalSwiwng();
			break;
		case PlayerWeapon::WeaponState::JumpAttack:
			InitJumpAttack();
			break;
		case PlayerWeapon::WeaponState::Dashing:
			InitDash();
			break;
		case PlayerWeapon::WeaponState::Cooldown:
			InitCooldown();
			break;
		}
		// リクエストをリセット
		stateRequest_ = nullopt;
	}
}

/// <summary>
/// 待機モーションの初期化
/// </summary>
void PlayerWeapon::InitIdle()
{
	idleTime_ = 0.0f;
	worldTransform_.translation_ = { 0.0f, 0.0f, -2.0f };
	worldTransform_.rotation_ = { 0.0f, 0.0f, 0.0f };
	attackProgress_ = 0.0f;
	// ジャンプフラグをFalse
	isJumpAttack_ = false;
}

/// <summary>
/// 通常攻撃の初期化
/// </summary>
void PlayerWeapon::InitAttack()
{
	attackProgress_ = 0.0f;
	currentMotion_ = &attackMotions_[0];

}


void PlayerWeapon::InitLeftHorizontalSwing()
{
	attackProgress_ = 0.0f;
	currentMotion_ = &attackMotions_[1];
}

void PlayerWeapon::InitRightHorizontalSwiwng()
{
	attackProgress_ = 0.0f;
	currentMotion_ = &attackMotions_[2];
}

void PlayerWeapon::InitJumpAttack()
{
	attackProgress_ = 0.0f;
	currentMotion_ = &attackMotions_[3];
	isJumpAttack_ = true;
}

void PlayerWeapon::InitDash()
{
	attackProgress_ = 0.0f;
	currentMotion_ = &attackMotions_[4];
	isJumpAttack_ = false;
}


/// <summary>
/// _クールダウンの初期化
/// </summary>
void PlayerWeapon::InitCooldown()
{
	elapsedCooldownTime_ = 0.0f;
}

/// <summary>
/// 全体の各状態の更新処理
/// </summary>
void PlayerWeapon::UpdateState()
{
	const float deltaTime = 0.016f;
	switch (state_)
	{
	case PlayerWeapon::WeaponState::Idle:
		IdleMotion(deltaTime);
		break;
	case PlayerWeapon::WeaponState::Attacking:
		UpdateAttackMotion(deltaTime);
		break;
	case PlayerWeapon::WeaponState::LSwing:
		UpdateLeftHorizontalSwing(deltaTime);
		break;
	case PlayerWeapon::WeaponState::RSwing:
		UpdateRightHorizontalSwiwng(deltaTime);
		break;
	case PlayerWeapon::WeaponState::JumpAttack:
		UpdateJumpAttack(deltaTime);
		break;
	case PlayerWeapon::WeaponState::Dashing:
		UpdateDashMotion(deltaTime);
		break;
	case PlayerWeapon::WeaponState::Cooldown:
		UpdateCooldown(deltaTime);
		break;
	}
}

/// <summary>
/// 待機モーション
/// </summary>
/// <param name="deltaTime"></param>
void PlayerWeapon::IdleMotion(float deltaTime)
{
	// スペースを押されたらアタック
	if (input_->PushKey(DIK_SPACE)) {
		stateRequest_ = WeaponState::Attacking;
		return;
	}

	idleTime_ += deltaTime;

	// 垂直方向のぷかぷか運動
	float verticalOffset = sin(idleTime_) * 0.1f; // 振幅0.1のsin波

	// 回転運動
	float rotationSpeed = 1.0f; // 回転速度（度/秒）
	worldTransform_.rotation_.y += rotationSpeed * deltaTime;

	// 更新
	worldTransform_.translation_.y =  verticalOffset; // 上下運動を適用
	//worldTransform_.UpdateMatrix();

}

/// <summary>
/// 通常攻撃
/// </summary>
/// <param name="deltaTime"></param>
void PlayerWeapon::UpdateAttackMotion(float deltaTime)
{
	attackProgress_ += deltaTime / currentMotion_->duration;

	// SRTの補間による更新
	const auto& keyframes = currentMotion_->srtKeyframes;
	for (size_t i = 0; i < keyframes.size() - 1; ++i) {
		if (attackProgress_ >= keyframes[i].time && attackProgress_ <= keyframes[i + 1].time) {
			float t = (attackProgress_ - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
			// 位置の補間
			worldTransform_.translation_ = Lerp(keyframes[i].position, keyframes[i + 1].position, t);
			// 回転の補間
			worldTransform_.rotation_ = QuaternionToEuler(Slerp(keyframes[i].rotation, keyframes[i + 1].rotation, t));
		}
	}

	// コンボ可能タイミングの管理
	if (attackProgress_ >= 0.5f && !canCombo_) {
		canCombo_ = true;
		elapsedComboTime_ = 0.0f;
	}

	// コンボ開始
	if (attackProgress_ >= 0.5f) {
		if (IsComboAvailable() && input_->PushKey(DIK_SPACE)) {
			stateRequest_ = WeaponState::LSwing;
			currentMotion_ = LSwing_;
			attackProgress_ = 0.0f;
			canCombo_ = false;
		}
		// モーション終了
		else if (attackProgress_ >= 0.7f) {
			stateRequest_ = WeaponState::Cooldown; // クールダウン状態へ移行
		}
	}


}

void PlayerWeapon::UpdateLeftHorizontalSwing(float deltaTime)
{
	attackProgress_ += deltaTime / currentMotion_->duration;

	// 攻撃モーションの補間
	const auto& keyframes = currentMotion_->srtKeyframes;
	for (size_t i = 0; i < keyframes.size() - 1; ++i) {
		if (attackProgress_ >= keyframes[i].time && attackProgress_ <= keyframes[i + 1].time) {
			float t = (attackProgress_ - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
			t = std::clamp(t, 0.0f, 1.0f);
			worldTransform_.translation_ = Lerp(keyframes[i].position, keyframes[i + 1].position, t);
			worldTransform_.rotation_ = QuaternionToEuler(Slerp(keyframes[i].rotation, keyframes[i + 1].rotation, t));
		}
	}

	// コンボ可能タイミングの管理
	if (attackProgress_ >= 0.5f && !canCombo_) {
		canCombo_ = true;
		elapsedComboTime_ = 0.0f;
	}


	// コンボ開始
	if (attackProgress_ >= 0.5f) {
		if (IsComboAvailable() && input_->PushKey(DIK_SPACE)) {
			stateRequest_ = WeaponState::RSwing;
			currentMotion_ = RSwing_; // ダッシュ攻撃モーション
			attackProgress_ = 0.0f;
			canCombo_ = false;
		}
		// モーション終了
		else if (attackProgress_ >= 0.7f) {
			stateRequest_ = WeaponState::Cooldown; // クールダウン状態へ移行
		}
	}

}

void PlayerWeapon::UpdateRightHorizontalSwiwng(float deltaTime)
{
	attackProgress_ += deltaTime / currentMotion_->duration;

	// 攻撃モーションの補間
	const auto& keyframes = currentMotion_->srtKeyframes;
	for (size_t i = 0; i < keyframes.size() - 1; ++i) {
		if (attackProgress_ >= keyframes[i].time && attackProgress_ <= keyframes[i + 1].time) {
			float t = (attackProgress_ - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
			t = std::clamp(t, 0.0f, 1.0f);
			worldTransform_.translation_ = Lerp(keyframes[i].position, keyframes[i + 1].position, t);
			worldTransform_.rotation_ = QuaternionToEuler(Slerp(keyframes[i].rotation, keyframes[i + 1].rotation, t));
		}
	}

	// コンボ可能タイミングの管理
	if (attackProgress_ >= 0.5f && !canCombo_) {
		canCombo_ = true;
		elapsedComboTime_ = 0.0f;
	}

	// コンボ開始
	if (attackProgress_ >= 0.5f) {
		if (IsComboAvailable() && input_->PushKey(DIK_SPACE)) {
			stateRequest_ = WeaponState::JumpAttack;
			currentMotion_ = jumpAttack_; // ダッシュ攻撃モーション
			attackProgress_ = 0.0f;
			canCombo_ = false;
		}
		// モーション終了
		else if (attackProgress_ >= 0.7f) {
			stateRequest_ = WeaponState::Cooldown; // クールダウン状態へ移行
		}
	}

}

void PlayerWeapon::UpdateJumpAttack(float deltaTime)
{
	attackProgress_ += deltaTime / currentMotion_->duration;

	// 攻撃モーションの補間
	const auto& keyframes = currentMotion_->srtKeyframes;
	for (size_t i = 0; i < keyframes.size() - 1; ++i) {
		if (attackProgress_ >= keyframes[i].time && attackProgress_ <= keyframes[i + 1].time) {
			float t = (attackProgress_ - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
			t = std::clamp(t, 0.0f, 1.0f);
			worldTransform_.translation_ = Lerp(keyframes[i].position, keyframes[i + 1].position, t);
			worldTransform_.rotation_ = QuaternionToEuler(Slerp(keyframes[i].rotation, keyframes[i + 1].rotation, t));
		}
	}

	if (attackProgress_ >= 0.9f) {
		isJumpAttack_ = false;
	}

	// コンボ可能タイミングの管理
	if (attackProgress_ >= 1.0f && !canCombo_) {
		canCombo_ = true;
		elapsedComboTime_ = 0.0f;
	}

	// コンボ開始
	if (attackProgress_ >= 1.0f) {
		if (IsComboAvailable() && input_->PushKey(DIK_SPACE)) {
			//stateRequest_ = WeaponState::Dashing;
			//currentMotion_ = dashMotion_; // ダッシュ攻撃モーション
			//attackProgress_ = 0.0f;
			//canCombo_ = false;
		}
		// モーション終了
		else if (attackProgress_ >= 1.2f) {
			stateRequest_ = WeaponState::Cooldown; // クールダウン状態へ移行
			
		}
	}
}

void PlayerWeapon::UpdateDashMotion(float deltaTime)
{
	attackProgress_ += deltaTime / currentMotion_->duration;

	// ダッシュ攻撃モーションの補間
	const auto& keyframes = currentMotion_->srtKeyframes;
	for (size_t i = 0; i < keyframes.size() - 1; ++i) {
		if (attackProgress_ >= keyframes[i].time && attackProgress_ <= keyframes[i + 1].time) {
			float t = (attackProgress_ - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
			t = std::clamp(t, 0.0f, 1.0f);
			worldTransform_.translation_ = Lerp(keyframes[i].position, keyframes[i + 1].position, t);
			worldTransform_.rotation_ = QuaternionToEuler(Slerp(keyframes[i].rotation, keyframes[i + 1].rotation, t));
		}
	}

	// コンボ可能タイミングの管理
	if (attackProgress_ >= 0.5f && !canCombo_) {
		canCombo_ = true;
		elapsedComboTime_ = 0.0f;
	}

	// コンボ開始
	if (attackProgress_ >= 0.5f) {
		if (IsComboAvailable() && input_->PushKey(DIK_SPACE)) {
			stateRequest_ = WeaponState::Dashing;
			currentMotion_ = dashMotion_; // ダッシュ攻撃モーション
			attackProgress_ = 0.0f;
			canCombo_ = false;
		}
		// モーション終了
		else if (attackProgress_ >= 0.7f) {
			stateRequest_ = WeaponState::Cooldown; // クールダウン状態へ移行
		}
	}
}

/// <summary>
/// クールダウン
/// </summary>
/// <param name="deltaTime"></param>
void PlayerWeapon::UpdateCooldown(float deltaTime)
{
	// 経過時間を更新
	elapsedCooldownTime_ += deltaTime;

	// クールダウンが終了したら状態をIdleに戻す
	if (elapsedCooldownTime_ >= cooldownTime_) {
		stateRequest_ = WeaponState::Idle; // 武器の状態を待機に戻す
		elapsedCooldownTime_ = 0.0f; // 経過時間をリセット
	}
}

void PlayerWeapon::OnCollision(Collider* other)
{
	// 衝突相手の種別IDを取得
	uint32_t typeID = other->GetTypeID();
	// 衝突相手が敵なら
	if (typeID == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) {


	}

}

Vector3 PlayerWeapon::GetCenterPosition() const
{
	// ローカル座標でのオフセット
	const Vector3 offset = { 0.0f, 0.0f, 0.0f };
	// ワールド座標に変換
	Vector3 worldPos = TransformCoordinates(offset, worldTransform_.matWorld_);

	return worldPos;
}

Matrix4x4 PlayerWeapon::GetWorldMatrix() const
{
	return worldTransform_.matWorld_;
}

void PlayerWeapon::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "PlayerWeapon";

	// 武器の位置・回転・スケール
	worldTransform_.translation_ = globalVariables->GetVector3Value(groupName, "Translation");
	worldTransform_.rotation_ = globalVariables->GetVector3Value(groupName, "Rotation");
	worldTransform_.scale_ = globalVariables->GetVector3Value(groupName, "Scale");

	// クールダウン時間
	cooldownTime_ = globalVariables->GetFloatValue(groupName, "CooldownTime");

	// コンボ猶予時間
	comboWindow_ = globalVariables->GetFloatValue(groupName, "ComboWindow");

	// 各モーションのパラメータ
	for (size_t i = 0; i < attackMotions_.size(); ++i) {
		std::string motionName = "AttackMotion_" + std::to_string(i);

		attackMotions_[i].duration = globalVariables->GetFloatValue(groupName, motionName + "_Duration");
		attackMotions_[i].hitStartTime = globalVariables->GetFloatValue(groupName, motionName + "_HitStartTime");
		attackMotions_[i].hitEndTime = globalVariables->GetFloatValue(groupName, motionName + "_HitEndTime");

		for (size_t j = 0; j < attackMotions_[i].srtKeyframes.size(); ++j) {
			std::string keyframeName = motionName + "_Keyframe_" + std::to_string(j);

			attackMotions_[i].srtKeyframes[j].time = globalVariables->GetFloatValue(groupName, keyframeName + "_Time");
			attackMotions_[i].srtKeyframes[j].position = globalVariables->GetVector3Value(groupName, keyframeName + "_Position");
			attackMotions_[i].srtKeyframes[j].scale = globalVariables->GetVector3Value(groupName, keyframeName + "_Scale");
			attackMotions_[i].srtKeyframes[j].rotation = globalVariables->GetQuaternionValue(groupName, keyframeName + "_Rotation");
		}
	}
}
