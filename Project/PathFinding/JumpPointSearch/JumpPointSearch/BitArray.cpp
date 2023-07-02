#include "BitArray.h"

namespace PathFinder
{
	BitArray::BitArray(size_t width, size_t depth)
		: _width(width)
		, _depth(depth)
	{
		_wordWidth = _width / 64;

		if (_width % 64 != 0)
		{
			_wordWidth += 1;
		}

		_container = new __int64[_depth * _wordWidth];
	}

	BitArray::~BitArray()
	{
		delete[] _container;
	}

	/*
		return word contains (x, y) value
	*/
	__int64 BitArray::GetWord(const size_t x, const size_t y) const
	{
		if (y >= _depth || x >= _width || y < 0 || x < 0)
		{
			return -1;
		}

		return _container[(y * _wordWidth) + (x / 64)];
	}

	bool BitArray::GetValue(const size_t x, const size_t y) const
	{
		if (y >= _depth || x >= _width || y < 0 || x < 0)
		{
			return -1;
		}

		return _container[(y * _wordWidth) + (x / 64)] & (1 << (x % 64));
	}

	void BitArray::SetValue(const size_t x, const size_t y, const bool value)
	{
		if (y >= _depth || x >= _width || y < 0 || x < 0)
		{
			return;
		}

		if (value)
		{
			_container[(y * _wordWidth) + (x / 64)] |= 1LL << (x % 64);
		}
		else
		{
			_container[(y * _wordWidth) + (x / 64)] &= ~(1LL << (x % 64));
		}
	}


}