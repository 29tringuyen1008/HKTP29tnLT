#include "Windows.h"
#include <iostream>

tn::Window::Window() {
	this->hWnd = nullptr;
	this->CustomProcedure = nullptr;

	this->StoredAttributes = nullptr;
}

bool tn::Window::Create(StartingWindowDescription* TargetDescription) {
	m_SWD = (*TargetDescription);
	bool result = false;

	if (this->hWnd == nullptr)
	{
		HWND parent_c = nullptr;
		if (TargetDescription->Parent != nullptr) {
			this->Parent = TargetDescription->Parent;
			parent_c = TargetDescription->Parent->hWnd;
		}

		this->hWnd = CreateWindowExW(TargetDescription->WindowExternalStyle, TargetDescription->WindowClassType, TargetDescription->WindowName,
			TargetDescription->WindowStyle, TargetDescription->PositionX, TargetDescription->PositionY,
			TargetDescription->SizeX, TargetDescription->SizeY, parent_c, TargetDescription->hMenu, TargetDescription->hInstance,
			TargetDescription->OtherAttributes);
		if (hWnd != nullptr) {
			result = true;
		}
		else {
			//std::cout << GetLastError() << std::endl;
			result = false;
		}

		if (UseWindowDataAsClass == true) {
			this->StoredAttributes = this;
			this->SetVoidAttributesInAReinterpretedByteData(StoredAttributes);
		}
		else {
			this->StoredAttributes = TargetDescription->OtherAttributes;
		}
	}

	return result;
}

tn::StartingWindowDescription tn::Window::GetStartingDescription()
{
	return this->m_SWD;
}

bool tn::Window::SetVoidAttributesInAReinterpretedByteData(void* Attribites) {
	if (SetWindowLongPtrW(this->hWnd, GWLP_USERDATA, (LONG_PTR)Attribites) == 0)
	{
		if (GetLastError() != 0) {
			return false;
		}
	}

	return true;
}

void* tn::Window::GetVoidAttributesFromAReinterpretedByteData() {
	void* result = (void*)GetWindowLongPtrW(this->hWnd, GWLP_USERDATA);
	return result;
}

bool tn::Window::Show(WindowVisibleTypes WindowVisibleType) {
	bool result = false;
	int numbool = 1;

	if (WindowVisibleType == WindowVisibleTypes::Show) {
		numbool = ShowWindow(this->hWnd, SW_SHOW);;
	}
	else if (WindowVisibleType == WindowVisibleTypes::Hide) {
		numbool = ShowWindow(this->hWnd, SW_HIDE);;
	}

	if (numbool == 0) {
		result = true;
	}
	else {
		result = false;
	}

	return result;
}

bool tn::Window::Destroy()
{
	BOOL result = DestroyWindow(this->hWnd);
	this->hWnd = nullptr;
	if (result == FALSE) {
		return false;
	}
	return true;
}

bool tn::Window::IsPresent()
{
	return (bool)IsWindow(this->hWnd);
}

bool tn::Window::RunProcedure(unsigned int Message, void* OtherAttributes) {
	if (CustomProcedure != nullptr) {
		return CustomProcedure(this, Message, OtherAttributes);
	}
	else {
		return false;
	}
}

bool tn::Window::ProcessMessage(int* Event) {
	MSG msg = {};
	while (PeekMessageW(&msg, this->hWnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

		if (Event != nullptr) {
			if (msg.hwnd == this->hWnd) {
				*Event = msg.message;
			}
			else {
				*Event = 0;
			}
		}
		if (IsWindow(this->hWnd) == false) {
			if (this->ContinueAfterDestroyed == false) {
				return false;
			}
		}
	}

	return true;
}

void tn::Window::SetUIParent(Window* ParentWindow)
{
	if (ParentWindow != nullptr)
	{
		SetParent(this->hWnd, ParentWindow->hWnd);
		this->Parent = ParentWindow;
	}
	else
	{
		this->Parent = nullptr;
	}
}

tn::Window* tn::Window::GetUIParent()
{
	return this->Parent;
}

void tn::Window::Move(tn::Vector2 NewPosition)
{
	if (hWnd != nullptr)
	{
		RECT rt = {};
		GetClientRect(hWnd, &rt);
		MoveWindow(hWnd, NewPosition.X, NewPosition.Y, rt.right, rt.bottom, TRUE);
	}
}

void tn::Window::Move(Vector2 NewPosition, Vector2 NewSize)
{
	if (hWnd != nullptr)
	{
		MoveWindow(hWnd, NewPosition.X, NewPosition.Y, NewSize.X, NewSize.Y, TRUE);
	}
}

const HWND tn::Window::GetHWND() const
{
	return hWnd;
}

HWND tn::Window::GetHWND() {
	return hWnd;
}

const HMENU tn::Window::GetHMENU() const
{
	return GetMenu(hWnd);
}

HMENU tn::Window::GetHMENU()
{
	return GetMenu(hWnd);
}

const HINSTANCE tn::Window::GetHINSTANCE() const
{
	return (HINSTANCE)GetWindowLongPtrW(this->hWnd, GWLP_HINSTANCE);
}

HINSTANCE tn::Window::GetHINSTANCE()
{
	return (HINSTANCE)GetWindowLongPtrW(this->hWnd, GWLP_HINSTANCE);
}

tn::Window::~Window() {
	DestroyWindow(hWnd);
	hWnd = nullptr;
}

///////////////////////////////////////////////////////////////