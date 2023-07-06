#pragma once

namespace PathFinder
{
	enum DISTANCE
	{
		COMMON = 100,
		DIAGNAL = 141
	};

	class Vector2
	{
	public:
		Vector2(const int x, const int y);
		~Vector2() = default;

	public:
		int DistanceManhattan(const Vector2& other) const;
		int DistanceDiagnal(const Vector2& other) const;

		Vector2& operator=(const Vector2& rhs);
		Vector2 operator+(const Vector2& rhs) const;
		Vector2 operator*(const int time) const;
		bool operator==(const Vector2& rhs) const;
		bool operator!=(const Vector2& rhs) const;

	public:
		int _x;
		int _y;
	};
}