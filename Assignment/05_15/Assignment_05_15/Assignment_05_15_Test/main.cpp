
#include "SerializationBuffer.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>

using namespace MyDataStruct;

void Test0(void);
void Test1(void);

#define MY_SEED 3000

int main(void)
{
	srand(MY_SEED);
	//Test0();
	Test1();
	return 0;
}

void Test0(void)
{
	SPacket s;

	unsigned char a = 123;
	unsigned short b = 65530;
	int c = 123123123;
	long long d = 123123123123123123;

	unsigned char aa;
	unsigned short bb;
	int cc;
	long long dd;

	s << a << b << c << d;
	s >> aa >> bb >> cc >> dd;
	
	assert(a == aa);
	assert(b == bb);
	assert(c == cc);
	assert(d == dd);
}

void Test1(void)
{
	SPacket s;
	std::vector<long long> v;
	v.reserve(500);

	for (size_t iCnt = 0; iCnt < 250; iCnt++)
	{
		printf("%zu\n", iCnt);
		long long value = ((short)rand() << 48) + ((short)rand() << 32) + ((short)rand() << 16) + ((short)rand());
		
		int typeNum = iCnt % 12;

		switch (typeNum)
		{
		case 0:
			{
				size_t prevSize = s.Size();

				unsigned char ucValue = (unsigned char)value;
				s << ucValue;
				printf("%d\n", ucValue);

				assert((prevSize + sizeof(ucValue)) == s.Size());
				break;
			}
			
		case 1:
			{
				size_t prevSize = s.Size();

				char cValue = (char)value;
				s << cValue;

				assert((prevSize + sizeof(cValue)) == s.Size());
				break;
			}
		
		case 2:
			{
				size_t prevSize = s.Size();

				unsigned short usValue = (unsigned short)value;
				s << usValue;

				assert((prevSize + sizeof(usValue)) == s.Size());
				break;
			}

		case 3:
			{
				size_t prevSize = s.Size();

				short sValue = (short)value;
				s << sValue;

				assert((prevSize + sizeof(sValue)) == s.Size());
				break;
			}

		case 4:
			{
				size_t prevSize = s.Size();

				unsigned int uiValue = (unsigned int)value;
				s << uiValue;

				assert((prevSize + sizeof(uiValue)) == s.Size());
				break;
			}

		case 5:
			{
				size_t prevSize = s.Size();

				int iValue = (int)value;
				s << iValue;

				assert((prevSize + sizeof(iValue)) == s.Size());
				break;
			}

		case 6:
			{
				size_t prevSize = s.Size();

				unsigned long ulValue = (unsigned long)value;
				s << ulValue;
				
				assert((prevSize + sizeof(ulValue)) == s.Size());
				break;
			}

		case 7:
			{
				size_t prevSize = s.Size();

				long lValue = (long)value;
				s << lValue;

				assert((prevSize + sizeof(lValue)) == s.Size());
				break;
			}

		case 8:
			{
				size_t prevSize = s.Size();

				unsigned long long ullValue = (unsigned long long)value;
				s << ullValue;

				assert((prevSize + sizeof(ullValue)) == s.Size());
				break;
			}

		case 9:
			{
				size_t prevSize = s.Size();

				s << value;

				assert((prevSize + sizeof(value)) == s.Size());
				break;
			}

		case 10:
			{
				size_t prevSize = s.Size();

				float fValue = (float)value;
				s << fValue;

				assert((prevSize + sizeof(fValue)) == s.Size());
				break;
			}

		case 11:
			{
				size_t prevSize = s.Size();

				double dValue = (double)value;
				s << dValue;

				assert((prevSize + sizeof(dValue)) == s.Size());
				break;
			}
		}
		
		v.push_back(value);
	}

	for (size_t iCnt = 0; iCnt < 250; iCnt++)
	{
		int typeNum = iCnt % 12;
		switch (typeNum)
		{
		case 0:
			{
				size_t prevSize = s.Size();

				unsigned char ucValue;
				s >> ucValue;

				printf("%d %d\n", (unsigned char)v[iCnt], ucValue);
				assert((unsigned char)v[iCnt] == ucValue);
				assert((prevSize - sizeof(ucValue)) == s.Size());
				break;
			}

		case 1:
			{
				size_t prevSize = s.Size();

				char cValue;
				s >> cValue;

				assert((char)v[iCnt] == cValue);
				assert((prevSize - sizeof(cValue)) == s.Size());
				break;
			}

		case 2:
			{
				size_t prevSize = s.Size();

				unsigned short usValue;
				s >> usValue;

				assert((unsigned short)v[iCnt] == usValue);
				assert((prevSize - sizeof(usValue)) == s.Size());
				break;
			}

		case 3:
			{
				size_t prevSize = s.Size();

				short sValue;
				s >> sValue;

				assert((short)v[iCnt] == sValue);
				assert((prevSize - sizeof(sValue)) == s.Size());
				break;
			}

		case 4:
			{
				size_t prevSize = s.Size();

				unsigned int uiValue;
				s >> uiValue;

				assert((unsigned int)v[iCnt] == uiValue);
				assert((prevSize - sizeof(uiValue)) == s.Size());
				break;
			}

		case 5:
			{
				size_t prevSize = s.Size();

				int iValue;
				s >> iValue;

				assert((int)v[iCnt] == iValue);
				assert((prevSize - sizeof(iValue)) == s.Size());
				break;
			}

		case 6:
			{
				size_t prevSize = s.Size();

				unsigned long ulValue;
				s >> ulValue;

				assert((unsigned long)v[iCnt] == ulValue);
				assert((prevSize - sizeof(ulValue)) == s.Size());
				break;
			}

		case 7:
			{
				size_t prevSize = s.Size();

				long lValue;
				s >> lValue;

				assert((long)v[iCnt] == lValue);
				assert((prevSize - sizeof(lValue)) == s.Size());
				break;
			}

		case 8:
			{
				size_t prevSize = s.Size();

				unsigned long long ullValue;
				s >> ullValue;

				assert((unsigned long long)v[iCnt] == ullValue);
				assert((prevSize - sizeof(ullValue)) == s.Size());
				break;
			}

		case 9:
			{
				size_t prevSize = s.Size();

				long long llValue;
				s >> llValue;

				assert((long long)v[iCnt] == llValue);
				assert((prevSize - sizeof(llValue)) == s.Size());
				break;
			}

		case 10:
			{
				size_t prevSize = s.Size();

				float fValue;
				s >> fValue;

				assert((float)v[iCnt] == fValue);
				assert((prevSize - sizeof(fValue)) == s.Size());
				break;
			}

		case 11:
			{
				size_t prevSize = s.Size();

				double dValue;
				s >> dValue;

				assert((double)v[iCnt] == dValue);
				assert((prevSize - sizeof(dValue)) == s.Size());
				break;
			}
		}

	}
}