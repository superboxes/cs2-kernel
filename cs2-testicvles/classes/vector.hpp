#pragma once
#ifndef _VECTOR_ESP_
#define _VECTOR_ESP_
#define _USE_MATH_DEFINES
#include <numbers>
#include <cmath>
#include <math.h>

struct Vector3
{
	// constructor
	constexpr Vector3(
		const float x = 0.f,
		const float y = 0.f,
		const float z = 0.f) noexcept :
		x(x), y(y), z(z) { }

	// operator overloads
	constexpr const Vector3& operator-(const Vector3& other) const noexcept
	{
		return Vector3{ x - other.x, y - other.y, z - other.z };
	}

	constexpr const Vector3& operator+(const Vector3& other) const noexcept
	{
		return Vector3{ x + other.x, y + other.y, z + other.z };
	}

	constexpr const Vector3& operator/(const float factor) const noexcept
	{
		return Vector3{ x / factor, y / factor, z / factor };
	}

	constexpr const Vector3& operator*(const float factor) const noexcept
	{
		return Vector3{ x * factor, y * factor, z * factor };
	}

	constexpr const bool operator>(const Vector3& other) const noexcept {
		return x > other.x && y > other.y && z > other.z;
	}

	constexpr const bool operator>=(const Vector3& other) const noexcept {
		return x >= other.x && y >= other.y && z >= other.z;
	}

	constexpr const bool operator<(const Vector3& other) const noexcept {
		return x < other.x && y < other.y && z < other.z;
	}

	constexpr const bool operator<=(const Vector3& other) const noexcept {
		return x <= other.x && y <= other.y && z <= other.z;
	}

	// utils
	constexpr const Vector3& ToAngle() const noexcept
	{
		return Vector3{
			std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>),
			std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>),
			0.0f };
	}

	float length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	float length2d() const {
		return std::sqrt(x * x + y * y);
	}

	constexpr const bool IsZero() const noexcept
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}

	float calculate_distance(const Vector3& point) const {
		float dx = point.x - x;
		float dy = point.y - y;
		float dz = point.z - z;

		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}

	float magnitude(const Vector3& other)
	{
		return sqrtf(pow(other.x - this->x, 2) + pow(other.y - this->y, 2) + pow(other.z - this->z, 2));
	}

	float yawAngle(const Vector3& other) const {
		return atan2(other.y - this->y, other.x - this->x) * (180 / M_PI);
	}

	float pitchAngle(const Vector3& other) const {
		float mag = sqrtf(pow(other.x - this->x, 2) + pow(other.y - this->y, 2) + pow(other.z - this->z, 2));
		return asinf(-(other.z - this->z) / mag) * (180 / M_PI);
	}

	float normalize() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	Vector3 delta(const Vector3& other) const
	{
		return Vector3(other.x - x, other.y - y, other.z - z);
	}

	// struct data
	float x, y, z;
};
#endif