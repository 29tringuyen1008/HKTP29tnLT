#pragma once
#include "Physics.h"
#include <Windows.h>

namespace tn {
	constexpr const wchar_t* Default_Window_Class_Name = L"29TN_WIN32";

	enum class WindowVisibleTypes {
		Show = 0,
		Hide = 1,
	};

	class Window; class Menu;
	struct MenuElement {
		unsigned int Type;
		unsigned int ID;
		const wchar_t* Text;
		void AppendChild(Menu* Child);
	};

	class Menu {
	public:
		Window* UI = nullptr;

		Menu();

		bool Initialize();
		void Append(MenuElement Element);
		void Append(MenuElement* ListOfElements, int NumberOfElements);
		void Release();
		void SetVisible();
	private:
		friend struct MenuElement;
		HMENU hMenu = nullptr;            // top-level menu 
		HMENU hMenuTrackPopup = nullptr;  // shortcut menu 
	};

	struct StartingWindowDescription
	{
		const wchar_t* WindowName = L""; //OPTIONAL
		Window* Parent; //OPTIONAL
		unsigned int SizeX = 1280;
		unsigned int SizeY = 720;
		int PositionX = 50; //Initial position X
		int PositionY = 50; //Initial position Y
		void* OtherAttributes = nullptr; //OPTIONAL
		DWORD WindowExternalStyle = 0L;
		LPCWSTR WindowClassType = L"Default_Window_Class_Name";
		DWORD WindowStyle = WS_OVERLAPPEDWINDOW;
		HMENU hMenu = nullptr; //OPTIONAL 
		HINSTANCE hInstance = nullptr; //OPTIONAL
	};

	class Window {
	public:
		Window();
		~Window();

		bool Create(StartingWindowDescription* TargetDescription);
		StartingWindowDescription GetStartingDescription();

		bool Show(WindowVisibleTypes WindowVisibleType);
		bool Destroy();
		bool IsPresent();
		bool RunProcedure(unsigned int Message, void* OtherAttributes);
		bool ProcessMessage(int* Event = nullptr);

		void SetUIParent(Window* ParentWindow);
		Window* GetUIParent();

		void Move(Vector2 NewPosition);
		void Move(Vector2 NewPosition, Vector2 NewSize);

		bool (*CustomProcedure) (Window* ProcedureWindow, unsigned int Message, void* OtherAttributes);
		bool UseWindowDataAsClass = false; //OPTIONAL
		bool ContinueAfterDestroyed = false;

		const HWND GetHWND() const;
		HWND GetHWND();
		const HMENU GetHMENU() const;
		HMENU GetHMENU();
		const HINSTANCE GetHINSTANCE() const;
		HINSTANCE GetHINSTANCE();

		//Set void attributes in a reinterpreted data.
		bool SetVoidAttributesInAReinterpretedByteData(void* Attribites);

		//Get void attributes from a reinterpreted data.
		void* GetVoidAttributesFromAReinterpretedByteData();
	private:
		friend class GraphicsDevice;
		friend class SwapChain;
		friend class Menu;
		friend class Audio;

		Window* Parent = nullptr;
		StartingWindowDescription m_SWD = {};
		void* StoredAttributes = nullptr;

		HWND hWnd;
	};
}