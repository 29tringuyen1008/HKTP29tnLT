#include "Debugger.h"
#include "GameObject.h"
#include <string>
#include <codecvt>

LRESULT CALLBACK ProgramDebugger::DebuggerProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam)
{
	ProgramDebugger* ReferenceDebugger = (ProgramDebugger*) GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	if (ReferenceDebugger == nullptr) return DefWindowProcW(hWnd, hMsg, wParam, lParam);

	RECT WindowSrc;
	GetClientRect(hWnd, &WindowSrc);

	switch (hMsg)
	{
	case WM_GETMINMAXINFO: {
		MINMAXINFO* mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize.x = 300;
		mmi->ptMinTrackSize.y = 200;
	}
	case WM_SIZE:
	{
		InvalidateRect(hWnd, NULL, TRUE);

		MoveWindow(ReferenceDebugger->GUI.m_PropertiesTab->WindowCore,
			0,
			0,
			(int)(static_cast<float>(WindowSrc.right) - (static_cast<float>(WindowSrc.right) / 5)),
			(int)(static_cast<float>(WindowSrc.bottom) * 0.5f),
			TRUE);

		RECT ExploreListSrc;
		GetWindowRect(ReferenceDebugger->GUI.m_ExploreList->WindowCore, &ExploreListSrc);

		MoveWindow(ReferenceDebugger->GUI.m_ExploreList->WindowCore,
			(int)(static_cast<float>(WindowSrc.right) - (static_cast<float>(WindowSrc.right) / 5)),
			0,
			(static_cast<float>(WindowSrc.right) / 5),
			(int)(static_cast<float>(WindowSrc.bottom) * 0.5f),
		TRUE);

		RECT LogListSrc;
		GetWindowRect(ReferenceDebugger->GUI.LogList, &LogListSrc);
		ReferenceDebugger->GUI.LogListPosSize[0] = {
			0,
			static_cast<float>(WindowSrc.bottom) * 0.5f
		};
		ReferenceDebugger->GUI.LogListPosSize[1] = {
			static_cast<float>(WindowSrc.right),
			static_cast<float>(WindowSrc.bottom) - ReferenceDebugger->GUI.LogListPosSize[0].Y
		};

		MoveWindow(ReferenceDebugger->GUI.LogList,
			ReferenceDebugger->GUI.LogListPosSize[0].X, ReferenceDebugger->GUI.LogListPosSize[0].Y,
			ReferenceDebugger->GUI.LogListPosSize[1].X, ReferenceDebugger->GUI.LogListPosSize[1].Y,
			TRUE);

		return 0L;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 2:
			DestroyWindow(hWnd);
			break;
		}
		return 0L;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_TAB:
			ReferenceDebugger->Update();
			break;
		}
		return 0L;
	default:
		return DefWindowProcW(hWnd, hMsg, wParam, lParam);
	}
}

