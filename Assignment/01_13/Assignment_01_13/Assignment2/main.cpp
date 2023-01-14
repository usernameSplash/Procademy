#include <cstddef>
#include <iostream>
#include <iomanip>

void ControlEachBitOfNumber()
{
	uint16_t number;
	number = 0;
	
	while (true) {
		uint16_t bitPosition;
		bool bitStatus;

		std::cout << "비트 위치: ";
		std::cin >> bitPosition;

		std::cout << "OFF/ON [0,1]: ";
		std::cin >> bitStatus;

		if (bitPosition < 0 || bitPosition > 16)
		{
			std::cout << "비트 범위를 초과하였습니다." << "\n";
			continue;
		}

		if(bitStatus) 
		{
			number = number | (1 << (bitPosition - 1));
		}
		else
		{
			number = number & ~(1 << (bitPosition - 1));
		}

		for (int pos = 15; pos >= 0; pos--)
		{
			std::cout << std::setw(2) << (pos + 1) << "번 Bit: " << (bool)(number & (1 << pos)) << "\n";
		}

		std::cout << "비트 패턴 해석 결과: " << number << "\n" << std::endl;
	}
}

int main()
{
	ControlEachBitOfNumber();
	return 0;
}