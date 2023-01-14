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

		std::cout << "��Ʈ ��ġ: ";
		std::cin >> bitPosition;

		std::cout << "OFF/ON [0,1]: ";
		std::cin >> bitStatus;

		if (bitPosition < 0 || bitPosition > 16)
		{
			std::cout << "��Ʈ ������ �ʰ��Ͽ����ϴ�." << "\n";
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
			std::cout << std::setw(2) << (pos + 1) << "�� Bit: " << (bool)(number & (1 << pos)) << "\n";
		}

		std::cout << "��Ʈ ���� �ؼ� ���: " << number << "\n" << std::endl;
	}
}

int main()
{
	ControlEachBitOfNumber();
	return 0;
}