LRESULT CALLBACK ProgramDebugger::PropertiesTabProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam)
{
	PropertiesTab* Main = (PropertiesTab*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	HFONT DefaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	RECT WindowSrc;
	GetClientRect(hWnd, &WindowSrc);

	switch (hMsg)
	{
	case WM_CREATE:
	{
		CREATESTRUCT* CreateParameter = reinterpret_cast<CREATESTRUCT*>(lParam);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(CreateParameter->lpCreateParams));
		return 0L;
	}
	case WM_VSCROLL:
	{
		ScrollWindow(hWnd, 0, 0, nullptr, nullptr);
		return 0L;
	}
	case WM_SIZE:
	{
		return 0L;
	}
	case PM_SELECTOBJECTBOX:
	{
		DebuggerExploreBox* ObjectBox = reinterpret_cast<DebuggerExploreBox*>(lParam);
		for (size_t id = 0; id < Main->HWNDList.size(); id++)
		{
			DestroyWindow(Main->HWNDList.at(id));
			Main->HWNDList.erase(Main->HWNDList.begin() + id);
		};
		Main->HWNDList.clear();
		Main->HWNDList.shrink_to_fit();

		std::wstring Title;
		if (ObjectBox->Alias != L"" and ObjectBox->Alias != nullptr)
		{
			Title = ObjectBox->Alias + (L" (" + ObjectBox->ReferenceObject->GetName() + L")");
		}
		else
		{
			Title = ObjectBox->ReferenceObject->GetName();
		}
		HWND Name = CreateWindow(L"Static", Title.c_str(), WS_VISIBLE | WS_CHILD, 0, 0, WindowSrc.right, 15, hWnd, nullptr, nullptr, nullptr);
		SendMessageW(Name, WM_SETFONT, WPARAM(DefaultFont), TRUE);
		Main->HWNDList.push_back(Name);

		unsigned long Height = 30;
		if (ObjectBox->ReferenceObject->GetParent() != nullptr)
		{
			std::wstring ParentText = L"Parent: " + ObjectBox->ReferenceObject->GetParent()->GetName();
			HWND ParentProp = CreateWindow(L"Static", ParentText.c_str(), WS_VISIBLE | WS_CHILD, 0, Height, WindowSrc.right, 15, hWnd, nullptr, nullptr, nullptr);
			SendMessageW(ParentProp, WM_SETFONT, WPARAM(DefaultFont), TRUE);
			Main->HWNDList.push_back(ParentProp);
			Height += 15;
		}

		return 0L;
	}
	default:
		return DefWindowProcW(hWnd, hMsg, wParam, lParam);
	}
}

