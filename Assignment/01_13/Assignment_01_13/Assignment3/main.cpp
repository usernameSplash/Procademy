#include <cstdio>
#include <iostream>

void ControlEachByteOfNumber()
{
	uint32_t number;
	number = 0;

	while (true)
	{
		uint8_t bytePosition;
		uint8_t tempValue;
		uint32_t value;

		printf("Byte Position (1~4): ");
		scanf("%hhu", &bytePosition);

		printf("Value (0~255): ");
		scanf("%hhu", &tempValue);

		value = tempValue << ((bytePosition - 1) * 8);			// Shift Input Value to Correct Byte Zone.
		value = ~(0) & value;									// Make All Other Bits <1> for Change Value on Only Specific Byte Zone.

		number = number & ~(0xFF << ((bytePosition - 1) * 8));	// Erase Bits of `number` on Specific Byte Zone.
		number = number | value;								// Apply Input Value on Specific Byte Zone.

		for (int i = 0; i < 4; i++)
		{
			printf("Byte Zone %d: %d\n", (i + 1), (uint8_t)(number >> (i * 8)));
		}

		printf("Total 4 Byte Integer Value: %#010x\n\n", number);
	}
}
int main()
{
	ControlEachByteOfNumber();
	return 0;
}