#include "Base.h"
#include "../Core/Array.h"

#include "MapLoading.h"
#include "MapEditor.h"

#include <thread>
#include "JSON.h"

Program* m_Program = nullptr;
MapEditor* m_MapEditor = nullptr;

tn::Sound* MainMenu = nullptr;
tn::Sound* WalkingSound = nullptr;

Scene* MainMenuScene = nullptr;
Scene* GameplayScene = nullptr;
Scene* LoadingScene = nullptr;

MessageBoxUI* AboutFrame = nullptr;
/**/TextLabel* AboutTextLabel = nullptr;
/**/TextButton* CloseButtonOfAbout = nullptr;
MessageBoxUI* MainMenuFrame = nullptr;
/**/TextButton* PlayButton = nullptr;
/**/TextButton* MapEditorButton = nullptr;
/**/TextButton* SettingButton = nullptr;
/**/TextButton* AboutButton = nullptr;

Sprite* MainMenuBackground = new Sprite(m_Program);

tn::ReferenceArray<ProgramObject> Obj = {};

struct
{
	MessageBoxUI* Body;
	ImageLabel* ImageTexture;
	TextButton* SwitchTileTextureButton;
	TextButton* ChangeTileModeButton;
	bool TileMode = false;
	std::vector<Collider2D*> ColliderDetectors;
	TextButton* SaveMapButton;

	bool EntityMode = false;
	ImageLabel* EntityTexture;
	TextButton* SwitchEntityButton;
	TextButton* PlaceEntityModeButton;
	TextButton* TeleportButton;
} EditorUI;

Sprite* TitleObj2 = nullptr;

bool ToOpenMapEditor = true;
HWND OKButton = nullptr, CancelButton = nullptr;
LRESULT CALLBACK ClosingMapEditorProp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

void AboutButtonFunction(Program* Program, TextButton* Body, MouseObject* MainMouse)
{
	UI_Frame* AboutFrame = dynamic_cast<UI_Frame*>(Body->GetParent()->GetParent()->GetChild(L"AboutFrame"));
	AboutFrame->Visible = true;
	UI_Frame* Parent = dynamic_cast<UI_Frame*>(Body->GetParent());
	Parent->Visible = false;
	Parent->Position = { 0.5f, 0.5f, -Parent->Size.Offset.X / 2, -Parent->Size.Offset.Y / 2 };

	if (tn::SoundBuffer* sound = dynamic_cast<tn::SoundBuffer*>(Program->GameHierachy.Assets->GetChild(L"gui_click")))
	{
		if (tn::Sound* sa = dynamic_cast<tn::Sound*>(Program->GameHierachy.Assets->GetChild(L"gui_click_sound")))
		{
			sa->Play();
		}
		else
		{
			tn::Sound* s = new tn::Sound();
			s->SetBuffer(sound);
			Program->GameHierachy.Assets->AddChild(s);
			s->Play();
		}
	}
}

void SettingButtonFunction(Program* Program, TextButton* Body, MouseObject* MainMouse)
{
	UI_Frame* SettingFrame = dynamic_cast<UI_Frame*>(Body->GetParent()->GetParent()->GetChild(L"SettingFrame"));
	SettingFrame->Visible = true;
	UI_Frame* Parent = dynamic_cast<UI_Frame*>(Body->GetParent());
	Parent->Visible = false;
	Parent->Position = { 0.5f, 0.5f, -Parent->Size.Offset.X / 2, -Parent->Size.Offset.Y / 2 };

	if (tn::SoundBuffer* sound = dynamic_cast<tn::SoundBuffer*>(Program->GameHierachy.Assets->GetChild(L"gui_click")))
	{
		if (tn::Sound* sa = dynamic_cast<tn::Sound*>(Program->GameHierachy.Assets->GetChild(L"gui_click_sound")))
		{
			sa->Play();
		}
		else
		{
			tn::Sound* s = new tn::Sound();
			s->SetBuffer(sound);
			Program->GameHierachy.Assets->AddChild(s);
			s->Play();
		}
	}
}

void MoveToLoadingGameplay(Program* Program, TextButton* Body, MouseObject* Mouse);
void LoadMapEditor(Program* m_Program, TextButton* Body, MouseObject* Mouse);
void CreateEditorUI();

