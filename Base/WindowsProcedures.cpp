#include "Base.h"
#include "GameObject.h"
#include <WinUser.h>

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam)
{
	Program* ReferenceProgram = static_cast<Program*>(reinterpret_cast<void*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA)));
	if (ReferenceProgram == nullptr)
	{
		return DefWindowProcW(hWnd, hMsg, wParam, lParam);
	}

	switch (hMsg)
	{
	case WM_GETMINMAXINFO: {
		MINMAXINFO* mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize.x = 600;
		mmi->ptMinTrackSize.y = 480;
		return 0L;
	}
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0L;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0L;
	case WM_MOUSEWHEEL:
		ReferenceProgram->Mouse->MouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		return 0L;
	case WM_KEYDOWN:
		if (ReferenceProgram == nullptr)
		{
			return 0L;
		}
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			if (ReferenceProgram->GameState == GameStates::Gameplay)
			{
				if (ReferenceProgram->GameplayState != GameplayStates::End)
				{
					if (ReferenceProgram->GameplayState == GameplayStates::Run)
					{
						ReferenceProgram->PauseGameplay();
					}
					else
					{
						ReferenceProgram->ResumeGameplay();
					}
				}
			}
			break;
		}
		}

		if (ReferenceProgram->GameState == GameStates::MapEditor or (ReferenceProgram->GameState == GameStates::Gameplay and ReferenceProgram->GameplayState == GameplayStates::Run))
		{
			switch (wParam)
			{
			case 'W':
			case 'w':
				ReferenceProgram->KeyPressed.KeyW = true;
				break;
			case 'A':
			case 'a':
				ReferenceProgram->KeyPressed.KeyA = true;
				break;
			case 'S':
			case 's':
				ReferenceProgram->KeyPressed.KeyS = true;
				break;
			case 'D':
			case 'd':
				ReferenceProgram->KeyPressed.KeyD = true;
				break;
			case 'E':
			case 'e':
				ReferenceProgram->KeyPressed.KeyE = true;
				break;
			case 'Q':
			case 'q':
				ReferenceProgram->KeyPressed.KeyQ = true;
				break;
			case VK_TAB:
				ReferenceProgram->KeyPressed.KeyTab = true;
				break;
			case VK_SPACE:
				ReferenceProgram->KeyPressed.Space = true;
				break;
			}
		}
		else
		{
			ReferenceProgram->KeyPressed.KeyW = false;
			ReferenceProgram->KeyPressed.KeyA = false;
			ReferenceProgram->KeyPressed.KeyS = false;
			ReferenceProgram->KeyPressed.KeyD = false;
			ReferenceProgram->KeyPressed.KeyE = false;
			ReferenceProgram->KeyPressed.KeyQ = false;
			ReferenceProgram->KeyPressed.KeyTab = false;
			ReferenceProgram->KeyPressed.Space = false;
		}
		return 0L;
	case WM_KEYUP:
		if (ReferenceProgram == nullptr)
		{
			return 0L;
		}
		switch (wParam)
		{
		case 'W':
			ReferenceProgram->KeyPressed.KeyW = false;
			break;
		case 'A':
			ReferenceProgram->KeyPressed.KeyA = false;
			break;
		case 'S':
			ReferenceProgram->KeyPressed.KeyS = false;
			break;
		case 'D':
			ReferenceProgram->KeyPressed.KeyD = false;
			break;
		case 'Q':
			ReferenceProgram->KeyPressed.KeyQ = false;
			break;
		case 'E':
			ReferenceProgram->KeyPressed.KeyE = false;
			break;
		case VK_TAB:
			ReferenceProgram->KeyPressed.KeyTab = false;
			break;
		case VK_SPACE:
			ReferenceProgram->KeyPressed.Space = false;
			break;
		case VK_F1:
		{
			if (ReferenceProgram->Debug.ShowDebug == false)
				ReferenceProgram->Debug.ShowDebug = true;
			else ReferenceProgram->Debug.ShowDebug = false;
			break;
		}
		}
		return 0L;
	case WM_MOUSEMOVE:
		ReferenceProgram->Mouse->Position.X = GET_X_LPARAM(lParam);
		ReferenceProgram->Mouse->Position.Y = GET_Y_LPARAM(lParam);
		return 0L;
	case WM_LBUTTONDOWN:
		ReferenceProgram->Mouse->LeftMouseDown = true;
		ReferenceProgram->Mouse->LeftMouseHolded = true;
		ReferenceProgram->Mouse->LeftMouseUp = false;

		{
			TRACKMOUSEEVENT tme = { 0 };
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = hWnd;
			TrackMouseEvent(&tme);
		}
		return 0L;
	case WM_LBUTTONUP:
	{
		ReferenceProgram->Mouse->LeftMouseDown = false;
		ReferenceProgram->Mouse->LeftMouseHolded = false;
		ReferenceProgram->Mouse->LeftMouseUp = true;
		return 0L;
	}
	case WM_RBUTTONDOWN:
		ReferenceProgram->Mouse->RightMouseDown = true;
		ReferenceProgram->Mouse->RightMouseHolded = true;
		ReferenceProgram->Mouse->RightMouseUp = false;

		{
			TRACKMOUSEEVENT tme = { 0 };
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = hWnd;
			TrackMouseEvent(&tme);
		}
		return 0L;
	case WM_RBUTTONUP:
	{
		ReferenceProgram->Mouse->RightMouseDown = false;
		ReferenceProgram->Mouse->RightMouseHolded = false;
		ReferenceProgram->Mouse->RightMouseUp = true;
		return 0L;
	}
	case WM_KILLFOCUS:
	case WM_MOUSELEAVE:
		ReferenceProgram->Mouse->LeftMouseDown = false;
		ReferenceProgram->Mouse->LeftMouseHolded = false;
		ReferenceProgram->Mouse->LeftMouseUp = false;
		ReferenceProgram->Mouse->RightMouseDown = false;
		ReferenceProgram->Mouse->RightMouseHolded = false;
		ReferenceProgram->Mouse->RightMouseUp = false;
		return 0L;
	default:
		ReferenceProgram->Mouse->MouseWheelDelta = 0.0f;
		ReferenceProgram->Mouse->LeftMouseUp = false;
		//ReferenceProgram->Mouse->LeftMouseDown = false;
		//ReferenceProgram->Mouse->LeftMouseHolded = false;
		//ReferenceProgram->Mouse->LeftMouseUp = false;
		//ReferenceProgram->WhichUI_MouseIsTouching = nullptr;
		return DefWindowProcW(hWnd, hMsg, wParam, lParam);
	}

	return 0L;
}

