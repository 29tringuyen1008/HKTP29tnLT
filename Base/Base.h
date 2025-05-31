#pragma once

#define PixelsOfTile 64
#include "../Core/Windows.h"
#include "../Core/Graphics.h"
#include "../Core/Audio.h"
#include "../Core/Misc.h"

#include "Lua/lua.hpp"

#include <time.h>
#include <chrono>
#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <gdiplus.h>
#include <thread>
#include <psapi.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "psapi.lib")

#ifdef _DEBUG
#define TN_DEBUG
#endif

#include "Lua29tnlibs.h"
#include "GameObject.h"
#include "Camera.h"
#include "Debugger.h"
#include "Animation.h"
#include "Task.h"
#include "../Core/Array.h"
#include "Item.h"
#include "Entities.h"

#include "GameplayRelatedObjects.h"

constexpr const wchar_t* TimesNewRomanFont = L"Times New Roman\0";
constexpr const wchar_t* VerdanaFont = L"Verdana\0";

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SettingBoxProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ExtendedEditProc(HWND hEdit, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK SelectionLangMessageBox(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam);

enum class GameStates
{
	MainMenu = 0,
	Gameplay = 1,
	MapEditor = 2,
};

enum class GameplayStates
{
	Undetermined = 0,
	Run = 1,
	Pause = 2,
	End = 3,
};

class Object;
class Sprite;
class Entity;
class Scene;
class UI_Frame;
class ProgramDebugger;
class DebuggerExploreList;

class SceneFolder;
class GUIFolder;
class AssetFolder;
class MouseObject;

struct Game
{
	SceneFolder* Scenes = nullptr;
	GUIFolder* GUI = nullptr;
	AssetFolder* Assets = nullptr;
	std::vector<tn::Texture*> Textures = {};
};

class Program
{
public:
	int Result = 0;
	tn::Window* MainWnd = nullptr;
	tn::GraphicsDevice* MainGraphicDevice = nullptr;
	tn::SoundFramework* MainSoundFramework = nullptr;
	Scene* MainMenuSceneRef = nullptr;
	GameStates GameState = GameStates::MainMenu;
	GameplayStates GameplayState = GameplayStates::Undetermined;

	Program();
	~Program();

	void Start();
	void PauseWhileProcessWindow(float Seconds);
	void LoadResources();
	void LoadGameItems();
	void LoadLuaLibraries();
	void LoadDebuggingClients();
	void Main();

	void PauseGameplay();
	void ResumeGameplay();
	void EndGameplay();

	void Stop();
	
	void LoadInventoryUI();
	void LoadSettingsUI();
	void SetupInitialLangSelection();
	void LoadObjectiveUI();
	void StartGameplayScript();
	void GiveItemToPlayer(const wchar_t* PrototypeItemName, const wchar_t* AltText = L"\0");
	void SetBuildMode(bool Set);
	EntityPrototype* FindEntity(const wchar_t* PrototypeEntityName);
	void PlaceEntity(EntityPrototype* prototype, tn::Vector2 Position);

	bool GetLangIndex(size_t* Output, const std::wstring& lang);
	bool GetLangByIndex(std::wstring* Output, const size_t& index);
	std::wstring GetLocaleText(const wchar_t* lang, const wchar_t* textname);

	void SetObjectiveMessage(std::wstring ObjectiveMsg);
	bool CreateMessageBox(const std::wstring& Caption, const std::wstring& Text, MessageBoxUI** OutputBody);
	
	bool CheckGDIPlusRun();

	tn::Texture* GetFirstLoadedTexture(std::wstring Name);

	void AddDebris(ProgramObject* Obj, float timetodestroy);
private:
	friend class ProgramObject;

	std::vector<const char*> ListofAudio = {
		"Intro", "MainMenu", "gui_click", "gui_close", "camera_shutter", "MetalWalking", "AsphaltWalking",
	};
	std::vector<const char*> ListofTexture = {
		"Title", "Player", "Camcorder", "WoodenBlock"
	};
	std::vector<const char*> ListofTileTexture = {
		"GrassTile", "ConcreteTile", "AsphaltTile"
	};

	friend LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam);
	friend LRESULT CALLBACK SelectionLangMessageBox(HWND hWnd, UINT hMsg, WPARAM wParam, LPARAM lParam);

	void UntouchUIsForMouse(ProgramObject* Obj);
	void CheckUIForMouse(ProgramObject* Obj, bool* IsChildTouched = nullptr);

	void CheckCollidersForMouse(ProgramObject* Obj);
	static void RenderObject(Program* m_Program, ProgramObject* obj);
	static void RenderUI(Program* m_Program, ProgramObject* obj, bool DescendentIsFolderRendering);

	struct {
		tn::ReferenceArray<ProgramObject> Queued = {};
		tn::ReferenceArray<ProgramObject> OnCleanup = {};
	} GarbageCollection;

	void AddObjectToGarbageCollection(ProgramObject* Obj);

	static DWORD CALLBACK MainScript(LPVOID ThreadParameter);
	static DWORD CALLBACK GameplayScript(LPVOID ThreadParameter);
	static DWORD CALLBACK GraphicEngineThreadFunction(LPVOID ThreadParameter);
	static DWORD CALLBACK TaskThreadFunction(LPVOID ThreadParameter);
public: 
	void AddAnimation(Animation* Animation);
	void SetCurrentScene(Scene* targetscene);
	Scene* GetSceneByName(std::wstring name);

	Game GameHierachy;
	Entity* Player = nullptr;
	struct SoundWhenSteppingOnTile
	{
		tn::Texture* TileTexture;
		tn::Sound* Sound;
	};
	std::vector<SoundWhenSteppingOnTile> TileWalkingSounds;

	std::vector<ItemDescription> ListOfItems;
	std::vector<EntityPrototype> ListOfEntities;
	struct {
		tn::NormalArray<InventoryItem*> CharInventoryItemSlots;
		tn::NormalArray<InventoryItem*> ActiveItemSlots;
	} Inventory;

	float PlayerMovementSpeed = 200.0f; //cm/s

	Camera* CurrentCamera = nullptr;
	Scene* CurrentScene = nullptr;
	SYSTEMTIME LocalTime = {};

	UI_Frame* WhichUI_MouseIsTouching = nullptr;
	UI_Frame* UI_UnderContinuousPress = nullptr;
	std::vector<luaL_Reg>ProgramEmbeddedFunctions;
	std::vector<Animation*> RefAnimations;
	std::vector<tn::Texture*> TileTextures;
	size_t CurrentLangIndex = 0;
	std::wstring CurrentLang = L"vi";

	struct LocaleTextA
	{
		LocaleTextA() : Textname(), Text() { };
		LocaleTextA(const std::wstring& _Textname, const std::wstring& _Text)
		{
			Textname = _Textname; Text = _Text;
		};
		LocaleTextA& operator=(const LocaleTextA& newLocaleTextA)
		{
			Textname = newLocaleTextA.Textname; Text = newLocaleTextA.Text;
			return *this;
		};

		std::wstring Textname = L"";
		std::wstring Text = L"";
	};

	struct LocaleA
	{
		LocaleA() : Lang(), Texts() {};
		LocaleA(std::wstring&& _Lang, tn::NormalArray<LocaleTextA>&& _Texts)
			: Lang(tn::MoveElement(_Lang)), Texts(tn::MoveElement(_Texts)) 
		{};
		LocaleA& operator=(const LocaleA& newlocaleA)
		{
			Lang = newlocaleA.Lang;
			Texts = newlocaleA.Texts;
			return *this;
		}

		std::wstring Lang = {};
		tn::NormalArray<LocaleTextA> Texts = {};
	};
	tn::NormalArray<LocaleA> Localization = {};

	struct {
		Task* RenderStepped = nullptr;
		Task* Heartbeat = nullptr;
	} CoreEvents;

	struct ISettingGUIs
	{
		MessageBoxUI* SettingFrame = nullptr;
		/**/ScrollingFrame* ScrollFrame = nullptr;
		/***/TextLabel* LanguageSettingText = nullptr;
		/***/TextButton* LanguageSettingButton = nullptr;
		/***/TextLabel* VolumeSettingText = nullptr;
		/***/TextButton* VolumeSettingButton = nullptr;
		/**/TextButton* CloseButtonOfSettingFrame = nullptr;
	} SettingsGUI;

	struct SelectLangA
	{
		tn::Window* Base = nullptr;
		HWND hSelection = nullptr;
		HWND OKButton = nullptr;
		int SelectedLangIndex = 0;
		wchar_t* PreselectedLang = nullptr;
	} SelectLang;
private:
	const int ToolboxItemSlots = 9;
	const int NumberOfActiveItemSlots = 3;
	size_t InventorySize = 9;
	tn::Vector2 InventoryFrameSize = { 600, 400 };
	tn::Vector2 InventorySlotFrameOffsetSize = { 45, 45 };
	float InventorySlotPosLimitX = 300.0f;
	float RechargeTimeStart = 0.0f;
	float RechargeTime = 0.0f;

	struct {
		bool KeyW = false;
		bool KeyA = false;
		bool KeyS = false;
		bool KeyD = false;
		bool KeyE = false;
		bool KeyQ = false;
		bool KeyTab = false;
		bool Space = false;
	} KeyPressed;

	struct Debris {
		ProgramObject* obj;
		float timetodestroy = 0.0f;
		float currenttime = 0.0f;
	};
	std::vector<Debris*> DebrisList;
	std::vector<Task> TaskScheduler;

	MouseObject* Mouse = nullptr;
	tn::Matrix4x4 projectionmatrix = {};

	WNDCLASSEXW MainWindowClass = {};
	WNDCLASSEXW SettingBoxClass = {};
	WNDCLASSEXW SelectionLangMsgBoxClass = {};

	HINSTANCE hInst = {};

	tn::GraphicInterfaceSetupProperties GraphicProp = {};

	bool IsGDIPlusRun = true;
	ULONG_PTR gdiPlusToken = 0;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;

	tn::FunctionError m_FunctionError = {};

	Sprite* ShadowScreenPause = nullptr;
	TextLabel* PauseText = nullptr;
	struct StaticGUIsA
	{
		MessageBoxUI* GameEndUI = nullptr;
		/**/TextLabel* GameEndText = nullptr;
	} StaticGUIs;
	lua_State* LuaFileLoad = nullptr;
	lua_State* LuaBase = nullptr;
	lua_State* LuaWeaponScript = nullptr;
	CRITICAL_SECTION cs;
	float interval = 0.0f;

	size_t CurrentThreadIndex = 0.0f;

	struct ToolboxA {
		bool Loaded = false;
		UI_Frame* ToolboxFrame = nullptr;
		/**/tn::ReferenceArray<UI_Frame> ToolboxFrameSlots = {};
		UI_Frame* ActiveItemInventory = nullptr;
		/**/ImageLabel* ActiveItemInventoryProfile = nullptr;
		/**/tn::ReferenceArray<UI_Frame> ActiveSlots = {};
		struct CharacterInventoryA {
			MessageBoxUI* Body = nullptr;
			tn::ReferenceArray<UI_Frame> FrameSlots = {};
		} CharacterInventory;

		GameplayRelatedObjects::SelectedInventoryItemBox* DraggedTool_InventoryBox = nullptr;
		GameplayRelatedObjects::InventorySlot* SelectedInventoryBoxWithEntityData = nullptr;
		bool BuildMode = false;
		Sprite* HighlightedPlaceableEntity = nullptr;
		Sprite* HighlightedTileInBuildMode = nullptr;
		size_t ActiveItemIndex = 0;

		UI_Frame* ObjectiveFrame = nullptr;
		/**/TextLabel* ObjectiveFrameTitle = nullptr;
		/**/TextLabel* ObjectiveFrameText = nullptr;
		std::wstring ObjectiveMsg = L"";
	} Toolbox;

	struct ProgramAnalysisA
	{
		unsigned long long RenderInterval = 0;
		float Frequency = 0.0f;
		unsigned long long MemoryUsageMB; //MB

		PROCESS_MEMORY_COUNTERS pmc;
	} ProgramAnalysis;

	struct DebugA
	{
		bool ShowDebug = false;
		TextLabel* RIText = nullptr;
		TextLabel* FrequencyText = nullptr;
		TextLabel* MemoryUsageMBText = nullptr;
	} Debug;

	HANDLE RenderingThread; DWORD ThreadID1; bool TriggerStop1 = false;
	HANDLE LuaOpenThread; DWORD ThreadID2;
	HANDLE TaskThread; DWORD ThreadID3; bool TriggerStop2 = false; bool CallToEndGameplay = false;
	HANDLE MainScriptThread, GameplayScriptThread;
private:
	static void InventoryFrameProfileEnter(Program* p, UI_Frame* Body, MouseObject* Mouse);
	static void InventoryFrameProfileLeave(Program* p, UI_Frame* Body, MouseObject* Mouse);
	static void SelectedInventoryFrameUndragged(Program* p, DragInterface* Body, MouseObject* Mouse);
	static void InventoryFrameClicked(Program* p, TextButton* Body, MouseObject* Mouse);
};