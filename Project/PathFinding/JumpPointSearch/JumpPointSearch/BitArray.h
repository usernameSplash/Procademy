#pragma once

namespace PathFinder
{
	class BitArray
	{
	public:
		BitArray(const size_t width, const size_t depth);
		~BitArray();

		__int64 GetWord(const size_t x, const size_t y) const;
		bool GetValue(const size_t x, const size_t y) const;
		void SetValue(const size_t x, const size_t y, const bool value);

	private:
		__int64* _container;
		size_t _depth;
		size_t _width;
		size_t _wordWidth;
	};
}