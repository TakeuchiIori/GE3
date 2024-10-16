#pragma once
struct Vector3 final {
	float x;
	float y;
	float z;

	// ベクトルの加算演算子オーバーロード
	Vector3 operator+(const Vector3& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	// ベクトルの引き算演算子オーバーロード
	Vector3 operator-(const Vector3& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}

	// スカラ値との乗算を行うオペレーター
	Vector3& operator+=(const Vector3& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}
	// スカラ乗算オペレーターの定義
	Vector3 operator*(float scalar) const {
		return Vector3(x * scalar, y * scalar, z * scalar);
	}
};
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