void LoadMainMenu()
{
	AboutFrame = new MessageBoxUI(m_Program);
	AboutFrame->GraphicDevice = m_Program->MainGraphicDevice;
	AboutFrame->SetName(L"AboutFrame");
	AboutFrame->Visible = false;
	AboutFrame->BarSizeY = { 0, 50 };
	AboutFrame->BarColor = { 0.25f,0.25f,0.25f };
	AboutFrame->BarText = m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"about");
	AboutFrame->Size = { 0, 0, 580, 420 };
	AboutFrame->Position = { 0.5f, 0.5f, -(AboutFrame->Size.Offset.X / 2), -(AboutFrame->Size.Offset.Y / 2) };
	AboutFrame->ZDepth = 1.0f;
	AboutFrame->Load();

	AboutTextLabel = new TextLabel(m_Program);
	AboutTextLabel->GraphicDevice = m_Program->MainGraphicDevice;
	AboutTextLabel->SetName(L"AboutTextLabel");
	AboutTextLabel->Visible = true;
	AboutTextLabel->Size = { 1.0f, 0, 0, 350 };
	AboutTextLabel->Position = { 0, 0, 0, 50 };
	AboutTextLabel->ZDepth = 1.0f;
	AboutTextLabel->Text = m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"info");
	AboutTextLabel->Font = VerdanaFont;
	AboutTextLabel->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentLeft;
	AboutTextLabel->TextSize = 24.0f;
	AboutTextLabel->WordBreak = true;
	AboutTextLabel->BackgroundTransparencyAlpha = 0.0f;
	AboutTextLabel->TextColor = { 1.0f, 1.0f, 1.0f };
	AboutFrame->AddChild(AboutTextLabel);
	AboutTextLabel->Load();

	CloseButtonOfAbout = new TextButton(m_Program);
	CloseButtonOfAbout->GraphicDevice = m_Program->MainGraphicDevice;
	CloseButtonOfAbout->SetName(L"CloseButton");
	CloseButtonOfAbout->Size = { 0, 0, 100, 50 };
	CloseButtonOfAbout->BackgroundColor = { 0.8f, 0.04f, 0.05f };
	CloseButtonOfAbout->Position = { 0, 1.0f, 10, -(CloseButtonOfAbout->Size.Offset.Y + 10) };
	CloseButtonOfAbout->ZDepth = 1.0f;
	CloseButtonOfAbout->TextSize = 32.0f;
	CloseButtonOfAbout->Text = m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"close");
	CloseButtonOfAbout->Font = VerdanaFont;
	CloseButtonOfAbout->Italic = false;
	CloseButtonOfAbout->Bold = false;
	CloseButtonOfAbout->TextColor = { 1.0f,1.0f,1.0f };
	CloseButtonOfAbout->TextAlignment = TextAlignmentEnum::TextAlignmentCenter;
	CloseButtonOfAbout->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentMiddle;
	CloseButtonOfAbout->Alpha = 1.0f;
	AboutFrame->AddChild(CloseButtonOfAbout);
	CloseButtonOfAbout->Load();
	CloseButtonOfAbout->OnMouseButton1Click([](Program* Program, TextButton* Body, MouseObject* Mouse) -> void {
		if (Body)
		{
			if (UI_Frame* p = dynamic_cast<UI_Frame*>(Body->GetParent()))
			{
				p->Visible = false;
				p->Position = { 0.5f, 0.5f, -p->Size.Offset.X / 2, -p->Size.Offset.Y / 2 };
			}
			if (UI_Frame* p2 = dynamic_cast<UI_Frame*>(Body->GetParent()->GetParent()->GetChild(L"MainMenuFrame")))
			{
				p2->Visible = true;
			}

			if (tn::SoundBuffer* sound = dynamic_cast<tn::SoundBuffer*>(Program->GameHierachy.Assets->GetChild(L"gui_close")))
			{
				if (tn::Sound* sa = dynamic_cast<tn::Sound*>(Program->GameHierachy.Assets->GetChild(L"gui_close_sound")))
				{
					sa->Play();
				}
				else
				{
					tn::Sound* s = new tn::Sound();
					s->SetBuffer(sound);
					Program->GameHierachy.Assets->AddChild(s);
					s->Play();
				}
			}
		}
		});

	//////////////////////////////

	m_Program->LoadSettingsUI();

	//////////////////////////////

	MainMenuFrame = new MessageBoxUI(m_Program);
	MainMenuFrame->SetName(L"MainMenuFrame");
	MainMenuFrame->BarText = m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"main_menu");
	MainMenuFrame->GraphicDevice = m_Program->MainGraphicDevice;
	MainMenuFrame->BarSizeY = { 0, 50 };
	MainMenuFrame->BarColor = { 0.1f,0.1f,0.1f };
	MainMenuFrame->Size = { 0, 0, 300, 325 };
	MainMenuFrame->Position = { 0.5f, 0.5f, -MainMenuFrame->Size.Offset.X/2, -MainMenuFrame->Size.Offset.Y/2 };
	MainMenuFrame->ZDepth = 1.0f;
	MainMenuFrame->Load();

	PlayButton = new TextButton(m_Program);
	PlayButton->SetName(L"PlayButton");
	PlayButton->Size = { 0, 0, 300, 50 };
	PlayButton->BackgroundColor = { 0.5f, 0.5f, 0.5f };
	PlayButton->Position = { 0, 0, 0, 75 };
	PlayButton->ZDepth = 1.0f;
	PlayButton->TextSize = 32.0f;
	PlayButton->Text = m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"play");
	PlayButton->Font = VerdanaFont;
	PlayButton->Italic = false;
	PlayButton->Bold = false;
	PlayButton->TextColor = { 1.0f,1.0f,1.0f };
	PlayButton->GraphicDevice = m_Program->MainGraphicDevice;
	PlayButton->TextAlignment = TextAlignmentEnum::TextAlignmentCenter;
	PlayButton->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentMiddle;
	PlayButton->Alpha = 1.0f;
	PlayButton->Load();
	PlayButton->OnMouseButton1Click(MoveToLoadingGameplay);
	MainMenuFrame->AddChild(PlayButton);

	MapEditorButton = new TextButton(m_Program);
	MapEditorButton->SetName(L"MapEditorButton");
	MapEditorButton->Size = { 0, 0, 300, 50 };
	MapEditorButton->BackgroundColor = { 0.5f, 0.5f, 0.5f };
	MapEditorButton->Position = { 0, 0, 0, 150 };
	MapEditorButton->ZDepth = 1.0f;
	MapEditorButton->TextSize = 32.0f;
	MapEditorButton->Text = m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"ki_thuat_ban_do");
	MapEditorButton->Font = VerdanaFont;
	MapEditorButton->Italic = false;
	MapEditorButton->Bold = false;
	MapEditorButton->TextColor = { 1.0f,1.0f,1.0f };
	MapEditorButton->GraphicDevice = m_Program->MainGraphicDevice;
	MapEditorButton->TextAlignment = TextAlignmentEnum::TextAlignmentCenter;
	MapEditorButton->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentMiddle;
	MapEditorButton->Alpha = 1.0f;
	MapEditorButton->Load();
	MapEditorButton->OnMouseButton1Click(LoadMapEditor);
	MainMenuFrame->AddChild(MapEditorButton);

	SettingButton = new TextButton(m_Program);
	SettingButton->SetName(L"SettingButton");
	SettingButton->Size = { 0, 0, 300, 50 };
	SettingButton->BackgroundColor = { 0.5f, 0.5f, 0.5f };
	SettingButton->Position = { 0, 0, 0, 225 };
	SettingButton->ZDepth = 1.0f;
	SettingButton->TextSize = 32.0f;
	SettingButton->Text = m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"cai_dat");
	SettingButton->Font = VerdanaFont;
	SettingButton->Italic = false;
	SettingButton->Bold = false;
	SettingButton->TextColor = { 1.0f,1.0f,1.0f };
	SettingButton->GraphicDevice = m_Program->MainGraphicDevice;
	SettingButton->TextAlignment = TextAlignmentEnum::TextAlignmentCenter;
	SettingButton->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentMiddle;
	SettingButton->Alpha = 1.0f;
	SettingButton->Load();
	MainMenuFrame->AddChild(SettingButton);
	SettingButton->OnMouseButton1Click(SettingButtonFunction);

	AboutButton = new TextButton(m_Program);
	AboutButton->SetName(L"AboutButton");
	AboutButton->Size = { 0, 0, 300, 50 };
	AboutButton->BackgroundColor = { 0.5f, 0.5f, 0.5f };
	AboutButton->Position = { 0, 0, 0, 300 };
	AboutButton->ZDepth = 1.0f;
	AboutButton->TextSize = 32.0f;
	AboutButton->Text = m_Program->GetLocaleText(m_Program->CurrentLang.data(), L"about");
	AboutButton->Font = VerdanaFont;
	AboutButton->Italic = false;
	AboutButton->Bold = false;
	AboutButton->TextColor = { 1.0f,1.0f,1.0f };
	AboutButton->GraphicDevice = m_Program->MainGraphicDevice;
	AboutButton->TextAlignment = TextAlignmentEnum::TextAlignmentCenter;
	AboutButton->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentMiddle;
	AboutButton->Alpha = 1.0f;
	AboutButton->Load();
	MainMenuFrame->AddChild(AboutButton);
	AboutButton->OnMouseButton1Click(AboutButtonFunction);

	m_Program->Player->SetName(L"Player");
	m_Program->Player->Size = { 50, 50 };
	m_Program->Player->Position = { 0, 0 };
	m_Program->Player->SetTexture(dynamic_cast<tn::Texture*>(m_Program->GetFirstLoadedTexture(L"Player")));
	m_Program->Player->Alpha = 1.0f;
	m_Program->Player->ZDepth = 1.5f;

	m_Program->GameHierachy.GUI->AddChild(MainMenuFrame);
	m_Program->GameHierachy.GUI->AddChild(AboutFrame);
}