LRESULT CALLBACK ProgramDebugger::ExploreListProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam)
{
	SCROLLINFO si = {};
	DebuggerExploreList* Main = (DebuggerExploreList*) GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	HFONT DefaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	RECT WindowSrc;
	GetClientRect(hWnd, &WindowSrc);

	switch (hMsg)
	{
	case WM_CREATE:
	{
		CREATESTRUCT* CreateParameter = reinterpret_cast<CREATESTRUCT*>(lParam);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(CreateParameter->lpCreateParams));
		return 0L;
	}
	case WM_MOUSEWHEEL:
	{
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		int linesToScroll = zDelta / WHEEL_DELTA;

		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &si);

		if (si.nMax > WindowSrc.bottom)
		{
			Main->ExploreListScrollPos = si.nPos;

			if (linesToScroll < 0)
			{
				si.nPos += 10;
			}
			else if (linesToScroll > 0)
			{
				si.nPos -= 10;
			}

			si.nPos = max(0, min(si.nPos, si.nMax - (int)si.nPage + 1));

			SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

			if (Main->ExploreListScrollPos != si.nPos)
			{
				ScrollWindow(hWnd, 0, Main->ExploreListScrollPos - si.nPos, NULL, NULL);
				UpdateWindow(hWnd);

				Main->ExploreListScrollPos = si.nPos;
			}
		}
		return 0;
	}
	case WM_VSCROLL:
	{
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &si);

		if (si.nMax > WindowSrc.bottom)
		{
			Main->ExploreListScrollPos = si.nPos;

			switch (LOWORD(wParam))
			{
			case SB_LINEUP:
				si.nPos -= 10;
				break;
			case SB_LINEDOWN:
				si.nPos += 10;
				break;
			case SB_PAGEUP:
				si.nPos -= si.nPage;
				break;
			case SB_PAGEDOWN:
				si.nPos += si.nPage;
				break;
			case SB_THUMBTRACK:
				si.nPos = si.nTrackPos;
				break;
			}

			si.nPos = max(0, min(si.nPos, si.nMax - (int)si.nPage + 1));

			SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

			if (Main->ExploreListScrollPos != si.nPos)
			{
				ScrollWindow(hWnd, 0, Main->ExploreListScrollPos - si.nPos, NULL, NULL);
				UpdateWindow(hWnd);

				Main->ExploreListScrollPos = si.nPos;
			}
		}
		return 0L;
	}
	case WM_SIZE:
	{
		return 0L;
	}
	case EM_UPDATEEXDATA:
	{
		if (Main->OnUpdating == false)
		{
			Main->TotalHeight = 0;
			Main->OnUpdating = true;
			for (size_t id = 0; id < Main->ObjectBoxList.size(); id++)
			{
				if (Main->ObjectBoxList.at(id))
				{
					DestroyWindow(Main->ObjectBoxList.at(id)->hWndCore);
					delete Main->ObjectBoxList.at(id);
					Main->ObjectBoxList.at(id) = nullptr;
				}
			};
			Main->ObjectBoxList.clear();
			Main->ObjectBoxList.shrink_to_fit();

			DebuggerExploreBox* SceneBox = new DebuggerExploreBox();
			SceneBox->hWndCore = CreateWindowExW(0L, L"ExploreBoxDebugger", L"Scene", WS_VISIBLE | WS_CHILD, 1, Main->TotalHeight, WindowSrc.right, 15, hWnd, NULL, NULL, (void*)SceneBox);
			SendMessageW(SceneBox->hWndCore, WM_SETFONT, WPARAM(DefaultFont), TRUE); Main->TotalHeight += 15;
			SceneBox->Alias = L"Current Scene";
			SceneBox->ReferenceObject = Main->ReferenceProgram->CurrentScene;
			Main->ObjectBoxList.push_back(SceneBox);

			std::vector<Object*> CurrentSceneObjectList = Main->ReferenceProgram->CurrentScene->GetChildren();
			for (unsigned long i = 0; i < CurrentSceneObjectList.size(); i++)
			{
				DebuggerExploreBox* Box = new DebuggerExploreBox();
				Box->hWndCore = CreateWindowExW(0L, L"ExploreBoxDebugger", CurrentSceneObjectList.at(i)->GetName().c_str(), WS_VISIBLE | WS_CHILD, 6, Main->TotalHeight, WindowSrc.right, 15, hWnd, NULL, NULL, (void*)Box);
				SendMessageW(Box->hWndCore, WM_SETFONT, WPARAM(DefaultFont), TRUE);
				Box->ReferenceObject = CurrentSceneObjectList.at(i);
				Main->TotalHeight += 15;
				Main->ObjectBoxList.push_back(Box);
			}

			DebuggerExploreBox* GUIBox = new DebuggerExploreBox();
			GUIBox->hWndCore = CreateWindowExW(0L, L"ExploreBoxDebugger", L"GUI", WS_VISIBLE | WS_CHILD, 1, Main->TotalHeight, WindowSrc.right, 15, hWnd, NULL, NULL, (void*)GUIBox);
			SendMessageW(GUIBox->hWndCore, WM_SETFONT, WPARAM(DefaultFont), TRUE); Main->TotalHeight += 15;
			GUIBox->Alias = L"GUIs";
			GUIBox->ReferenceObject = Main->ReferenceProgram->GameHierachy.GUI;
			Main->ObjectBoxList.push_back(GUIBox);

			std::vector<Object*> GUIList = Main->ReferenceProgram->GameHierachy.GUI->GetChildren();
			for (unsigned long i = 0; i < GUIList.size(); i++)
			{
				DebuggerExploreBox* Box = new DebuggerExploreBox();
				Box->hWndCore = CreateWindowExW(0L, L"ExploreBoxDebugger", GUIList.at(i)->GetName().c_str(), WS_VISIBLE | WS_CHILD, 6, Main->TotalHeight, WindowSrc.right, 15, hWnd, NULL, NULL, (void*)Box);
				SendMessageW(Box->hWndCore, WM_SETFONT, WPARAM(DefaultFont), TRUE);
				Box->ReferenceObject = GUIList.at(i);
				Main->TotalHeight += 15;
				Main->ObjectBoxList.push_back(Box);
			}

			DebuggerExploreBox* AssetsBox = new DebuggerExploreBox();
			AssetsBox->hWndCore = CreateWindowExW(0L, L"ExploreBoxDebugger", L"Assets", WS_VISIBLE | WS_CHILD, 1, Main->TotalHeight, WindowSrc.right, 15, hWnd, NULL, NULL, (void*)AssetsBox);
			SendMessageW(AssetsBox->hWndCore, WM_SETFONT, WPARAM(DefaultFont), TRUE); Main->TotalHeight += 15;
			AssetsBox->Alias = L"";
			AssetsBox->ReferenceObject = Main->ReferenceProgram->GameHierachy.Assets;
			Main->ObjectBoxList.push_back(AssetsBox);

			if (Main->TotalHeight > WindowSrc.bottom)
			{
				si.cbSize = sizeof(si);
				si.fMask = SIF_RANGE | SIF_PAGE;
				si.nMin = 0;
				si.nMax = Main->TotalHeight;
				si.nPage = WindowSrc.bottom;
				si.nPos = Main->ExploreListScrollPos;
				SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
			}

			Main->OnUpdating = false;
		}
		return 0L;
	}
	case EM_SELECTOBJECTBOX:
	{
		return SendMessageW(Main->ReferencePropertiesTab->WindowCore, PM_SELECTOBJECTBOX, wParam, lParam);
	}
	default:
		return DefWindowProcW(hWnd, hMsg, wParam, lParam);
	}
}

