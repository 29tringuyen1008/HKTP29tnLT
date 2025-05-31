#include "Windows.h"

void tn::MenuElement::AppendChild(Menu* Child) {
	this->ID = reinterpret_cast<unsigned int>(Child->hMenu); //UINT_PTR = unsigned int
}

tn::Menu::Menu() {
	
}

bool tn::Menu::Initialize() {
	this->hMenu = CreateMenu();
	if (hMenu != nullptr) return true; else return false;
}

void tn::Menu::Append(MenuElement Element) {
	AppendMenuW(this->hMenu, Element.Type, Element.ID, Element.Text);
}

void tn::Menu::Append(MenuElement* ListOfElements, int NumberOfElements) {
	if (NumberOfElements > 0) {
		for (int i = 0; i < NumberOfElements; i++) {
			AppendMenuW(this->hMenu, ListOfElements[i].Type, ListOfElements[i].ID, ListOfElements[i].Text);
			//AppendMenuW(hMenu, MF_STRING, NULL, L"File");
		}
	}
}

void tn::Menu::SetVisible() {
	SetMenu(UI->hWnd, this->hMenu);
}

void tn::Menu::Release() {
	DestroyMenu(hMenu);
}