void CreateLoadingScene()
{
	LoadingScene = new Scene(m_Program);
	LoadingScene->SetName(L"LoadingScene");

	Sprite* BlackBackground = new Sprite(m_Program);
	BlackBackground->Color = { 0.0f, 0.0f, 0.0f };
	BlackBackground->Size = { 1000, 1000 };
	BlackBackground->ZDepth = 10.0f;
	LoadingScene->AddObjectToScene(BlackBackground);

	TextLabel* LoadingText = new TextLabel(m_Program);
	LoadingText->GraphicDevice = m_Program->MainGraphicDevice;
	LoadingText->Position = { 0, 0, -200, -200 };
	LoadingText->Size = { 0, 0, 400, 400 };
	LoadingText->BackgroundColor = { 0.0f, 0.0f, 0.0f };
	LoadingText->BackgroundTransparencyAlpha = 1.0f;
	LoadingText->Text = L"Loading";
	LoadingText->TextColor = { 1.0f, 1.0f, 1.0f };
	LoadingText->TextSize = 64.0f;
	LoadingText->Visible = true;
	LoadingText->ZDepth = 5.0f;
	LoadingText->Load();
	LoadingScene->AddObjectToScene(LoadingText);

	m_Program->GameHierachy.Scenes->AddChild(LoadingScene);
}

void LoadIntroAnimation()
{
	MainMenuBackground->Size = { 10000, 10000 };
	MainMenuBackground->Color = { 0, 0, 0 };
	MainMenuBackground->ZDepth = 15.0f;
	MainMenuScene->AddObjectToScene(MainMenuBackground);

	tn::Sound* Intro = new tn::Sound();
	Intro->SetBuffer(dynamic_cast<tn::SoundBuffer*>(m_Program->GameHierachy.Assets->GetChild(L"Intro")));
	Intro->SetVolume(1);
	//m_Program->CurrentScene->AddObjectToScene(Player);
	MainMenuScene->AddObjectToScene(Intro);
	m_Program->CurrentCamera->BackgroundColor = { 0, 0, 0 };

	Intro->Play();

	Animation BackgroundAnim = {};
	BackgroundAnim.TargetProperty = &MainMenuBackground->Color.Blue;
	BackgroundAnim.Duration = 3.5f;
	float startBA = 0.0f;
	float endBA = 0.3;
	BackgroundAnim.ValueType = AnimationValueType::Float;
	BackgroundAnim.MovementType = AnimationMovementType::Square;
	BackgroundAnim.InOrOut = AnimationMovementInOrOut::Out;
	BackgroundAnim.SetStart(&startBA);
	BackgroundAnim.SetEnd(&endBA);
	m_Program->AddAnimation(&BackgroundAnim);

	float startWA = 0.99f;
	float endWA = 0.0f;
	float endOA = 0.3f;

	Animation BackgroundAnimR1 = {};
	BackgroundAnimR1.TargetProperty = &MainMenuBackground->Color.Red;
	BackgroundAnimR1.Duration = 1.0f;
	BackgroundAnimR1.ValueType = AnimationValueType::Float;
	//para->t->Wait(0.001);
	BackgroundAnimR1.SetStart(&startWA);
	BackgroundAnimR1.SetEnd(&endWA);

	Animation BackgroundAnimR2 = {};
	BackgroundAnimR2.TargetProperty = &MainMenuBackground->Color.Green;
	BackgroundAnimR2.Duration = 1.0f;
	BackgroundAnimR2.ValueType = AnimationValueType::Float;
	//para->t->Wait(0.001);
	BackgroundAnimR2.SetStart(&endWA);
	BackgroundAnimR2.SetEnd(&endWA);

	Animation BackgroundAnimR3 = {};
	BackgroundAnimR3.TargetProperty = &MainMenuBackground->Color.Blue;
	BackgroundAnimR3.Duration = 1.0f;
	BackgroundAnimR3.ValueType = AnimationValueType::Float;
	//para->t->Wait(0.001);
	BackgroundAnimR3.SetStart(&startWA);
	BackgroundAnimR3.SetEnd(&endOA);

	m_Program->AddAnimation(&BackgroundAnimR1);
	m_Program->AddAnimation(&BackgroundAnimR2);
	m_Program->AddAnimation(&BackgroundAnimR3);

	BackgroundAnim.Play();
	BackgroundAnim.WaitUntilCompleted();
	BackgroundAnimR3.Play();
	//MainMenuBackground->Color = { 0.0f, 0.0f, 0.3f };
	//Sleep(3500);
}