LRESULT CALLBACK SettingBoxProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam)
{
	switch (hMsg)
	{
	case WM_CREATE:
		return 0L;
	case WM_COMMAND:
		return 0L;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0L;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0L;
	default:
		return DefWindowProcW(hWnd, hMsg, wParam, lParam);
	}
	return 0L;
}

LRESULT CALLBACK ExtendedEditProc(HWND hEdit, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	switch (msg) {
	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			SetFocus(nullptr);
			return 0L;
		}
		break;

	case WM_KILLFOCUS:
		HideCaret(hEdit);
		return 0L;

	case WM_SETFOCUS:
		ShowCaret(hEdit);
		return 0L;

	case WM_DESTROY:
		RemoveWindowSubclass(hEdit, ExtendedEditProc, uIdSubclass);
	}
	return DefSubclassProc(hEdit, msg, wParam, lParam);
}

#define LangSelectionBeforeEnterProc_ID_COMBOBOX 1001
#define LangSelectionBeforeEnterProc_ID_OK       1002
#define LangSelectionBeforeEnterProc_ID_CANCEL   1003
#define WM_COMBOBOX_LANG_CHANGED (WM_USER + 1)

LRESULT CALLBACK SelectionLangMessageBox(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam)
{
	Program* RefProgram = reinterpret_cast<Program*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	if (RefProgram)
	{
		switch (hMsg)
		{
		case WM_GETMINMAXINFO: {
			MINMAXINFO* mmi = (MINMAXINFO*)lParam;
			mmi->ptMinTrackSize.x = 200;
			mmi->ptMinTrackSize.y = 125;
			mmi->ptMaxTrackSize.x = 200;
			mmi->ptMaxTrackSize.y = 125;
			return 0L;
		}
		case WM_LBUTTONDOWN:
			UpdateWindow(hWnd);
			return 0L;
		case WM_CREATE:
			return 0L;
		case WM_COMMAND:
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE:
			{
				if (LOWORD(wParam) == LangSelectionBeforeEnterProc_ID_COMBOBOX)
				{
					int index = SendMessageW(RefProgram->SelectLang.hSelection, CB_GETCURSEL, 0, 0);
					wchar_t text[3];
					SendMessageW(RefProgram->SelectLang.hSelection, CB_GETLBTEXT, index, (LPARAM)text);
					RefProgram->SelectLang.PreselectedLang = text;
				}
			}
			case CBN_DROPDOWN:
				return 0L;
			case CBN_KILLFOCUS:
				SetFocus(hWnd);
				SendMessageW(hWnd, 0, 0, 0);
				return 0L;
			}

			switch (LOWORD(wParam))
			{
			case 1:
				SendMessageW(hWnd, WM_CLOSE, 0, 0);
				return 0L;
			}

			return DefWindowProcW(hWnd, hMsg, wParam, lParam);
			//SendMessageW(hWnd, WM_CLOSE, 0, 0);
		case WM_COMBOBOX_LANG_CHANGED:
		{
			HWND hCombo = (HWND)lParam;
			int selected = (int)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			if (selected != CB_ERR) {
				wchar_t lang[32] = {};
				SendMessage(hCombo, CB_GETLBTEXT, selected, (LPARAM)lang);
				MessageBoxW(hWnd, lang, L"Selected language", MB_OK);
			}
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0L;
		case WM_CLOSE:
		{
			int index = SendMessageW(RefProgram->SelectLang.hSelection, CB_GETCURSEL, 0, 0);
			wchar_t text[3] = L"";
			SendMessageW(RefProgram->SelectLang.hSelection, CB_GETLBTEXT, index, (LPARAM)text);
			RefProgram->SelectLang.PreselectedLang = text;

			RefProgram->CurrentLang = RefProgram->SelectLang.PreselectedLang;
			(void)RefProgram->GetLangIndex(&RefProgram->CurrentLangIndex, RefProgram->CurrentLang);
			DestroyWindow(hWnd);
			return 0L;
		}
		default:
			return DefWindowProcW(hWnd, hMsg, wParam, lParam);
		}
	}
	else
	{
		switch (hMsg)
		{
		case WM_NCCREATE:
		{
			if (CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam))
			{
				if (RefProgram = static_cast<Program*>(pCreate->lpCreateParams))
				{
					tn::StartingWindowDescription CurrentWndDesc = RefProgram->SelectLang.Base->GetStartingDescription();

					HFONT hModernFont = CreateFontW(
						-MulDiv(9, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72), // height for 9pt
						0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
						DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
						L"Segoe UI"
					);
					if (hModernFont == nullptr) hModernFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
					SendMessageW(hWnd, WM_SETFONT, WPARAM(hModernFont), TRUE);

					RefProgram->SelectLang.hSelection = CreateWindowExW(0L, WC_COMBOBOXW, nullptr, WS_BORDER| WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_HASSTRINGS,
						(CurrentWndDesc.SizeX / 2) - 25, 100, 150, 200, hWnd, (HMENU)LangSelectionBeforeEnterProc_ID_COMBOBOX, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hWnd, GWLP_HINSTANCE)), nullptr);
					SendMessage(RefProgram->SelectLang.hSelection, (UINT)CB_ADDSTRING, 0, (LPARAM)L"en");
					SendMessage(RefProgram->SelectLang.hSelection, (UINT)CB_ADDSTRING, 0, (LPARAM)L"vi");
					SendMessage(RefProgram->SelectLang.hSelection, (UINT)CB_SETCURSEL, (WPARAM)1, (LPARAM)0);
					SendMessageW(RefProgram->SelectLang.hSelection, WM_SETFONT, WPARAM(hModernFont), TRUE);

					RefProgram->SelectLang.OKButton = CreateWindowExW(0L, L"BUTTON", L"OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, CurrentWndDesc.SizeX - 45, CurrentWndDesc.SizeY + 12, 75, 25, hWnd, (HMENU)1, nullptr, nullptr);
					SendMessageW(RefProgram->SelectLang.OKButton, WM_SETFONT, WPARAM(hModernFont), TRUE);

					SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(RefProgram));
				}
			}
			break;
		}
		}

		return DefWindowProcW(hWnd, hMsg, wParam, lParam);
	}
	return 0L;
}