LRESULT CALLBACK ProgramDebugger::ExploreBoxProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam)
{
	SCROLLINFO si = {};
	DebuggerExploreBox* Main = (DebuggerExploreBox*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	HFONT DefaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	RECT WindowSrc;
	GetClientRect(hWnd, &WindowSrc);

	switch (hMsg)
	{
	case WM_CREATE:
	{
		CREATESTRUCT* CreateParameter = reinterpret_cast<CREATESTRUCT*>(lParam);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(CreateParameter->lpCreateParams));
		InvalidateRect(hWnd, NULL, TRUE);
		return 0L;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		Gdiplus::Graphics graphics(hdc);

		graphics.Clear(Gdiplus::Color(255, 242, 242, 242));

		Gdiplus::FontFamily fontFamily(L"Segoe UI");
		Gdiplus::Font font(&fontFamily, 14, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::SolidBrush textbrush(Gdiplus::Color(255, 0, 0, 0));
		Gdiplus::RectF layoutRect(0, 0, static_cast<float>(WindowSrc.right), static_cast<float>(WindowSrc.bottom));

		Gdiplus::StringFormat stringFormat = {};
		stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		if (Main->Alias != L"")
		{
			graphics.DrawString(Main->Alias, lstrlenW(Main->Alias), &font, layoutRect, &stringFormat, &textbrush);
		}
		else
		{
			graphics.DrawString(Main->ReferenceObject->GetName().c_str(), Main->ReferenceObject->GetName().size(), &font, layoutRect, &stringFormat, &textbrush);
		}

		EndPaint(hWnd, &ps);
		return 0L;
	}
	case WM_LBUTTONDOWN:
	{
		SendMessageW(GetParent(hWnd), EM_SELECTOBJECTBOX, 0, reinterpret_cast<LPARAM>(Main));
		return 0L;
	}
	default:
		return DefWindowProcW(hWnd, hMsg, wParam, lParam);
	}
}

ProgramDebugger::ProgramDebugger()
{

}

void ProgramDebugger::Start()
{
	this->wndclass.cbSize = sizeof(WNDCLASSEXW);
	this->wndclass.lpszClassName = L"DebuggerIndependent1";
	this->wndclass.lpfnWndProc = &DebuggerProcedure;
	this->wndclass.style = CS_HREDRAW | CS_VREDRAW;
	this->wndclass.hCursor = LoadCursorW(0, IDC_ARROW);
	this->wndclass.hIcon = LoadIconW(0, IDI_APPLICATION);
	this->wndclass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	this->wndclass.hInstance = hInst;

	ExploreListClass.cbSize = sizeof(WNDCLASSEXW);
	ExploreListClass.lpszClassName = L"ExploreListDebugger";
	ExploreListClass.lpfnWndProc = &ExploreListProcedure;
	ExploreListClass.style = CS_HREDRAW | CS_VREDRAW;
	ExploreListClass.hCursor = LoadCursorW(0, IDC_ARROW);
	ExploreListClass.hIcon = LoadIconW(0, IDI_APPLICATION);
	ExploreListClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	ExploreListClass.hInstance = hInst;

	ExploreBox.cbSize = sizeof(WNDCLASSEXW);
	ExploreBox.lpszClassName = L"ExploreBoxDebugger";
	ExploreBox.lpfnWndProc = &ExploreBoxProcedure;
	ExploreBox.style = CS_HREDRAW | CS_VREDRAW;
	ExploreBox.hCursor = LoadCursorW(0, IDC_ARROW);
	ExploreBox.hIcon = LoadIconW(0, IDI_APPLICATION);
	ExploreBox.hbrBackground = (HBRUSH)COLOR_WINDOW;
	ExploreBox.hInstance = hInst;

	PropertiesTabClass.cbSize = sizeof(WNDCLASSEXW);
	PropertiesTabClass.lpszClassName = L"PropertiesTabDebugger";
	PropertiesTabClass.lpfnWndProc = &PropertiesTabProcedure;
	PropertiesTabClass.style = CS_HREDRAW | CS_VREDRAW;
	PropertiesTabClass.hCursor = LoadCursorW(0, IDC_ARROW);
	PropertiesTabClass.hIcon = LoadIconW(0, IDI_APPLICATION);
	PropertiesTabClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	PropertiesTabClass.hInstance = hInst;

	RegisterClassExW(&this->wndclass);
	RegisterClassExW(&ExploreListClass);
	RegisterClassExW(&ExploreBox);
	RegisterClassExW(&PropertiesTabClass);

	HFONT DefaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	this->GUI.m_ExploreList = new DebuggerExploreList();
	this->GUI.m_ExploreList->ReferenceProgram = this->ReferenceProgram;
	this->GUI.m_PropertiesTab = new PropertiesTab();
	this->GUI.m_PropertiesTab->ReferenceProgram = this->ReferenceProgram;

	this->Window = new tn::Window();
	tn::StartingWindowDescription StartingDebuggerDesc = {};
	StartingDebuggerDesc.SizeX = 600;
	StartingDebuggerDesc.SizeY = 480;
	StartingDebuggerDesc.hInstance = hInst;
	StartingDebuggerDesc.WindowClassType = wndclass.lpszClassName;
	StartingDebuggerDesc.WindowName = L"29_tringuyen's Debugger";
	this->Window->Create(&StartingDebuggerDesc);
	SendMessageW(this->Window->GetHWND(), WM_SETFONT, WPARAM(DefaultFont), TRUE);
	this->Window->SetVoidAttributesInAReinterpretedByteData(this);

	this->Menu.MainMenu = new tn::Menu();
	this->Menu.MainMenu->UI = this->Window;
	this->Menu.MainMenu->Initialize();

	tn::MenuElement MainMenuElements[] = {
		{MF_STRING, 1, L"Tools"},
		{MF_STRING, 2, L"Exit"},
	};

	this->Menu.MainMenu->Append(MainMenuElements, 2);
	this->Menu.MainMenu->SetVisible();

	this->Window->Show(tn::WindowVisibleTypes::Show);

	RECT WinRt = {};
	GetClientRect(this->Window->GetHWND(), &WinRt);

	this->GUI.m_PropertiesTab->WindowCore = CreateWindowExW(0, L"PropertiesTabDebugger", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
		0, 0,
		(int)(static_cast<float>(WinRt.right) - (static_cast<float>(WinRt.right) / 5)),
		(int)(static_cast<float>(WinRt.bottom) * 0.5f),
		this->Window->GetHWND(), nullptr, nullptr, this->GUI.m_PropertiesTab);

	SendMessageW(GUI.m_PropertiesTab->WindowCore, WM_SETFONT, WPARAM(DefaultFont), TRUE);

	this->GUI.m_ExploreList->ReferencePropertiesTab = this->GUI.m_PropertiesTab;
	this->GUI.m_ExploreList->WindowCore = CreateWindowExW(0, L"ExploreListDebugger", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
		static_cast<float>(WinRt.right) - (static_cast<float>(WinRt.right) / 5), 0,
		(static_cast<float>(WinRt.right) / 5), static_cast<float>(WinRt.bottom) * 0.5f,
		this->Window->GetHWND(), nullptr, nullptr, this->GUI.m_ExploreList);

	SendMessageW(this->GUI.m_ExploreList->WindowCore, WM_SETFONT, WPARAM(DefaultFont), TRUE);

	this->GUI.LogListPosSize[0] = { 0, static_cast<float>(WinRt.bottom) * 0.5f };
	this->GUI.LogListPosSize[1] = { 
		static_cast<float>(WinRt.right),
		static_cast<float>(WinRt.bottom) - this->GUI.LogListPosSize[0].Y
	};

	GUI.LogList = CreateWindowExW(0, L"ListBox", L"", WS_VISIBLE | WS_CHILD | WS_VSCROLL,
		this->GUI.LogListPosSize[0].X, this->GUI.LogListPosSize[0].Y,
		this->GUI.LogListPosSize[1].X, this->GUI.LogListPosSize[1].Y,
		this->Window->GetHWND(), nullptr, nullptr, nullptr);

	SendMessageW(GUI.LogList, WM_SETFONT, WPARAM(DefaultFont), TRUE);
}

void ProgramDebugger::Stop()
{
	if (GUI.m_ExploreList != nullptr) delete GUI.m_ExploreList;
	DestroyWindow(GUI.LogList);

	UnregisterClassW(L"DebuggerIndependent1", hInst);
	UnregisterClassW(L"ExploreListDebugger", hInst);
	UnregisterClassW(L"ExploreBoxDebugger", hInst);
	UnregisterClassW(L"PropertiesTabDebugger", hInst);
	if (this->Menu.MainMenu)
	{
		delete this->Menu.MainMenu;
	}
	if (Window)
	{
		Window->Destroy();
		delete Window;
	}
	ReferenceProgram = nullptr;
}

void ProgramDebugger::SendLog(const char* Log)
{
	GetLocalTime(&LocalTime);

	std::string HourString;
	if (LocalTime.wHour < 10) HourString = "0" + std::to_string(LocalTime.wHour);
	else HourString = std::to_string(LocalTime.wHour);

	std::string MinuteString;
	if (LocalTime.wMinute < 10) MinuteString = "0" + std::to_string(LocalTime.wMinute);
	else MinuteString = std::to_string(LocalTime.wMinute);

	std::string SecondString;
	if (LocalTime.wSecond < 10) SecondString = "0" + std::to_string(LocalTime.wSecond);
	else SecondString = std::to_string(LocalTime.wSecond);

	std::string FullLog = "[" + HourString + ":" + MinuteString + ":" + SecondString + "; " + std::to_string(LocalTime.wDay) + "/" + std::to_string(LocalTime.wMonth) + "/" + std::to_string(LocalTime.wYear) + "]: " + Log;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring output_wstr = converter.from_bytes(FullLog);

	SendMessageW(GUI.LogList, LB_ADDSTRING, 0, (LPARAM)output_wstr.data());
}

void ProgramDebugger::SendLog(const wchar_t* Log)
{
	GetLocalTime(&this->LocalTime);

	std::wstring HourString;
	if (this->LocalTime.wHour < 10) HourString = L"0" + std::to_wstring(this->LocalTime.wHour);
	else HourString = std::to_wstring(LocalTime.wHour);

	std::wstring MinuteString;
	if (this->LocalTime.wMinute < 10) MinuteString = L"0" + std::to_wstring(this->LocalTime.wMinute);
	else MinuteString = std::to_wstring(LocalTime.wMinute);

	std::wstring SecondString;
	if (this->LocalTime.wSecond < 10) SecondString = L"0" + std::to_wstring(this->LocalTime.wSecond);
	else SecondString = std::to_wstring(this->LocalTime.wSecond);

	std::wstring FullLog = L"[" + HourString + L":" + MinuteString + L":" + SecondString + L"; " + std::to_wstring(this->LocalTime.wDay) + L"/" + std::to_wstring(this->LocalTime.wMonth) + L"/" + std::to_wstring(this->LocalTime.wYear) + L"]: " + Log;
	SendMessageW(GUI.LogList, LB_ADDSTRING, 0, (LPARAM)FullLog.data());
}

void ProgramDebugger::Update()
{
	SendMessageW(GUI.m_ExploreList->WindowCore, EM_UPDATEEXDATA, 0, 0);
}

ProgramDebugger::~ProgramDebugger()
{

}