void RenderStepped(void* Parameter) {
	float* deltaTime = static_cast<float*>(Parameter);
	if (deltaTime != nullptr)
	{
		std::cout << "Delta: " << (*deltaTime) << std::endl;
		std::cout << "Frequency: " <<  1.0f / (*deltaTime) << std::endl;
	}
};

void Print1(void* p)
{
	std::cout << "1" << std::endl;
}

void Print2(void* p)
{
	std::cout << "2" << std::endl;
}

void Print3(void* p)
{
	std::cout << "3" << std::endl;
}

DWORD CALLBACK Program::MainScript(LPVOID ThreadParameter)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	m_Program = static_cast<Program*>(ThreadParameter);
	m_Program->GameState == GameStates::MainMenu;

	//m_Program->CoreEvents.RenderStepped->ConnectedFunctions.push_back(RenderStepped);

	WalkingSound = new tn::Sound();
	WalkingSound->SetBuffer(dynamic_cast<tn::SoundBuffer*>(m_Program->GameHierachy.Assets->GetChild(L"PlayerWalking")));

	SceneDescription SceneDesc = {};
	SceneDesc.Type = ST_Static;
	SceneDesc.WorldWidth = 1000;
	SceneDesc.WorldHeight = 1000;

	m_Program->MainMenuSceneRef = new Scene(m_Program, SceneDesc);
	m_Program->MainMenuSceneRef->SetName(L"Start");

	MainMenuScene = m_Program->MainMenuSceneRef;

	SceneDescription GameplaySceneDesc = {};
	GameplaySceneDesc.Type = ST_2DSurface;
	GameplaySceneDesc.WorldWidth = 1000.0f;
	GameplaySceneDesc.WorldHeight = 1000.0f;

	GameplayScene = new Scene(m_Program, GameplaySceneDesc);
	GameplayScene->SetName(L"Gameplay");

	m_Program->SetCurrentScene(MainMenuScene);
	m_Program->GameHierachy.Scenes->AddChild(MainMenuScene);
	m_Program->GameHierachy.Scenes->AddChild(GameplayScene);

	TitleObj2 = new Sprite(m_Program);
	TitleObj2->SetName(L"Title");
	TitleObj2->Size = { 1000, 300 };
	TitleObj2->Position = { 0, 0 };
	TitleObj2->SetTexture(dynamic_cast<tn::Texture*>(m_Program->GetFirstLoadedTexture(L"Title")));
	TitleObj2->ZDepth = 5.0f;
	TitleObj2->Alpha = 1.0f;
	m_Program->CurrentScene->AddObjectToScene(TitleObj2);

	MainMenu = new tn::Sound();
	MainMenu->SetBuffer(dynamic_cast<tn::SoundBuffer*>(m_Program->GameHierachy.Assets->GetChild(L"MainMenu")));
	MainMenu->SetVolume(1);
	m_Program->CurrentScene->AddObjectToScene(MainMenu);

	CreateLoadingScene();
	LoadIntroAnimation();
	LoadMainMenu();

	//////////////////////////////

	MainMenu->Play();

	Folder* ParticlesMainMenu = new Folder(m_Program);
	ParticlesMainMenu->SetName(L"ParticlesFolder");
	m_Program->CurrentScene->AddObjectToScene(ParticlesMainMenu);

	Collider2D* Collider2DPlr = new Collider2D(m_Program);
	Collider2DPlr->BaseObject = m_Program->Player;

	std::cout << "All cleared!" << std::endl;

	while (m_Program->TriggerStop1 == false)
	{
		if (m_Program->GameState == GameStates::MainMenu)
		{
			Sleep(10);
		}
		else
		{
			Sleep(1);
		}
	}

	if (m_MapEditor != nullptr)
	{
		delete m_MapEditor;
	}
	CoUninitialize();

	m_Program->MainScriptThread = nullptr;
	return 0;
}

void StartGameplay(Program* m_Program);
void GenerateBaseplate(Program* m_Program)
{
	for (size_t y = (-GameplayScene->GetDesc().WorldHeight / 2); y <= (GameplayScene->GetDesc().WorldHeight / 2); y += 1)
	{
		for (size_t x = (-GameplayScene->GetDesc().WorldWidth / 2); x <= (GameplayScene->GetDesc().WorldWidth / 2); x += 1)
		{
			float l = (((size_t)(x / 10.0f) % 2 + 1.0f) + ((size_t)(y / 10.0f) % 2 + 1.0f)) / 2.0f;
			Sprite* Tile = new Sprite(m_Program);
			Tile->SetName(L"Tile");
			Tile->SetTexture(dynamic_cast<tn::Texture*>(m_Program->GetFirstLoadedTexture(L"GrassTileTexture")));
			Tile->Alpha = 1.0f;
			Tile->Size.X = 100;
			Tile->Size.Y = 100;
			Tile->Position.X = x * 100;
			Tile->Position.Y = y * 100;
			Tile->ZDepth = 2.0f;
			GameplayScene->AddTile(Tile);
		}
	}
}

void MoveToLoadingGameplay(Program* Program, TextButton* Body, MouseObject* Mouse)
{
	if (tn::SoundBuffer* sound = dynamic_cast<tn::SoundBuffer*>(Program->GameHierachy.Assets->GetChild(L"gui_click")))
	{
		if (tn::Sound* sa = dynamic_cast<tn::Sound*>(Program->GameHierachy.Assets->GetChild(L"gui_click_sound")))
		{
			sa->Play();
		}
		else
		{
			tn::Sound* s = new tn::Sound();
			s->SetBuffer(sound);
			Program->GameHierachy.Assets->AddChild(s);
			s->Play();
		}
	}

	MainMenuFrame->Visible = false;
	Program->CurrentCamera->BackgroundColor = { 0.0f, 0.0f, 0.0f };
	Program->SetCurrentScene(LoadingScene);
	Sleep(1.0f);
	if (MapLoading::Load2DMap(Program, &GameplayScene, L"./scripts/map.json") == LOAD2DMAP_FILE_NOT_FOUND)
	{
		MessageBoxW(nullptr, L"scripts/map.json not found!", L"ERROR", MB_ICONSTOP | MB_OK);
		throw;
	}

	MainMenu->Stop();

	Program->GameState = GameStates::Gameplay;
	StartGameplay(Program);
}

