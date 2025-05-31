#include "Base.h"
#include "JSON.h"

void Program::SetupInitialLangSelection()
{
	tn::StartingWindowDescription StartingWndDesc = {};
	StartingWndDesc.SizeX = 200;
	StartingWndDesc.SizeY = 125;
	StartingWndDesc.WindowClassType = L"SelectionMsgBoxTN";
	StartingWndDesc.WindowName = L"Select Language";
	StartingWndDesc.PositionX = 50;
	StartingWndDesc.PositionY = 50;
	StartingWndDesc.WindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	StartingWndDesc.OtherAttributes = this;
	StartingWndDesc.hMenu = nullptr;
	StartingWndDesc.WindowExternalStyle = 0;
	StartingWndDesc.Parent = nullptr;
	StartingWndDesc.hInstance = this->hInst;
	StartingWndDesc.OtherAttributes = static_cast<void*>(this);

	this->SelectLang.Base = new tn::Window();
	this->SelectLang.Base->ContinueAfterDestroyed = false;
	this->SelectLang.Base->UseWindowDataAsClass = false;
	bool CreatedSuccessfully = this->SelectLang.Base->Create(&StartingWndDesc);

//#ifndef _DEBUG
	HANDLE PlayerDataFile = CreateFileW(L"./playerdata.json", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (PlayerDataFile == INVALID_HANDLE_VALUE)
	{
		if (CreatedSuccessfully == true)
		{
			this->SelectLang.Base->Show(tn::WindowVisibleTypes::Show);

			bool Stop = false;
			MSG msg = {};
			while (Stop == false)
			{
				while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessageW(&msg);

					if (IsWindow(this->SelectLang.Base->GetHWND()) == false) {
						Stop = true;
					}
				}
				Sleep(1);
			}
		}
		else
		{
			MessageBoxExW(nullptr, L"Phần mềm này có vấn đề để không cho bạn chọn ngôn ngữ", L"ERROR", MB_OK, 0);
		}
		JSONElement element = {};
		element.InitializeAsObject();

		JSONElement langelement = this->CurrentLang;
		langelement.SetName(L"lang");
		element << langelement;

		JSONGenerator Parser = {};
		Parser.SetFile(L"./playerdata.json");
		Parser.SetElement(&element);
		Parser.Start();
	}
	else
	{
		JSONElement element = {};
		JSONParser Parser = {};
		Parser.LoadFromFile(PlayerDataFile);
		Parser.SetOutput(&element);
		Parser.StartParse();

		if (element.GetEnumType() == JSONElementTypeEnum::Object)
		{
			JSONElement langelement = element[L"lang"];
			if (langelement.GetEnumType() == JSONElementTypeEnum::String)
			{
				std::wstring str = langelement;
				this->CurrentLang = str;
			}
		}

		CloseHandle(PlayerDataFile);
	}
//#else
	//this->CurrentLang = L"vi";
//#endif
}