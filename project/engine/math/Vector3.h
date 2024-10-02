#pragma once
struct Vector3 final {
	float x;
	float y;
	float z;
};
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};