void StartGameplay(Program* m_Program)
{
	m_Program->SetCurrentScene(GameplayScene);
	m_Program->LoadInventoryUI();
	m_Program->LoadObjectiveUI();
	m_Program->StartGameplayScript();

	m_Program->GiveItemToPlayer(L"camcorder");
	m_Program->GiveItemToPlayer(L"wooden-block-item");
	GameplayScene->AddObjectToScene(m_Program->Player);
	m_Program->GameplayState = GameplayStates::Run;
	m_Program->Player->Position = { 0.0f, 0.0f };
}

HWND WidthTextBox, HeightTextBox;
size_t Width = 0;
int LengthOfWidth = 0;
size_t Height = 0;
int LengthOfHeight = 0;

LRESULT CALLBACK ClosingMapEditorProp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {

	std::vector<wchar_t> buffer;
	switch (uMsg) {
	case WM_LBUTTONUP:
		LengthOfWidth = GetWindowTextLengthW(WidthTextBox);
		LengthOfHeight = GetWindowTextLengthW(HeightTextBox);

		if (LengthOfWidth > 0)
		{
			buffer.resize(LengthOfWidth + 1);
			GetWindowTextW(WidthTextBox, buffer.data(), LengthOfWidth + 1);
			Width = wcstoull(buffer.data(), nullptr, 10);
		}

		if (LengthOfHeight > 0)
		{
			buffer.resize(LengthOfHeight + 1);
			GetWindowTextW(HeightTextBox, buffer.data(), LengthOfHeight + 1);
			Height = wcstoull(buffer.data(), nullptr, 10);
		}

		if (hWnd == OKButton)
		{
			ToOpenMapEditor = true;
		}
		else if (hWnd == CancelButton)
		{
			ToOpenMapEditor = false;
		}
		DestroyWindow(GetParent(hWnd));
		break;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
};

void LoadMapEditor(Program* m_Program, TextButton* Body, MouseObject* Mouse)
{
	if (tn::SoundBuffer* sound = dynamic_cast<tn::SoundBuffer*>(m_Program->GameHierachy.Assets->GetChild(L"gui_click")))
	{
		if (tn::Sound* sa = dynamic_cast<tn::Sound*>(m_Program->GameHierachy.Assets->GetChild(L"gui_click_sound")))
		{
			sa->Play();
		}
		else
		{
			tn::Sound* s = new tn::Sound();
			s->SetBuffer(sound);
			m_Program->GameHierachy.Assets->AddChild(s);
			s->Play();
		}
	}

	HFONT DefaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	HWND hWnd = CreateWindowExW(0L, L"SettingBoxTN", L"Map Editor Setup", 0, CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, nullptr, nullptr, nullptr, nullptr);
	SendMessageW(hWnd, WM_SETFONT, WPARAM(DefaultFont), TRUE);

	HWND WidthTexthWnd = CreateWindowExW(0L, L"STATIC", L"Width: ", WS_VISIBLE | WS_CHILD, 0, 10, 75, 50, hWnd, nullptr, nullptr, nullptr);
	SendMessageW(WidthTexthWnd, WM_SETFONT, WPARAM(DefaultFont), TRUE);

	HWND HeightTexthWnd = CreateWindowExW(0L, L"STATIC", L"Height: ", WS_VISIBLE | WS_CHILD, 0, 75, 75, 50, hWnd, nullptr, nullptr, nullptr);
	SendMessageW(HeightTexthWnd, WM_SETFONT, WPARAM(DefaultFont), TRUE);

	WidthTextBox = CreateWindowExW(0L, L"EDIT", L"", WS_VISIBLE | WS_CHILD | ES_NUMBER | WS_BORDER, 75, 10, 225, 50, hWnd, nullptr, nullptr, nullptr);
	SendMessageW(WidthTextBox, WM_SETFONT, WPARAM(DefaultFont), TRUE);
	SetWindowSubclass(WidthTextBox, ExtendedEditProc, 1, 0);

	HeightTextBox = CreateWindowExW(0L, L"EDIT", L"", WS_VISIBLE | WS_CHILD | ES_NUMBER | WS_BORDER, 75, 75, 225, 50, hWnd, nullptr, nullptr, nullptr);
	SendMessageW(HeightTextBox, WM_SETFONT, WPARAM(DefaultFont), TRUE);
	SetWindowSubclass(HeightTextBox, ExtendedEditProc, 1, 0);

	OKButton = CreateWindowExW(0L, L"BUTTON", L"OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 100, 130, 75, 30, hWnd, nullptr, nullptr, nullptr);
	SetWindowSubclass(OKButton, ClosingMapEditorProp, 1, 0);
	SendMessageW(OKButton, WM_SETFONT, WPARAM(DefaultFont), TRUE);

	CancelButton = CreateWindowExW(0L, L"BUTTON", L"Cancel", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 200, 130, 75, 30, hWnd, nullptr, nullptr, nullptr);
	SetWindowSubclass(CancelButton, ClosingMapEditorProp, 1, 0);
	SendMessageW(CancelButton, WM_SETFONT, WPARAM(DefaultFont), TRUE);

	ShowWindow(hWnd, SW_SHOW);

	bool Stop = false;
	MSG msg = {};
	while (Stop == false)
	{
		while (PeekMessageW(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);

			if (IsWindow(hWnd) == false) {
				Stop = true;
			}
			Sleep(1);
		}
		Sleep(1);
	}

	if (ToOpenMapEditor == true)
	{
		if (LengthOfWidth <= 4 and LengthOfWidth > 0)
		{
			if (Width == 0)
			{
				MessageBoxW(m_Program->MainWnd->GetHWND(), L"The width musn't be 0! It must be 1 <= x <= 9999", L"Invalid value", MB_ICONERROR);
				goto MAP_EDITOR_SETUP_END;
			}
		}
		else if (LengthOfWidth == 0)
		{
			MessageBoxW(m_Program->MainWnd->GetHWND(), L"The setting box musn't be empty!", L"Invalid value", MB_ICONERROR);
			goto MAP_EDITOR_SETUP_END;
		}
		else
		{
			MessageBoxW(m_Program->MainWnd->GetHWND(), L"Digits too long! It must be 1 <= x <= 9999", L"Invalid value", MB_ICONERROR);
			goto MAP_EDITOR_SETUP_END;
		}

		if (LengthOfHeight <= 4 and LengthOfHeight > 0)
		{
			if (Height == 0)
			{
				MessageBoxW(m_Program->MainWnd->GetHWND(), L"The height musn't be 0! It must be 1 <= x <= 9999", L"Invalid value", MB_ICONERROR);
				goto MAP_EDITOR_SETUP_END;
			}
		}
		else if (LengthOfHeight == 0)
		{
			MessageBoxW(m_Program->MainWnd->GetHWND(), L"The setting box musn't be empty!", L"Invalid value", MB_ICONERROR);
			goto MAP_EDITOR_SETUP_END;
		}
		else
		{
			MessageBoxW(m_Program->MainWnd->GetHWND(), L"Digits too long! It must be 1 <= x <= 9999", L"Invalid value", MB_ICONERROR);
			goto MAP_EDITOR_SETUP_END;
		}

		CreateEditorUI();

		MainMenuFrame->Visible = false;
		m_Program->CurrentCamera->BackgroundColor = { 0.0f, 0.0f, 0.0f };
		m_Program->CurrentCamera->ViewportPosition = { 0.0f, 0.0f, 0.0f };
		m_Program->SetCurrentScene(LoadingScene);

		m_MapEditor = new MapEditor();
		m_MapEditor->Width = Width;
		m_MapEditor->Height = Height;
		m_MapEditor->TargetScene = GameplayScene;
		m_MapEditor->Program = m_Program;
		m_MapEditor->ListOfTextures = m_Program->TileTextures;
		m_MapEditor->ReferenceListOfEntityPrototypesPtr = &m_Program->ListOfEntities;
		m_MapEditor->CreateMap();

		Sleep(0.5f);

		m_MapEditor->TextureIDSelected = 0;
		m_MapEditor->EntityIDSelected = 0;
		MainMenu->Stop();
		m_Program->GameState = GameStates::MapEditor;

		GameplayScene->AddObjectToScene(m_Program->Player);
		m_Program->Player->Position = { 0.0f, 0.0f };
		m_Program->SetCurrentScene(GameplayScene);
		EditorUI.Body->Visible = true;
	}

MAP_EDITOR_SETUP_END:
	DestroyWindow(hWnd);
	DestroyWindow(WidthTexthWnd);
	DestroyWindow(HeightTexthWnd);
	DestroyWindow(WidthTextBox);
	DestroyWindow(HeightTextBox);
	DestroyWindow(OKButton);
	DestroyWindow(CancelButton);
}

void OnSwitchTexture(Program* p, TextButton* body, MouseObject* m) {
	if (m_MapEditor->TextureIDSelected >= m_MapEditor->ListOfTextures.size() - 1)
	{
		m_MapEditor->TextureIDSelected = 0;
	}
	else
	{
		m_MapEditor->TextureIDSelected++;
	}
	EditorUI.ImageTexture->TargetTexture = m_MapEditor->ListOfTextures.at(m_MapEditor->TextureIDSelected);
}

void OnSwitchEntity(Program* p, TextButton* body, MouseObject* m) {
	if (m_MapEditor->EntityIDSelected >= m_MapEditor->ReferenceListOfEntityPrototypesPtr->size() - 1)
	{
		m_MapEditor->EntityIDSelected = 0;
	}
	else
	{
		m_MapEditor->EntityIDSelected++;
	}
	EditorUI.EntityTexture->TargetTexture = m_MapEditor->ReferenceListOfEntityPrototypesPtr->at(m_MapEditor->EntityIDSelected).Icon;
}

void ChangeTextureTile(Collider2D* Body, MouseObject* Mouse)
{
	if (Body->BaseObject != nullptr)
	{
		Sprite* sp = dynamic_cast<Sprite*>(Body->BaseObject);
		sp->SetTexture(EditorUI.ImageTexture->TargetTexture);
	}
}

void ChangeTextureTileMode(Program* p, TextButton* body, MouseObject* m)
{
	if (EditorUI.TileMode == false and EditorUI.EntityMode == false)
	{
		EditorUI.TileMode = true;
		EditorUI.EntityMode = false;
		body->BackgroundColor = { 0, 1.0f, 0 };
		for (Object* objtile : p->CurrentScene->Terrain->GetChildrenVector())
		{
			if (ProgramObject* rawTile = dynamic_cast<ProgramObject*>(objtile))
			{
				if (rawTile->IsActive())
				{
					if (Sprite* Tile = dynamic_cast<Sprite*>(rawTile))
					{
						Collider2D* newCollider2D = new Collider2D(m_Program);
						newCollider2D->BaseObject = Tile;
						newCollider2D->OnMouseClick(ChangeTextureTile);
						p->CurrentScene->AddObjectToScene(newCollider2D);
						EditorUI.ColliderDetectors.push_back(newCollider2D);
					}
				}
			}
		}
	}
	else
	{
		EditorUI.TileMode = false;
		body->BackgroundColor = { 0.5f, 0.5f, 0.5f };
		for (Collider2D* Collider : EditorUI.ColliderDetectors)
		{
			Collider->SetParent(nullptr);
			Collider->Destroy();
		}
		EditorUI.ColliderDetectors.clear();
	}
}

void PlaceEntity_Editor(Collider2D* Body, MouseObject* Mouse)
{
	if (Body->BaseObject != nullptr)
	{
		if (Sprite* sp = dynamic_cast<Sprite*>(Body->BaseObject))
		{
			m_Program->PlaceEntity(&m_MapEditor->ReferenceListOfEntityPrototypesPtr->at(m_MapEditor->EntityIDSelected), sp->Position);
		}
	}
}

void ChangeEntityPlacementMode(Program* p, TextButton* body, MouseObject* m)
{
	if (EditorUI.TileMode == false and EditorUI.EntityMode == false)
	{
		EditorUI.EntityMode = true;
		m_Program->SetBuildMode(true);
		EditorUI.TileMode = false;
		body->BackgroundColor = { 0, 1.0f, 0 };
		for (Object* objtile : p->CurrentScene->Terrain->GetChildrenVector())
		{
			if (ProgramObject* rawTile = dynamic_cast<ProgramObject*>(objtile))
			{
				if (rawTile->IsActive())
				{
					if (Sprite* Tile = dynamic_cast<Sprite*>(rawTile))
					{
						Collider2D* newCollider2D = new Collider2D(m_Program);
						newCollider2D->BaseObject = Tile;
						newCollider2D->OnMouseClick(PlaceEntity_Editor);
						p->CurrentScene->AddObjectToScene(newCollider2D);
						EditorUI.ColliderDetectors.push_back(newCollider2D);
					}
				}
			}
		}
	}
	else
	{
		EditorUI.EntityMode = false;
		m_Program->SetBuildMode(false);
		body->BackgroundColor = { 0.5f, 0.5f, 0.5f };
		for (Collider2D* Collider : EditorUI.ColliderDetectors)
		{
			Collider->SetParent(nullptr);
			Collider->Destroy();
		}
		EditorUI.ColliderDetectors.clear();
	}
}

void SaveMap(Program* p, TextButton* body, MouseObject* m)
{
	JSONElement structure = {};
	structure.SetEnumType(JSONElementTypeEnum::Object);

	JSONElement widthjson((float)GameplayScene->GetDesc().WorldWidth);
	widthjson.SetName(L"world_width");
	JSONElement heightjson((float)GameplayScene->GetDesc().WorldHeight);
	heightjson.SetName(L"world_height");

	structure << widthjson;
	structure << heightjson;

	JSONElement tiles = {};
	tiles.SetEnumType(JSONElementTypeEnum::Array);
	tiles.SetName(L"tiles");
	for (Object* Tile : GameplayScene->Terrain->GetChildrenVector())
	{
		if (Sprite* SpriteTile = dynamic_cast<Sprite*>(Tile))
		{
			JSONElement tileobj = {};
			tileobj.SetEnumType(JSONElementTypeEnum::Object);
			JSONElement tileX = SpriteTile->Position.X / PixelsOfTile;
			tileX.SetName(L"x");
			JSONElement tileY = -SpriteTile->Position.Y / PixelsOfTile;
			tileY.SetName(L"y");
			JSONElement tiletexturename = SpriteTile->GetTexture()->GetName();
			tiletexturename.SetName(L"tileType");
			tileobj << tileX; tileobj << tileY; tileobj << tiletexturename;
			tiles << tileobj;
		}
	}
	structure << tiles;

	JSONElement entities = {};
	entities.SetEnumType(JSONElementTypeEnum::Array);
	entities.SetName(L"entities");

	for (Object* rawEntity : GameplayScene->SceneWorkspace->GetChildrenVector())
	{
		if (Entity* entity = dynamic_cast<Entity*>(rawEntity))
		{
			if (entity->ReferencePrototype != nullptr)
			{
				JSONElement entityobj = {};
				entityobj.SetEnumType(JSONElementTypeEnum::Object);
				JSONElement entityposX = entity->Position.X;
				entityposX.SetName(L"x");
				JSONElement entityposY = -entity->Position.Y;
				entityposY.SetName(L"y");
				JSONElement prototypeName = entity->ReferencePrototype->PrototypeName;
				prototypeName.SetName(L"prototypeName");
				entityobj << entityposX; entityobj << entityposY; entityobj << prototypeName;
				entities << entityobj;
			}
		}
	}
	structure << entities;

	JSONGenerator JSON_Exporter = {};
	JSON_Exporter.SetFile(L"./newmap.json");
	JSON_Exporter.SetElement(&structure);
	JSON_Exporter.Start();

	MessageBoxW(nullptr, L"Your map has been saved successfully!", L"Map saved!", MB_OK);
}

void CreateEditorUI()
{
	EditorUI.Body = new MessageBoxUI(m_Program);
	EditorUI.Body->SetName(L"EditorFrame");
	EditorUI.Body->BarText = L"Editor";
	EditorUI.Body->GraphicDevice = m_Program->MainGraphicDevice;
	EditorUI.Body->BarSizeY = { 0, 50 };
	EditorUI.Body->BarColor = { 0.1f,0.1f,0.1f };
	EditorUI.Body->Size = { 0, 0, 300, 325 };
	EditorUI.Body->Position = { 0, 0, 0, 0 };
	EditorUI.Body->BackgroundColor = { 0, 0, 0.1f };
	EditorUI.Body->ZDepth = 1.0f;
	EditorUI.Body->Visible = false;
	EditorUI.Body->Load();

	EditorUI.SwitchTileTextureButton = new TextButton(m_Program);
	EditorUI.SwitchTileTextureButton->SetName(L"SwitchTileTextureButton");
	EditorUI.SwitchTileTextureButton->Size.Offset = { 150, 35 };
	EditorUI.SwitchTileTextureButton->Position.Offset = { 25, 75 };
	EditorUI.SwitchTileTextureButton->Text = L"Switch texture";
	EditorUI.SwitchTileTextureButton->TextColor = { 0, 0, 0 };
	EditorUI.SwitchTileTextureButton->BackgroundColor = { 0.5f, 0.5f, 0.5f };
	EditorUI.SwitchTileTextureButton->GraphicDevice = m_Program->MainGraphicDevice;
	EditorUI.SwitchTileTextureButton->Visible = true;
	EditorUI.SwitchTileTextureButton->TextSize = 20.0f;
	EditorUI.SwitchTileTextureButton->WordBreak = true;
	EditorUI.SwitchTileTextureButton->Load();
	EditorUI.SwitchTileTextureButton->OnMouseButton1Up(OnSwitchTexture);
	EditorUI.Body->AddChild(EditorUI.SwitchTileTextureButton);

	EditorUI.ImageTexture = new ImageLabel(m_Program);
	EditorUI.ImageTexture->SetName(L"ImageTexture");
	EditorUI.ImageTexture->Size.Offset = { 35, 35 };
	EditorUI.ImageTexture->Position.Offset = { 200, 75 };
	EditorUI.ImageTexture->GraphicDevice = m_Program->MainGraphicDevice;
	EditorUI.ImageTexture->TargetTexture = m_Program->TileTextures.at(0);
	EditorUI.ImageTexture->Load();
	EditorUI.Body->AddChild(EditorUI.ImageTexture);

	EditorUI.ChangeTileModeButton = new TextButton(m_Program);
	EditorUI.ChangeTileModeButton->SetName(L"ChangeTileModeButton");
	EditorUI.ChangeTileModeButton->Size.Offset = { 250, 35 };
	EditorUI.ChangeTileModeButton->Position.Offset = { 25, 75 + 50 * 1 };
	EditorUI.ChangeTileModeButton->Text = L"Change tile's texture";
	EditorUI.ChangeTileModeButton->TextColor = { 0, 0, 0 };
	EditorUI.ChangeTileModeButton->BackgroundColor = { 0.5f, 0.5f, 0.5f };
	EditorUI.ChangeTileModeButton->GraphicDevice = m_Program->MainGraphicDevice;
	EditorUI.ChangeTileModeButton->Visible = true;
	EditorUI.ChangeTileModeButton->TextSize = 24.0f;
	EditorUI.ChangeTileModeButton->WordBreak = true;
	EditorUI.ChangeTileModeButton->Load();
	EditorUI.ChangeTileModeButton->OnMouseButton1Up(ChangeTextureTileMode);
	EditorUI.Body->AddChild(EditorUI.ChangeTileModeButton);

	EditorUI.SaveMapButton = new TextButton(m_Program);
	EditorUI.SaveMapButton->SetName(L"SaveMapButton");
	EditorUI.SaveMapButton->Size.Offset = { 250, 35 };
	EditorUI.SaveMapButton->Position.Offset = { 25, 75 + 50 * 2 };
	EditorUI.SaveMapButton->Text = L"Save map";
	EditorUI.SaveMapButton->TextColor = { 0, 0, 0 };
	EditorUI.SaveMapButton->BackgroundColor = { 0.5f, 0.5f, 0.5f };
	EditorUI.SaveMapButton->GraphicDevice = m_Program->MainGraphicDevice;
	EditorUI.SaveMapButton->Visible = true;
	EditorUI.SaveMapButton->TextSize = 24.0f;
	EditorUI.SaveMapButton->WordBreak = true;
	EditorUI.SaveMapButton->Load();
	EditorUI.SaveMapButton->OnMouseButton1Up(SaveMap);
	EditorUI.Body->AddChild(EditorUI.SaveMapButton);

	EditorUI.EntityTexture = new ImageLabel(m_Program);
	EditorUI.EntityTexture->SetName(L"EntityTexture");
	EditorUI.EntityTexture->Size.Offset = { 35, 35 };
	EditorUI.EntityTexture->Position.Offset = { 200, 75 + 50 * 3 };
	EditorUI.EntityTexture->GraphicDevice = m_Program->MainGraphicDevice;
	EditorUI.EntityTexture->Visible = true;
	EditorUI.EntityTexture->Load();
	EditorUI.Body->AddChild(EditorUI.EntityTexture);

	EditorUI.SwitchEntityButton = new TextButton(m_Program);
	EditorUI.SwitchEntityButton->SetName(L"SwitchEntityButton");
	EditorUI.SwitchEntityButton->Size.Offset = { 150, 35 };
	EditorUI.SwitchEntityButton->Position.Offset = { 25, 75 + 50 * 3 };
	EditorUI.SwitchEntityButton->Text = L"Switch entity";
	EditorUI.SwitchEntityButton->TextColor = { 0, 0, 0 };
	EditorUI.SwitchEntityButton->BackgroundColor = { 0.5f, 0.5f, 0.5f };
	EditorUI.SwitchEntityButton->GraphicDevice = m_Program->MainGraphicDevice;
	EditorUI.SwitchEntityButton->Visible = true;
	EditorUI.SwitchEntityButton->TextSize = 20.0f;
	EditorUI.SwitchEntityButton->WordBreak = true;
	EditorUI.SwitchEntityButton->Load();
	EditorUI.SwitchEntityButton->OnMouseButton1Up(OnSwitchEntity);
	EditorUI.Body->AddChild(EditorUI.SwitchEntityButton);

	EditorUI.PlaceEntityModeButton = new TextButton(m_Program);
	EditorUI.PlaceEntityModeButton->SetName(L"PlaceEntityModeButton");
	EditorUI.PlaceEntityModeButton->Size.Offset = { 250, 35 };
	EditorUI.PlaceEntityModeButton->Position.Offset = { 25, 75 + 50 * 4 };
	EditorUI.PlaceEntityModeButton->Text = L"Place entity";
	EditorUI.PlaceEntityModeButton->TextColor = { 0, 0, 0 };
	EditorUI.PlaceEntityModeButton->BackgroundColor = { 0.5f, 0.5f, 0.5f };
	EditorUI.PlaceEntityModeButton->GraphicDevice = m_Program->MainGraphicDevice;
	EditorUI.PlaceEntityModeButton->Visible = true;
	EditorUI.PlaceEntityModeButton->TextSize = 20.0f;
	EditorUI.PlaceEntityModeButton->WordBreak = true;
	EditorUI.PlaceEntityModeButton->Load();
	EditorUI.PlaceEntityModeButton->OnMouseButton1Up(ChangeEntityPlacementMode);
	EditorUI.Body->AddChild(EditorUI.PlaceEntityModeButton);

	m_Program->GameHierachy.GUI->AddChild(EditorUI.Body);
}