#pragma once
#include "../Core/Windows.h"
#include "../Core/Graphics.h"

#ifdef TN_DEBUG
#define SEND_DEBUG_LOG(Debugger, Message) if (Debugger != nullptr) Debugger->SendLog(Message)
#else
#define SEND_DEBUG_LOG(Debugger, Message) 
#endif

#define EM_UPDATEEXDATA 1256
#define EM_SELECTOBJECTBOX 1257

#define PM_SELECTOBJECTBOX 2256

class Program;
class Object;
struct DebuggerParameter
{
	int Args = 0;
	char** Argv = nullptr;
	Program* ReferenceProgram = nullptr;
};

class PropertiesTab
{
public:
	int TotalHeight = 0;

	Program* ReferenceProgram = nullptr;

	HWND WindowCore = nullptr;
	std::vector<HWND> HWNDList;
};

class DebuggerExploreBox
{
public:
	DebuggerExploreBox() = default;
	~DebuggerExploreBox() { 
		DestroyWindow(hWndCore); 
	};

	HWND hWndCore = nullptr;
	Object* ReferenceObject = nullptr;
	const wchar_t* Alias = L"";
};

class DebuggerExploreList
{
public:
	int TotalHeight = 0;
	int ExploreListScrollPos = 0;
	bool OnUpdating = false;

	DebuggerExploreList() = default;
	~DebuggerExploreList() {
		DestroyWindow(WindowCore);
	}

	PropertiesTab* ReferencePropertiesTab = nullptr;
	Program* ReferenceProgram = nullptr;

	HWND WindowCore = nullptr;
	std::vector<DebuggerExploreBox*> ObjectBoxList;
};

class ProgramDebugger
{
public:
	static LRESULT CALLBACK DebuggerProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK ExploreListProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK PropertiesTabProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK ExploreBoxProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam);
	ProgramDebugger();
	~ProgramDebugger();

	void Start();
	void Stop();
	void Update();

	void SendLog(const char* Log);
	void SendLog(const wchar_t* Log);

private:
	friend class Program;
	friend class DebuggerExploreList;

	HINSTANCE hInst = {};
	WNDCLASSEXW wndclass = {};
	WNDCLASSEXW ExploreListClass = {};
	WNDCLASSEXW ExploreBox = {};
	WNDCLASSEXW PropertiesTabClass = {};
	SYSTEMTIME LocalTime = {};

	struct
	{
		tn::Menu* MainMenu = nullptr;
	} Menu;

	struct
	{
		HWND LogList;
		tn::Vector2 LogListPosSize[2];
		DebuggerExploreList* m_ExploreList = nullptr;
		PropertiesTab* m_PropertiesTab = nullptr;
	} GUI;

	tn::Window* Window = nullptr;
	Program* ReferenceProgram = nullptr;
};

#include "Base.h"