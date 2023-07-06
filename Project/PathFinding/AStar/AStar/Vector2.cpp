#include "Vector2.h"
#include <cmath>

namespace PathFinder
{
	Vector2::Vector2(int x, int y)
		: _x(x)
		, _y(y)
	{

	}

	int Vector2::DistanceManhattan(const Vector2& other) const
	{
		return (abs(_x - other._x) + (abs(_y - other._y))) * DISTANCE::COMMON;
	}

	int Vector2::DistanceDiagnal(const Vector2& other) const
	{
		return abs(_x - other._x) * DISTANCE::DIAGNAL;
	}

	Vector2& Vector2::operator=(const Vector2& rhs)
	{
		if (this != &rhs)
		{
			_x = rhs._x;
			_y = rhs._y;
		}

		return *this;
	}

	Vector2 Vector2::operator+(const Vector2& rhs) const
	{
		return Vector2(_x + rhs._x, _y + rhs._y);
	}

	Vector2 Vector2::operator*(const int time) const
	{
		return Vector2(_x * time, _y * time);
	}

	bool Vector2::operator==(const Vector2& rhs) const
	{
		return (_x == rhs._x && _y == rhs._y);
	}

	bool Vector2::operator!=(const Vector2& rhs) const
	{
		return (_x != rhs._x || _y != rhs._y);
	}
}