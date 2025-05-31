#include "Misc.h"

float tn::BalanceBetweenZeroAndOne(float value) {
	if (value > 1.0f) return 1.0f;
	else if (value >= 0.0f and value <= 0.0f) return value;
	else return 0.0f;
}

int tn::ByteCopy(const void* Source, void* Destination, size_t SizeOfSource)
{
	if (SizeOfSource == 0)
		return ByteCopy_Success;

	if (Source != nullptr and Destination != nullptr)
	{
		const unsigned char* byteofSource = reinterpret_cast<const unsigned char*>(Source);
		unsigned char* byteofDestination = reinterpret_cast<unsigned char*>(Destination);

		for (size_t i = 0; i < SizeOfSource; ++i) {
			byteofDestination[i] = byteofSource[i];
		}
	}
	else
	{
		return ByteCopy_Fail;
	}

	return ByteCopy_Success;
}