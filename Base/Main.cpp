#include "Base.h"
#include <iostream>

int wmain(int args, wchar_t* argv[])
{
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	int returnvalue = 0;
	Program* BaseProgram = new Program();
	{
		BaseProgram->Start();

		returnvalue = BaseProgram->Result;

		delete BaseProgram;
	}

	CoUninitialize();
	Sleep(2000);
	return returnvalue;
}