#pragma once
#include <Vector3.h>
#include <WorldTransform/WorldTransform.h>


class FollowCamera
{
public:

    void Initialize();
    void Update();

    void UpdateInput();
	void FollowProsess();
    

    Vector3 translate_ = { 0,0,0 };
    Vector3 scale_ = { 1,1,1 };
    Vector3 rotate_ = { 0,0,0 };

    Matrix4x4 matView_ = {};

    void SetTarget(const WorldTransform& target) { target_ = &target; }

private:


    void ImGui();
    Vector3 rotation_;
    float kDeadZoneL_ = 100.0f;
    // 追従対象
    const WorldTransform* target_;
};

