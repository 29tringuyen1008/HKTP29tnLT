#include "Base.h"
#include "JSON.h"
#include <codecvt>
#include <locale>
#include <string>

Program::Program()
{
	this->IsGDIPlusRun = true;
	Gdiplus::GdiplusStartup(&gdiPlusToken, &gdiplusStartupInput, nullptr);
	InitializeCriticalSection(&cs);

	hInst = GetModuleHandle(NULL);

	Sleep(1);

	MainWindowClass.cbSize = sizeof(WNDCLASSEXW);
	MainWindowClass.lpszClassName = L"Program2";
	MainWindowClass.lpfnWndProc = &WindowProcedure;
	MainWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	MainWindowClass.hCursor = LoadCursorW(0, IDC_ARROW);
	MainWindowClass.hIcon = LoadIconW(0, IDI_APPLICATION);
	MainWindowClass.hInstance = hInst;
	//wndclass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));

	RegisterClassExW(&MainWindowClass);

	SettingBoxClass.cbSize = sizeof(WNDCLASSEXW);
	SettingBoxClass.lpszClassName = L"SettingBoxTN";
	SettingBoxClass.lpfnWndProc = &WindowProcedure;
	SettingBoxClass.style = CS_HREDRAW | CS_VREDRAW;
	SettingBoxClass.hCursor = LoadCursorW(0, IDC_ARROW);
	SettingBoxClass.hIcon = LoadIconW(0, IDI_APPLICATION);
	SettingBoxClass.hInstance = hInst;
	//wndclass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));

	RegisterClassExW(&SettingBoxClass);
	////////////////////////////////////////

	SelectionLangMsgBoxClass.cbSize = sizeof(WNDCLASSEXW);
	SelectionLangMsgBoxClass.lpszClassName = L"SelectionMsgBoxTN";
	SelectionLangMsgBoxClass.lpfnWndProc = &SelectionLangMessageBox;
	SelectionLangMsgBoxClass.style = CS_HREDRAW | CS_VREDRAW;
	SelectionLangMsgBoxClass.hCursor = LoadCursorW(0, IDC_ARROW);
	SelectionLangMsgBoxClass.hIcon = LoadIconW(0, IDI_APPLICATION);
	SelectionLangMsgBoxClass.hInstance = hInst;
	SelectionLangMsgBoxClass.hbrBackground = (HBRUSH)(COLOR_WINDOW);

	ATOM resultatom = RegisterClassExW(&SelectionLangMsgBoxClass);
	if (resultatom == 0)
	{
		std::wcout << L"RegisterClassExW failed. Error: " << GetLastError() << std::endl;
	}

	Sleep(1);
}

Program::~Program()
{
	delete MainWnd;

	UnregisterClassW(L"Program2", hInst);
	UnregisterClassW(L"SettingBoxTN", hInst);
	UnregisterClassW(L"SelectionMsgBox", nullptr);

	DeleteCriticalSection(&cs);
	this->IsGDIPlusRun = false;
	Gdiplus::GdiplusShutdown(gdiPlusToken);
}

void Program::Start()
{
	this->SetupInitialLangSelection();

	this->Mouse = new MouseObject(this);
	this->MainWnd = new tn::Window();

	///

	this->LuaFileLoad = luaL_newstate(this);
	if (luaL_dofile(this->LuaFileLoad, "./scripts/config.lua") != LUA_OK)
	{
		MessageBoxW(this->MainWnd->GetHWND(), L"Phần mềm không có scripts/config.lua!\nThe program doesn't have scripts/config.lua!", L"ERROR", MB_ICONERROR);
		return;
	}

	///NGÔN NGỮ
	{
		//////////////////////TIẾNG VIỆT////////////////////////
		lua_State* LocaleLoad = luaL_newstate(this);
		if (luaL_dofile(LocaleLoad, "./scripts/locale/vi.lua") != LUA_OK)
		{
			MessageBoxW(this->MainWnd->GetHWND(), L"Phần mềm không có scripts/locale/vi.lua!\nThe program doesn't have scripts/locale/vi.lua!", L"ERROR", MB_ICONERROR);
			return;
		}

		if (this->Localization.GetMaximumSize() == 1)
		{
			this->Localization[0] = { L"vi", {} };
		}
		else
		{
			this->Localization.AddToEnd({ L"vi", {} });
		}

		size_t sizeoftextsarray = 0;
		{
			lua_getglobal(LocaleLoad, "texts");
			if (lua_istable(LocaleLoad, -1))
			{
				lua_pushnil(LocaleLoad);
				while (lua_next(LocaleLoad, -2) != 0)
				{
					sizeoftextsarray++;

					lua_pop(LocaleLoad, 1);
				}
			}
		}
		this->Localization[0].Texts.Resize(sizeoftextsarray);
		{
			size_t i = 0;
			lua_getglobal(LocaleLoad, "texts");
			if (lua_istable(LocaleLoad, -1))
			{
				lua_pushnil(LocaleLoad);
				while (lua_next(LocaleLoad, -2) != 0)
				{
					static tn::NormalArray<wchar_t> StrByteBuffer = {};
					const char* nametext = lua_tostring(LocaleLoad, -2);
					const char* texttodisplay = lua_tostring(LocaleLoad, -1);

					tn::NormalArray<wchar_t>nametextTTD = {};
					tn::NormalArray<wchar_t>widetextTTD = {};

					unsigned int wideLengthNT = MultiByteToWideChar(CP_UTF8, 0, nametext, -1, NULL, 0);
					if (wideLengthNT > 0)
					{
						nametextTTD.Resize(wideLengthNT);
						MultiByteToWideChar(CP_UTF8, 0, nametext, -1, nametextTTD.Data(), wideLengthNT);
					}

					unsigned int wideLengthTTD = MultiByteToWideChar(CP_UTF8, 0, texttodisplay, -1, NULL, 0);
					if (wideLengthTTD > 0)
					{
						widetextTTD.Resize(wideLengthTTD);
						MultiByteToWideChar(CP_UTF8, 0, texttodisplay, -1, widetextTTD.Data(), wideLengthTTD);
					}

					if (nametextTTD.Data() != nullptr and widetextTTD.Data() != nullptr)
					{
						this->Localization[0].Texts[i] = { nametextTTD.Data(), widetextTTD.Data() };
					}
					
					lua_pop(LocaleLoad, 1);
					i++;
					Sleep(1);
				}
			}
		}
		lua_close(LocaleLoad);

		//////////////////////NGÔN NGỮ CÒN LẠI//////////////////////

		lua_getglobal(this->LuaFileLoad, "other_locale");
		if (lua_istable(this->LuaFileLoad, -1))
		{
			tn::NormalArray<wchar_t>widetextlang = {};
			lua_pushnil(this->LuaFileLoad);
			while (lua_next(this->LuaFileLoad, -2) != 0)
			{
				if (lua_isstring(this->LuaFileLoad, -1))
				{
					const char* lang_locale = lua_tostring(this->LuaFileLoad, -1);
					widetextlang.Resize(strlen(lang_locale) + 1);
					unsigned int wideLengthlang = MultiByteToWideChar(CP_UTF8, 0, lang_locale, -1, NULL, 0);
					if (wideLengthlang > 0)
					{
						MultiByteToWideChar(CP_UTF8, 0, lang_locale, -1, widetextlang.Data(), wideLengthlang);
					}
					size_t newindex = this->Localization.AddToEnd({ widetextlang.data(), {}});

					//Lấy những câu trong tệp cho một ngôn ngữ.
					std::string lang_filelink = "./scripts/locale/" + std::string(lang_locale) + ".lua";
					LocaleLoad = luaL_newstate(this);
					if (luaL_dofile(LocaleLoad, lang_filelink.c_str()) != LUA_OK)
					{
						std::wstring errmsg = L"Phần mềm không có scripts/locale/" + std::wstring(widetextlang.Data()) + L".lua!\nThe program doesn't have scripts/locale/" + std::wstring(widetextlang.Data()) + L".lua!\0";
						MessageBoxW(this->MainWnd->GetHWND(), errmsg.data(), L"ERROR", MB_ICONERROR);
						return;
					}

					size_t sizeoftextsarray = 0;
					{
						lua_getglobal(LocaleLoad, "texts");
						if (lua_istable(LocaleLoad, -1))
						{
							lua_pushnil(LocaleLoad);
							while (lua_next(LocaleLoad, -2) != 0)
							{
								sizeoftextsarray++;

								lua_pop(LocaleLoad, 1);
							}
						}
					}
					this->Localization[newindex].Texts.Resize(sizeoftextsarray);
					size_t i_text_other_lang = 0;
					lua_getglobal(LocaleLoad, "texts");
					if (lua_istable(LocaleLoad, -1))
					{
						lua_pushnil(LocaleLoad);
						while (lua_next(LocaleLoad, -2) != 0)
						{
							static tn::NormalArray<wchar_t> StrByteBuffer = {};
							const char* nametext = lua_tostring(LocaleLoad, -2);
							const char* texttodisplay = lua_tostring(LocaleLoad, -1);

							tn::NormalArray<wchar_t>nametextTTD = {};
							tn::NormalArray<wchar_t>widetextTTD = {};

							unsigned int wideLengthNT = MultiByteToWideChar(CP_UTF8, 0, nametext, -1, NULL, 0);
							if (wideLengthNT > 0)
							{
								nametextTTD.Resize(wideLengthNT);
								MultiByteToWideChar(CP_UTF8, 0, nametext, -1, nametextTTD.Data(), wideLengthNT);
							}

							unsigned int wideLengthTTD = MultiByteToWideChar(CP_UTF8, 0, texttodisplay, -1, NULL, 0);
							if (wideLengthTTD > 0)
							{
								widetextTTD.Resize(wideLengthTTD);
								MultiByteToWideChar(CP_UTF8, 0, texttodisplay, -1, widetextTTD.Data(), wideLengthTTD);
							}

							if (nametextTTD.Data() != nullptr and widetextTTD.Data() != nullptr)
							{
								this->Localization[newindex].Texts[i_text_other_lang] = { nametextTTD.Data(), widetextTTD.Data() };
							}

							lua_pop(LocaleLoad, 1);
							i_text_other_lang++;
							Sleep(1);
						}
					}

					lua_close(LocaleLoad);
				}
				lua_pop(this->LuaFileLoad, 1);
			}
		}
		else
		{
			lua_pop(this->LuaFileLoad, 1);
		}
	}

	this->GetLangIndex(&this->CurrentLangIndex, this->CurrentLang);
	//

	tn::StartingWindowDescription StartingWndDesc = {};
	StartingWndDesc.SizeX = 1280;
	StartingWndDesc.SizeY = 720;
	StartingWndDesc.hInstance = hInst;
	StartingWndDesc.WindowClassType = MainWindowClass.lpszClassName;
	std::wstring wndname = this->GetLocaleText(this->CurrentLang.data(), L"program_name");
	const wchar_t* wndname_raw = wndname.c_str();
	StartingWndDesc.WindowName = wndname_raw;
	MainWnd->UseWindowDataAsClass = false;
	MainWnd->ContinueAfterDestroyed = false;

	std::wcout << L"STAGE 1 STARTED" << std::endl;
	std::wcout << L"Creating Window...";
	if (MainWnd->Create(&StartingWndDesc) == true)
	{
		MainWnd->SetVoidAttributesInAReinterpretedByteData(this);
		std::wcout << L"SUCCESS!" << std::endl;
	}
	else
	{
		std::wcout << L"FAILED!" << std::endl;
		MessageBoxW(nullptr, L"Phần mềm không thể tạo màn hình/cửa sổ!\nThe program has failed to create the window!", L"ERROR", MB_ICONSTOP | MB_OK);
		throw;
	}

	//

	RECT winrt = {};
	GetClientRect(this->MainWnd->GetHWND(), &winrt);

	MainGraphicDevice = new tn::GraphicsDevice();
	MainGraphicDevice->SetErrorData(&m_FunctionError);

	GraphicProp.BackBufferCount = 1;
	GraphicProp.ScreenWidth = StartingWndDesc.SizeX;
	GraphicProp.ScreenHeight = StartingWndDesc.SizeY;
	GraphicProp.Windowed = true;
	GraphicProp.UI = MainWnd;
	GraphicProp.MultiSampleType = 1;
	GraphicProp.DepthStencilEnable = true;
	GraphicProp.DepthStencilType = D16;
	GraphicProp.SurfacesDimensionalType = SDT_32Bits;
	GraphicProp.SwapChainEffect = Discard;

	std::wcout << L"Starting graphic engine...";
	Sleep(1000);
	if (MainGraphicDevice->Initialize(&GraphicProp) == true)
	{
		std::wcout << L"SUCCESS!" << std::endl;
	}
	else
	{
		std::wcout << L"FAILED!" << std::endl;
		MessageBoxW(nullptr, L"Phần mềm không thể bật công cụ vẽ!\nThe program has failed to start the engine!", L"ERROR", MB_ICONSTOP | MB_OK);
		throw;
	}

	this->CurrentCamera = new Camera();
	this->CurrentCamera->ViewportPosition = { 0.0f, 0.0f, 0.0f };
	this->CurrentCamera->ViewportSize = { float(StartingWndDesc.SizeX), float(StartingWndDesc.SizeY) };
	this->CurrentCamera->ViewportOffset = { -float(StartingWndDesc.SizeX) / 2, -float(StartingWndDesc.SizeY) / 2 };
	this->CurrentCamera->BackgroundColor = { 0.0f, 0.0f, 0.25f };
	this->CurrentCamera->NearView = 0.0f;
	this->CurrentCamera->FarView = 100.0f;

	std::wcout << L"STAGE 1 FINISHED!" << std::endl << std::endl;

	this->LoadResources();
}

void Program::LoadResources()
{
	GameHierachy.Scenes = new SceneFolder(this);
	GameHierachy.Scenes->SetName(L"Scenes");
	GameHierachy.GUI = new GUIFolder(this);
	GameHierachy.GUI->SetName(L"GUI");
	GameHierachy.Assets = new AssetFolder(this);
	GameHierachy.Assets->SetName(L"Assets");

	//this->Mouse = new MouseObject(this);

	std::wcout << L"STAGE 2 STARTED" << std::endl;

	AudioAPIType AudioApiSetType;
	lua_getglobal(this->LuaFileLoad, "SoundEngine");
	if (lua_type(this->LuaFileLoad, -1) == LUA_TSTRING)
	{
		const char* soundapitypestr = lua_tostring(this->LuaFileLoad, -1);
		if (strcmp(soundapitypestr, "DirectSound") == 0)
		{
			AudioApiSetType = AudioAPIType::API_DirectSound;
		}
		else if (strcmp(soundapitypestr, "XAudio2") == 0)
		{
			AudioApiSetType = AudioAPIType::API_XAudio2;
		}
		else
		{
			MessageBoxW(this->MainWnd->GetHWND(), L"Invalid Sound API", L"ERROR", MB_ICONERROR);
			return;
		}

		this->MainSoundFramework = new tn::SoundFramework();
		this->MainSoundFramework->APIType = AudioApiSetType;
		this->MainSoundFramework->Window = this->MainWnd;
		this->MainSoundFramework->Initialize();
	}
	else if (lua_type(this->LuaFileLoad, -1) == LUA_TNUMBER)
	{
		lua_Number soundapitypenum = lua_tonumber(this->LuaFileLoad, -1);
		if (soundapitypenum == 0)
		{
			AudioApiSetType = AudioAPIType::API_DirectSound;
		}
		else if (soundapitypenum == 1)
		{
			AudioApiSetType = AudioAPIType::API_XAudio2;
		}
		else
		{
			MessageBoxW(this->MainWnd->GetHWND(), L"Invalid Sound API", L"ERROR", MB_ICONERROR);
			return;
		}
	}
	else
	{
		MessageBoxW(this->MainWnd->GetHWND(), L"In scripts/config.lua, SoundEngine must be either string or number", L"ERROR", MB_ICONERROR);
		return;
	}
	lua_pop(this->LuaFileLoad, 1);

	{
		std::vector<wchar_t> StrBuffer;
		std::vector<std::wstring> ListOfAudioNames = {};
		std::vector<tn::AudioDescription> ListOfAudioDesc = {};
		//Lấy nội dung của âm thanh -- Get info of audio
		for (size_t i = 0; i < ListofAudio.size(); i++)
		{
			lua_getglobal(this->LuaFileLoad, ListofAudio[i]);
		}
		lua_getglobal(this->LuaFileLoad, "Audio");
		if (!lua_istable(this->LuaFileLoad, -1)) {
			lua_pop(this->LuaFileLoad, 1);  // Clean up stack
			//throw std::runtime_error("Lua global 'Audio' is not a table");
			return;
		}
		lua_pushnil(this->LuaFileLoad);
		while (lua_next(this->LuaFileLoad, -2) != 0) {
			const char* key = lua_tostring(this->LuaFileLoad, -2);
			const char* value = lua_tostring(this->LuaFileLoad, -1);
			if (!key || !lua_istable(this->LuaFileLoad, -1)) {
				lua_pop(this->LuaFileLoad, 1);
				continue;
			}

			lua_getfield(this->LuaFileLoad, -1, "Loopable");
			bool Loop = (lua_toboolean(this->LuaFileLoad, -1) == TRUE) ? true : false;
			lua_pop(this->LuaFileLoad, 1);

			lua_getfield(this->LuaFileLoad, -1, "File");
			const char* strp = lua_tolstring(this->LuaFileLoad, -1, nullptr);
			lua_pop(this->LuaFileLoad, 1);

			size_t sizeforwchar = 0;
			mbstowcs_s(&sizeforwchar, nullptr, 0, key, 0);
			StrBuffer.resize(sizeforwchar);
			mbstowcs_s(nullptr, StrBuffer.data(), sizeforwchar, key, _TRUNCATE);
			ListOfAudioNames.push_back(std::wstring(StrBuffer.data()));
			StrBuffer.clear();

			sizeforwchar = 0;
			mbstowcs_s(&sizeforwchar, nullptr, 0, strp, 0);
			StrBuffer.resize(sizeforwchar);
			mbstowcs_s(nullptr, StrBuffer.data(), sizeforwchar, strp, _TRUNCATE);

			tn::AudioDescription desc = {};
			desc.APIType = AudioApiSetType;
			desc.EnableLoop = Loop;
			desc.Filename = std::wstring(StrBuffer.data());
			desc.Windowed = false;

			ListOfAudioDesc.push_back(desc);
			StrBuffer.clear();

			lua_pop(this->LuaFileLoad, 1); //Để chuyển tiếp âm thanh tiếp theo.
		}
		//Tải về âm thanh -- To load audio
		for (size_t i = 0; i < ListOfAudioDesc.size(); i++)
		{
			tn::SoundBuffer* LoadingAudioBuffer = new tn::SoundBuffer(this->MainSoundFramework);
			LoadingAudioBuffer->SetDesc(ListOfAudioDesc.at(i));

			//NAME
			LoadingAudioBuffer->SetName(ListOfAudioNames[i]);

			int successcode = LoadingAudioBuffer->Initialize();
			if (successcode != 0)
			{
				std::wstring FullMsg;
				switch (successcode)
				{
				case TN_AUDIO_ERR_FILENOTFOUND:
					FullMsg = L"The program has failed to load an audio! (" + std::wstring(StrBuffer.data()) + L")\nAudio file not found!";
					break;
				case TN_AUDIO_ERR_FILEEMPTY:
					FullMsg = L"The program has failed to load an audio! (" + std::wstring(StrBuffer.data()) + L")\nAudio file seems to be too small or empty";
					break;
				case TN_AUDIO_ERR_WAVE_FORMAT:
					FullMsg = L"The program has failed to load an audio! (" + std::wstring(StrBuffer.data()) + L")\nAudio file must be .wav, or is it corrupted?";
					break;
				case TN_AUDIO_ERR_MEMORY:
					FullMsg = L"The program has failed to load an audio! (" + std::wstring(StrBuffer.data()) + L")\nAudio engine cannot allocate space for a loaded audio file in a code. If the download is the source code, please check and fix the code, and then compile it to test it again.";
					break;
				case TN_AUDIO_ERR_INIT:
					FullMsg = L"The program has failed to load an audio! (" + std::wstring(StrBuffer.data()) + L")\nAudio buffer cannot be initialized in a code. If the download is the source code, please check and fix the code, and then compile it to test it again.";
					break;
				}
				MessageBoxW(nullptr, FullMsg.c_str(), L"ERROR", MB_ICONSTOP | MB_OK);
				//return this->Stop();
			}

			this->GameHierachy.Assets->AddChild(LoadingAudioBuffer);
		}
	}

	///////////////////////////////////////////////////////////////////

	{
		std::vector<wchar_t> StrBuffer;
		std::vector<std::wstring> TextureNames = {};
		std::vector<std::wstring> ImageDestinationList = {};

		{
			//Lấy nội dung của hình ảnh -- Get info of the texture
			lua_getglobal(this->LuaFileLoad, "Textures");
			lua_pushnil(this->LuaFileLoad);
			while (lua_next(this->LuaFileLoad, -2) != 0) {
				const char* key = lua_tostring(this->LuaFileLoad, -2);
				const char* value = lua_tostring(this->LuaFileLoad, -1);

				size_t sizeforwchar = 0;
				mbstowcs_s(&sizeforwchar, nullptr, 0, key, 0);
				StrBuffer.resize(sizeforwchar);
				mbstowcs_s(nullptr, StrBuffer.data(), sizeforwchar, key, _TRUNCATE);
				TextureNames.push_back(std::wstring(StrBuffer.data()));
				StrBuffer.clear();

				sizeforwchar = 0;
				mbstowcs_s(&sizeforwchar, nullptr, 0, value, 0);
				StrBuffer.resize(sizeforwchar);
				mbstowcs_s(nullptr, StrBuffer.data(), sizeforwchar, value, _TRUNCATE);
				ImageDestinationList.push_back(std::wstring(StrBuffer.data()));
				lua_pop(this->LuaFileLoad, 1);
				StrBuffer.clear();
			}

			//Tải về hình ảnh -- To load a texture
			for (size_t i = 0; i < ImageDestinationList.size(); i++)
			{
				tn::Texture* LoadedTexturePackage = new tn::Texture();
				LoadedTexturePackage->Filename = ImageDestinationList[i];
				LoadedTexturePackage->UseFullSize = true;

				//NAME
				LoadedTexturePackage->SetName(TextureNames[i].data());

				const wchar_t* MainErrMsg = L"The program has failed to load the image! (";
				if (LoadedTexturePackage->ImportImage() == false)
				{
					const wchar_t* ImportErr = L")\nImport error!";
					const wchar_t* Format = L"%s %s %s";
					size_t sizeofbuffer = lstrlenW(MainErrMsg) + lstrlenW(LoadedTexturePackage->Filename.data()) + lstrlenW(ImportErr) + lstrlenW(Format);
					wchar_t* FullMsg = new wchar_t[sizeofbuffer];
					swprintf_s(FullMsg, sizeofbuffer, Format, MainErrMsg, LoadedTexturePackage->Filename, ImportErr);
					MessageBoxW(nullptr, FullMsg, L"ERROR", MB_ICONSTOP | MB_OK);
					delete[] FullMsg;
					return this->Stop();
				}

				if (this->MainGraphicDevice->ExportTexture(LoadedTexturePackage) == false)
				{
					const wchar_t* GraphicErr = L")\nThe graphic engine cannot load an image in the final stage!";
					const wchar_t* Format = L"%s %s %s";
					size_t sizeofbuffer = lstrlenW(MainErrMsg) + lstrlenW(LoadedTexturePackage->Filename.data()) + lstrlenW(GraphicErr) + lstrlenW(Format);
					wchar_t* FullMsg = new wchar_t[sizeofbuffer];
					swprintf_s(FullMsg, sizeofbuffer, Format, MainErrMsg, LoadedTexturePackage->Filename, GraphicErr);
					MessageBoxW(nullptr, FullMsg, L"ERROR", MB_ICONSTOP | MB_OK);
					delete[] FullMsg;
					return this->Stop();
				}

				//this->GameHierachy.Assets->AddChild(LoadedTexturePackage);
				this->GameHierachy.Textures.push_back(LoadedTexturePackage);
			}
		}

		ImageDestinationList.clear();

		/////////////////////////////////////////

		{
			lua_getglobal(this->LuaFileLoad, "TileTextures");
			lua_pushnil(this->LuaFileLoad);
			while (lua_next(this->LuaFileLoad, -2) != 0) {
				if (lua_isstring(this->LuaFileLoad, -1))
				{
					const char* value = lua_tostring(this->LuaFileLoad, -1);
					size_t sizeforwchar = 0;
					mbstowcs_s(&sizeforwchar, nullptr, 0, value, 0);
					StrBuffer.resize(sizeforwchar);
					mbstowcs_s(nullptr, StrBuffer.data(), sizeforwchar, value, _TRUNCATE);
					if (tn::Texture* tiletexture = this->GetFirstLoadedTexture(StrBuffer.data()))
					{
						this->TileTextures.push_back(tiletexture);
					}
					StrBuffer.clear();
				}

				lua_pop(this->LuaFileLoad, 1);
			}
		}

		////////////////////////////////////////////////
		lua_getglobal(this->LuaFileLoad, "sound_onwalkingtiles");
		if (!lua_istable(this->LuaFileLoad, -1)) {
			MessageBoxW(nullptr, L"Expected a table in a variable 'sound_onwalkingtiles'.\n", L"ERROR", MB_ICONSTOP | MB_OK);
			return this->Stop();
		}

		lua_pushnil(this->LuaFileLoad);
		while (lua_next(this->LuaFileLoad, -2) != 0) {
			if (lua_istable(this->LuaFileLoad, -1)) {
				lua_getfield(this->LuaFileLoad, -1, "Texture");
				lua_getfield(this->LuaFileLoad, -2, "Audio");

				const char* texture = lua_tostring(this->LuaFileLoad, -2);

				size_t sizeforwchar = 0;
				mbstowcs_s(&sizeforwchar, nullptr, 0, texture, 0);
				StrBuffer.resize(sizeforwchar);
				mbstowcs_s(nullptr, StrBuffer.data(), sizeforwchar, texture, _TRUNCATE);

				tn::Texture* TargetTexture = this->GetFirstLoadedTexture(StrBuffer.data());

				const char* audio = lua_tostring(this->LuaFileLoad, -1);

				mbstowcs_s(&sizeforwchar, nullptr, 0, audio, 0);
				StrBuffer.resize(sizeforwchar);
				mbstowcs_s(nullptr, StrBuffer.data(), sizeforwchar, audio, _TRUNCATE);

				tn::Sound* WalkSound = new tn::Sound();
				WalkSound->SetBuffer(dynamic_cast<tn::SoundBuffer*>(this->GameHierachy.Assets->GetChild(StrBuffer.data())));
				WalkSound->SetVolume(0.5f);

				this->TileWalkingSounds.push_back({ TargetTexture, WalkSound });
				lua_pop(this->LuaFileLoad, 2);
			}

			lua_pop(this->LuaFileLoad, 1);
		}
	}

	std::wcout << L"STAGE 2 FINISHED" << std::endl;

	lua_close(this->LuaFileLoad);

	CoreEvents.RenderStepped = new Task();
	CoreEvents.Heartbeat = new Task();

	this->LoadDebuggingClients();
	this->LoadLuaLibraries();

	this->Player = new Entity(this);
	this->Player->SetTexture(this->GetFirstLoadedTexture(L"Player"));
	this->Player->Size = { 25, 25 };

	RenderingThread = CreateThread(nullptr, 0, &this->GraphicEngineThreadFunction, this, 0, &ThreadID1);
	TaskThread = CreateThread(nullptr, 0, &TaskThreadFunction, this, 0, &ThreadID3);

	this->LoadGameItems();

	this->Main();
}

void Program::LoadGameItems()
{
	this->ListOfItems = {
		{L"camcorder", L"Camcorder", L"", this->GetFirstLoadedTexture(L"Camcorder"), 1, ItemType::Weapon, { 100.0f, 1.0f } },
		{L"wooden-block-item", L"Wooden Block", L"", this->GetFirstLoadedTexture(L"WoodenBlock"), 1, ItemType::ItemWithEntityData, {0,0}, { L"wooden-block" }},
	};

	this->ListOfEntities = {
		{L"block", L"wooden-block", L"Wooden Block", this->GetFirstLoadedTexture(L"WoodenBlock"), {L"placeable-neutral", L"player-creation"},
			{this->GetFirstLoadedTexture(L"WoodenBlock")}, {0.5, L"wooden-block-item"}, {PixelsOfTile,PixelsOfTile},
			{-PixelsOfTile / 2,PixelsOfTile / 2,-PixelsOfTile / 2,PixelsOfTile / 2},
			{-PixelsOfTile / 2,PixelsOfTile / 2,-PixelsOfTile / 2,PixelsOfTile / 2}, {1,1}, 100.0f
		},
	};
}

void Program::LoadLuaLibraries()
{
	this->ProgramEmbeddedFunctions = {
		{"wait", Lua29tnlibs::wait},
		{"IsRunning", Lua29tnlibs::lua_IsProgramRunning},
		{"CreateThread", Lua29tnlibs::lua_CreateThread},
		{"SuspendThread", Lua29tnlibs::lua_SuspendThread},
		{"ResumeThread", Lua29tnlibs::lua_ResumeThread},
		{"CloseThread", Lua29tnlibs::lua_CloseThread},
		{"WaitForThread", Lua29tnlibs::lua_WaitForThread},
		{"SetBackgroundColor", Lua29tnlibs::SetBackgroundColor},
		{"GetDate", Lua29tnlibs::lua_GetDate},
		{"GetDay", Lua29tnlibs::lua_GetDay},
		{"GetDayOfWeek", Lua29tnlibs::lua_GetDayOfTheWeek},
		{"GetMonth", Lua29tnlibs::lua_GetMonth},
		{"GetYear", Lua29tnlibs::lua_GetYear},
		{"GetTimeHour", Lua29tnlibs::lua_GetTimeHour},
		{"GetTimeMinute", Lua29tnlibs::lua_GetTimeMinute},
		{"GetTimeSecond", Lua29tnlibs::lua_GetTimeSecond},
		///////////////////////////////////////////////////////
		{"GetWorldObj", LuaObjectLibs::lua_GetWorldObject},
		{"GetGUIs", LuaObjectLibs::lua_GetGUIs},
		{"GetAssets", LuaObjectLibs::lua_GetAssets},
		{"GetParent", LuaObjectLibs::lua_GetParentObject},
		{"GetChild", LuaObjectLibs::lua_GetChildObject},
		{"FindFirstChild", LuaObjectLibs::lua_FindFirstChild},
		{"WaitForChild", LuaObjectLibs::lua_WaitForChild},
		{"ReleaseObj", LuaObjectLibs::lua_ReleaseObject},
		{"GetObjInfo", LuaObjectLibs::lua_GetInfoObject},
		{"ModifyObj", LuaObjectLibs::lua_ModifyObject},
		{"LoadTexture", LuaObjectLibs::lua_LoadTexture},
		{"CreateObject", LuaObjectLibs::lua_CreateObject},
		///////////////////////////////////////////////////////
		{"SetAudioBuffer", LuaObjectLibs::lua_SetAudioBuffer},
		{"PlaySound", LuaObjectLibs::lua_PlaySound},
		{"PauseSound", LuaObjectLibs::lua_PauseSound},
		{"StopSound", LuaObjectLibs::lua_StopSound},
	};

	std::wcout << L"STAGE 3 STARTED" << std::endl;
	std::wcout << L"Load Lua's 29_tringuyen libraries" << std::endl;

	LuaBase = luaL_newstate(this);
	LuaWeaponScript = luaL_newstate(this);
	luaL_openlibs(LuaBase);
	luaL_openlibs(LuaWeaponScript);

	for (size_t i = 0; i < ProgramEmbeddedFunctions.size(); i++) {
		lua_register(LuaBase, ProgramEmbeddedFunctions.at(i).name, ProgramEmbeddedFunctions.at(i).func);
		lua_register(LuaWeaponScript, ProgramEmbeddedFunctions.at(i).name, ProgramEmbeddedFunctions.at(i).func);
	}

	std::wcout << L"STAGE 3 FINISHED" << std::endl;
	std::wcout << L"The program is ready!" << std::endl;
}

void Program::LoadDebuggingClients()
{
	Folder* DebugFolder = new Folder(this);
	DebugFolder->SetName(L"DebugClient");

	this->Debug.RIText = new TextLabel(this);
	this->Debug.RIText->SetName(L"FPSText");
	this->Debug.RIText->Position = { 0,0,0,0 };
	this->Debug.RIText->Size = { 0,0,300,25 };
	this->Debug.RIText->BackgroundColor = { 0,0,0 };
	this->Debug.RIText->BackgroundTransparencyAlpha = 0.0f;
	this->Debug.RIText->TextColor = { 1,1,1 };
	this->Debug.RIText->TextSize = 15;
	this->Debug.RIText->ZDepth = 1.0f;
	this->Debug.RIText->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentLeft;
	this->Debug.RIText->Visible = false;
	this->Debug.RIText->Text = L"Render interval: ???";
	this->Debug.RIText->Font = L"Verdana";
	this->Debug.RIText->GraphicDevice = this->MainGraphicDevice;
	this->Debug.RIText->Load();

	DebugFolder->AddChild(this->Debug.RIText);

	this->Debug.FrequencyText = new TextLabel(this);
	this->Debug.FrequencyText->SetName(L"FPSText");
	this->Debug.FrequencyText->Position = { 0,0,0,25 };
	this->Debug.FrequencyText->Size = { 0,0,300,25 };
	this->Debug.FrequencyText->BackgroundColor = { 0,0,0 };
	this->Debug.FrequencyText->BackgroundTransparencyAlpha = 0.0f;
	this->Debug.FrequencyText->TextColor = { 1,1,1 };
	this->Debug.FrequencyText->TextSize = 15;
	this->Debug.FrequencyText->ZDepth = 1.0f;
	this->Debug.FrequencyText->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentLeft;
	this->Debug.FrequencyText->Visible = false;
	this->Debug.FrequencyText->Text = L"Frequency: ???";
	this->Debug.FrequencyText->Font = L"Verdana";
	this->Debug.FrequencyText->GraphicDevice = this->MainGraphicDevice;
	this->Debug.FrequencyText->Load();

	DebugFolder->AddChild(this->Debug.FrequencyText);

	this->Debug.MemoryUsageMBText = new TextLabel(this);
	this->Debug.MemoryUsageMBText->SetName(L"FPSText");
	this->Debug.MemoryUsageMBText->Position = { 0,0,0,50 };
	this->Debug.MemoryUsageMBText->Size = { 0,0,300,25 };
	this->Debug.MemoryUsageMBText->BackgroundColor = { 0,0,0 };
	this->Debug.MemoryUsageMBText->BackgroundTransparencyAlpha = 0.0f;
	this->Debug.MemoryUsageMBText->TextColor = { 1,1,1 };
	this->Debug.MemoryUsageMBText->TextSize = 15;
	this->Debug.MemoryUsageMBText->ZDepth = 1.0f;
	this->Debug.MemoryUsageMBText->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentLeft;
	this->Debug.MemoryUsageMBText->Visible = false;
	this->Debug.MemoryUsageMBText->Text = L"Memory usage: ???";
	this->Debug.MemoryUsageMBText->Font = L"Verdana";
	this->Debug.MemoryUsageMBText->GraphicDevice = this->MainGraphicDevice;
	this->Debug.MemoryUsageMBText->Load();

	DebugFolder->AddChild(this->Debug.MemoryUsageMBText);
	this->GameHierachy.GUI->AddChild(DebugFolder);
}

void Program::Main()
{
	this->MainWnd->Show(tn::WindowVisibleTypes::Show);
	//Task t = {};
	//t.Thread = this->MainScript;
	//this->AddTask(&t);
	MainScriptThread = CreateThread(nullptr, 0, this->MainScript, this, 0, 0);
	LARGE_INTEGER start, end, frequency;

	while (MainWnd->ProcessMessage()) {
		GetLocalTime(&this->LocalTime);

		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);
		Sleep(1);
		QueryPerformanceCounter(&end);

		this->interval = static_cast<float>(end.QuadPart - start.QuadPart) / frequency.QuadPart;
	}

	CloseHandle(MainScriptThread);
	Sleep(1);
	this->Stop();
}

void Program::PauseGameplay()
{
	if (ShadowScreenPause)
	{
		ShadowScreenPause->Alpha = 0.75f;
	}
	else
	{
		ShadowScreenPause = new Sprite(this);
		ShadowScreenPause->Size = { 10000, 10000 };
		ShadowScreenPause->Color = { 0.0f, 0.0f, 0.0f };
		ShadowScreenPause->Alpha = 0.75f;
		ShadowScreenPause->ZDepth = 1.0f;
		this->CurrentScene->AddObjectToScene(ShadowScreenPause);
	}

	if (PauseText)
	{
		PauseText->Visible = true;
	}
	else
	{
		PauseText = new TextLabel(this);
		PauseText->BackgroundTransparencyAlpha = 0.0f;
		PauseText->Text = L"PAUSED";
		PauseText->TextSize = 50;
		PauseText->Font = L"Times New Roman";
		PauseText->WordBreak = true;
		PauseText->GraphicDevice = this->MainGraphicDevice;
		PauseText->TextAlignment = TextAlignmentEnum::TextAlignmentCenter;
		PauseText->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentMiddle;
		PauseText->Size.Offset = this->CurrentCamera->ViewportSize;
		PauseText->TextColor = { 1.0f, 1.0f, 1.0f };
		PauseText->Visible = true;
		PauseText->ZDepth = 2.0f;
		PauseText->Load();
		this->GameHierachy.GUI->AddChild(PauseText);
	}

	this->GameplayState = GameplayStates::Pause;
	SuspendThread(this->GameplayScriptThread);
	Sleep(100); //Để chắc chắn (likely) cho khỏi có phần mềm bị lỗi khí có hàm PauseGameplay() được dùng lặp đi lặp lại.
}

void Program::ResumeGameplay()
{
	if (ShadowScreenPause)
	{
		ShadowScreenPause->Alpha = 0.0f;
	}

	if (PauseText)
	{
		PauseText->Visible = false;
	}

	this->GameplayState = GameplayStates::Run;
	ResumeThread(this->GameplayScriptThread);
}

void Program::EndGameplay()
{
	if (this->GameState == GameStates::Gameplay)
	{
		if (this->GameplayState != GameplayStates::End)
		{
			if (GetCurrentThreadId() == this->ThreadID3) //ThreadID3 = Task thread ID
			{
				if (ShadowScreenPause)
				{
					ShadowScreenPause->Alpha = 0.75f;
				}
				else
				{
					ShadowScreenPause = new Sprite(this);
					ShadowScreenPause->Size = { 10000, 10000 };
					ShadowScreenPause->Color = { 0.0f, 0.0f, 0.0f };
					ShadowScreenPause->Alpha = 0.75f;
					ShadowScreenPause->ZDepth = 1.0f;
					this->CurrentScene->AddObjectToScene(ShadowScreenPause);
				}

				this->GameplayState = GameplayStates::End;

				if (this->StaticGUIs.GameEndUI == nullptr)
				{
					this->StaticGUIs.GameEndUI = new MessageBoxUI(this);
					this->StaticGUIs.GameEndUI->SetName(L"GameEndUI");
					this->StaticGUIs.GameEndUI->BarSizeY = { 0, 50 };
					this->StaticGUIs.GameEndUI->BarColor = { 0.25f,0.25f,0.25f };
					this->StaticGUIs.GameEndUI->Size.Offset = { 450, 350 };
					this->StaticGUIs.GameEndUI->Position = { 0.5f, 0.5f, -(this->StaticGUIs.GameEndUI->Size.Offset.X / 2), -(this->StaticGUIs.GameEndUI->Size.Offset.Y / 2) };
					this->StaticGUIs.GameEndUI->BarText = this->GetLocaleText(this->CurrentLang.data(), L"gameover");
					this->StaticGUIs.GameEndUI->Visible = true;
					this->StaticGUIs.GameEndUI->Load();

					this->StaticGUIs.GameEndText = new TextLabel(this);
					this->StaticGUIs.GameEndText->SetName(L"GameEndUI");
					this->StaticGUIs.GameEndText->Size.Offset = { 450, 50 };
					this->StaticGUIs.GameEndText->Position.Offset = { 0, 50 };
					this->StaticGUIs.GameEndText->Text = this->GetLocaleText(this->CurrentLang.data(), L"thanksforplaying");
					this->StaticGUIs.GameEndText->Font = L"Verdana";
					this->StaticGUIs.GameEndText->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentLeft;
					this->StaticGUIs.GameEndText->TextSize = 24.0f;
					this->StaticGUIs.GameEndText->TextColor = { 1,1,1 };
					this->StaticGUIs.GameEndText->BackgroundTransparencyAlpha = 0.0f;
					this->StaticGUIs.GameEndText->Visible = true;
					this->StaticGUIs.GameEndText->Load();
					this->StaticGUIs.GameEndUI->AddChild(this->StaticGUIs.GameEndText);

					this->GameHierachy.GUI->AddChild(this->StaticGUIs.GameEndUI);
				}
				else
				{
					this->StaticGUIs.GameEndUI->Visible = true;
				}

				if (this->GameplayScriptThread)
				{
					DWORD resultofGST = WaitForSingleObject(this->GameplayScriptThread, 250);
					if (resultofGST == WAIT_TIMEOUT)
					{
						SuspendThread(this->GameplayScriptThread);
						Sleep(1);
					}
					CloseHandle(this->GameplayScriptThread);
					this->GameplayScriptThread = nullptr;
				}
			}
			else
			{
				this->CallToEndGameplay = true;
			}
		}
	}
}

void Program::Stop()
{
	this->TriggerStop1 = true;
	this->TriggerStop2 = true;

	if (LuaOpenThread)
	{
		DWORD result = WaitForSingleObject(LuaOpenThread, 1000);
		if (result == WAIT_TIMEOUT)
		{
			(void)(TerminateThread(LuaOpenThread, 0));
			GameplayScriptThread = nullptr;
		}
		else if (result != WAIT_FAILED) CloseHandle(LuaOpenThread);
	}

	if (GameplayScriptThread)
	{
		DWORD result2 = WaitForSingleObject(GameplayScriptThread, 1000);
		if (result2 == WAIT_TIMEOUT)
		{
			(void)(TerminateThread(GameplayScriptThread, 0));
			GameplayScriptThread = nullptr;
		}
		else if (result2 != WAIT_FAILED) CloseHandle(GameplayScriptThread);
	}

	if (MainScriptThread)
	{
		DWORD result3 = WaitForSingleObject(MainScriptThread, 1000);
		if (result3 == WAIT_TIMEOUT)
		{
			(void)(TerminateThread(MainScriptThread, 0));
			MainScriptThread = nullptr;
		}
		else if (result3 != WAIT_FAILED)
		{
			CloseHandle(MainScriptThread);
		}
	}

	WaitForSingleObject(RenderingThread, 5000);
	CloseHandle(RenderingThread);
	WaitForSingleObject(TaskThread, 5000);
	CloseHandle(TaskThread);

	Sleep(100);
	lua_close(LuaBase);

	HANDLE hMemoryResourceNotification = CreateMemoryResourceNotification(LowMemoryResourceNotification);

	GameHierachy.Scenes->EnableGradualCleanupWhenDestroyed();
	GameHierachy.Scenes->Release();
	Sleep(1000);
	GameHierachy.Assets->EnableGradualCleanupWhenDestroyed();
	GameHierachy.Assets->Release();
	Sleep(1000);
	GameHierachy.GUI->EnableGradualCleanupWhenDestroyed();
	GameHierachy.GUI->Release();
	Sleep(1000);

	tn::SafeReleaseWinSys(&this->CurrentCamera);
	tn::SafeReleaseWinSys(&this->MainSoundFramework);
	tn::SafeReleaseWinSys(&this->MainGraphicDevice);
	MainWnd->Destroy();
}

void Program::RenderObject(Program* m_Program, ProgramObject* obj)
{
	if (obj != nullptr) {
		if (obj->IsActive()) {
			if (Sprite* sprite = dynamic_cast<Sprite*>(obj))
			{
				sprite->Update();

				m_Program->MainGraphicDevice->SetWorldMatrix(tn::MatrixTranslation({ sprite->Position.X, sprite->Position.Y, sprite->ZDepth }));
				m_Program->MainGraphicDevice->SetTexture(0, sprite->Texture);
				m_Program->MainGraphicDevice->Draw(DrawType_TriangleStrip, sprite->Vertices, 4, 0, 2);
			}

			size_t s = 0;
			ObjArray children = obj->GetChildren(&s);
			if (s > 0)
			{
				for (size_t i = 0; i < s; i++)
				{
					if (children[i] != nullptr)
					{
						m_Program->RenderObject(m_Program, dynamic_cast<ProgramObject*>(children[i]));
					}
				}
			}
		}
	}
}

void Program::RenderUI(Program* m_Program, ProgramObject* obj, bool DescendentIsFolderRendering)
{
	if (obj != nullptr) {
		if (obj->IsActive()) {
			bool CanRenderChildren = true;
			bool CheckDescendentAsAFolder_StillAncestorOfGUIFolder_AndNotUI_Frame = DescendentIsFolderRendering;

			if (UI_Frame* uif = dynamic_cast<UI_Frame*>(obj))
			{
				if (uif->Visible == true)
				{
					if (DescendentIsFolderRendering == true)
					{
						uif->IsItsDescendentAFolderAndAncestorOfGUIFolder = true;
					}
					else
					{
						uif->IsItsDescendentAFolderAndAncestorOfGUIFolder = false;
					}
					CheckDescendentAsAFolder_StillAncestorOfGUIFolder_AndNotUI_Frame = false;
					uif->ReferenceCamera = m_Program->CurrentCamera;
					uif->GraphicDevice = m_Program->MainGraphicDevice;
					uif->Update();

					if (uif->IsSurface == true)
					{
						m_Program->MainGraphicDevice->SetWorldMatrix(tn::MatrixTranslation({ uif->AbsolutePosition.X, uif->AbsolutePosition.Y, uif->ZDepth }));
					}
					else
					{
						m_Program->MainGraphicDevice->SetWorldMatrix(tn::MatrixTranslation({ 0, 0, 0 }));
					}

					if (uif->CompiledTexture != nullptr)
					{
						m_Program->MainGraphicDevice->SetTexture(0, uif->CompiledTexture);
					}
					else
					{
						m_Program->MainGraphicDevice->SetTexture(0, nullptr);
					}
					m_Program->MainGraphicDevice->Draw(DrawType_TriangleStrip, uif->Vertices, 4, 0, 2);
				}
				else
				{
					CanRenderChildren = false;
				}
			}

			if (CanRenderChildren == true)
			{
				size_t s = 0;
				ObjArray children = obj->GetChildren(&s);
				if (s > 0)
				{
					for (size_t i = 0; i < s; i++)
					{
						if (children[i] != nullptr)
						{
							m_Program->RenderUI(m_Program, dynamic_cast<ProgramObject*>(children[i]), CheckDescendentAsAFolder_StillAncestorOfGUIFolder_AndNotUI_Frame);
						}
					}
				}
			}
		}
	}
}

DWORD CALLBACK Program::GraphicEngineThreadFunction(LPVOID ThreadParameter) {
	(void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	Program* m_Program = static_cast<Program*>(ThreadParameter);

	tn::Matrix4x4 defaultMatrix = tn::MatrixTranslation({ 0.0f,0.0f,0.0f }) * tn::MatrixScaling({ 1.0f,1.0f,1.0f });

	LARGE_INTEGER start, end, frequency = {};
	tn::Vector2 cviewport = {}, cviewoffset = {};
	float nearview = 0.0f, farview = 0.0f;

	while (m_Program->TriggerStop1 == false)
	{
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);

		///RENDERING
		Scene* SceneToRender = m_Program->CurrentScene;
		if (SceneToRender != nullptr)
		{
			size_t scenechildamount = 0;
			ObjArray ObjectsOfScene = SceneToRender->GetChildren(&scenechildamount);
			size_t UIamount = 0;
			ObjArray ObjectsOfUI = m_Program->GameHierachy.GUI->GetChildren(&UIamount);
			if (m_Program->CoreEvents.RenderStepped != nullptr)
			{
				/*
				if (m_Program->CoreEvents.RenderStepped->FiredCount <= 0)
				{
					float deltaTime = static_cast<float>(end.QuadPart - start.QuadPart) / frequency.QuadPart;
					float* pf = &deltaTime;
					m_Program->CoreEvents.RenderStepped->FunctionParameter = (void*)pf;
					m_Program->CoreEvents.RenderStepped->Fire();
				}
				*/
			}

			if (m_Program->CurrentCamera != nullptr)
			{
				cviewport = m_Program->CurrentCamera->ViewportSize;
				cviewoffset = m_Program->CurrentCamera->ViewportOffset;
				nearview = m_Program->CurrentCamera->NearView;
				farview = m_Program->CurrentCamera->FarView;

				m_Program->MainGraphicDevice->ClearDepth(1.0f);
				m_Program->MainGraphicDevice->ClearScreen({ m_Program->CurrentCamera->BackgroundColor.Red, m_Program->CurrentCamera->BackgroundColor.Green, m_Program->CurrentCamera->BackgroundColor.Blue });
				//std::cout << cviewport.X << "," << cviewport.Y << "," << nearview << "," << farview << std::endl;
				m_Program->projectionmatrix = tn::MatrixOrthographicProjection({ cviewoffset.X, cviewoffset.Y }, { cviewport.X + cviewoffset.X, cviewport.Y + cviewoffset.Y }, nearview, farview);
				m_Program->MainGraphicDevice->SetProjectionMatrix(m_Program->projectionmatrix);
				m_Program->MainGraphicDevice->SetWorldMatrix(defaultMatrix);
				m_Program->MainGraphicDevice->SetViewMatrix(tn::MatrixTranslation(m_Program->CurrentCamera->ViewportPosition) * tn::MatrixScaling({ 1.0f,1.0f,1.0f }));

				m_Program->MainGraphicDevice->BeginScene();
				if (scenechildamount > 0)
				{
					for (size_t i = 0; i < scenechildamount; i++)
					{
						if (Object* Obj = ObjectsOfScene[i])
						{
							if (ProgramObject* pObj = dynamic_cast<ProgramObject*>(Obj))
							{
								m_Program->RenderObject(m_Program, pObj);
							}
						}
					}
				}
				m_Program->MainGraphicDevice->EndScene();
				m_Program->MainGraphicDevice->ClearDepth(1.0f);
				m_Program->projectionmatrix = tn::MatrixOrthographicProjection({ 0,0 }, { cviewport.X, cviewport.Y }, nearview, farview);
				m_Program->MainGraphicDevice->SetProjectionMatrix(m_Program->projectionmatrix);
				m_Program->MainGraphicDevice->SetViewMatrix(tn::MatrixTranslation({ 0,0,0 }) * tn::MatrixScaling({ 1.0f,1.0f,1.0f }));
				m_Program->MainGraphicDevice->BeginScene();

				if (UIamount > 0)
				{
					for (size_t i = 0; i < UIamount; i++)
					{
						ProgramObject* obj = dynamic_cast<ProgramObject*>(ObjectsOfUI[i]);
						if (obj != nullptr)
						{
							m_Program->RenderUI(m_Program, obj, true);
						}
					}
				}

				m_Program->MainGraphicDevice->EndScene();
				m_Program->MainGraphicDevice->Present();
			}
		}
		else
		{
			Sleep(1);
		}
		QueryPerformanceCounter(&end);
		m_Program->ProgramAnalysis.RenderInterval = (end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;
		m_Program->ProgramAnalysis.Frequency = frequency.QuadPart;

		if (K32GetProcessMemoryInfo(GetCurrentProcess(), &m_Program->ProgramAnalysis.pmc, sizeof(m_Program->ProgramAnalysis.pmc))) {
			m_Program->ProgramAnalysis.MemoryUsageMB = m_Program->ProgramAnalysis.pmc.WorkingSetSize / (1024 * 1024);
		}
	}

	CoUninitialize();
	return 0UL;
}

#define CheckUIForMouse_HasChildTouched 1

void Program::UntouchUIsForMouse(ProgramObject* Obj)
{
	if (Obj->IsActive())
	{
		size_t numberofchildren = 0;
		ObjArray UIs = Obj->GetChildren(&numberofchildren);
		if (numberofchildren > 0)
		{
			size_t i = 0;
			for (i = 0; i < numberofchildren; i++)
			{
				ProgramObject* obj = dynamic_cast<ProgramObject*>(UIs[i]);
				if (obj->IsActive())
				{
					if (UI_Frame* UIElement = dynamic_cast<UI_Frame*>(obj))
					{
						if (UIElement != this->WhichUI_MouseIsTouching)
						{
							if (UIElement->IsCursorTouching == true)
							{
								if (UIElement->MouseLeaveFunction != nullptr)
								{
									UIElement->MouseLeaveFunction(this, UIElement, this->Mouse);
								}
							}
							UIElement->IsCursorTouching = false;
						}
					}
				}
			}
		}
	}
}

void Program::CheckUIForMouse(ProgramObject* Obj, bool* IsChildTouched)
{
	if (this->WhichUI_MouseIsTouching != nullptr)
	{
		if (tn::IsPointWithinRect(this->Mouse->Position, this->WhichUI_MouseIsTouching->TouchableRect) == false)
		{
			this->WhichUI_MouseIsTouching = nullptr;
		}
	}

	if (Obj->IsActive())
	{
		this->UntouchUIsForMouse(Obj);

		size_t numberofchildren = 0;
		ObjArray CurrentListDescendants = Obj->GetChildren(&numberofchildren);
		if (numberofchildren > 0)
		{
			//Chuột có chạm vào gì không?
			size_t i = numberofchildren;
			for (i = numberofchildren; i > 0; i--)
			{
				ProgramObject* obj = dynamic_cast<ProgramObject*>(CurrentListDescendants[i - 1]);
				if (obj->IsActive())
				{
					if (UI_Frame* UIElement = dynamic_cast<UI_Frame*>(obj))
					{
						if (UIElement->Visible == true)
						{
							/*
							int hasChildTouched = this->CheckUIForMouse(UIElement); //[insert child here]
							if (hasChildTouched == CheckUIForMouse_HasChildTouched)
							{
								std::vector<Object*> v = this->GameHierachy.GUI->GetChildrenVector();
								auto it = std::find(v.begin(), v.end(), UIElement);
								if (it != v.end())
								{
									for (Object* BackObj : v)
									{
										if (UI_Frame* UIElementInTheBack = dynamic_cast<UI_Frame*>(BackObj))
										{
											callconnectfunction(UIElementInTheBack);
											UIElementInTheBack->IsCursorTouching = false;
										}
									}
									v.clear();
								}
								return hasChildTouched;
							}
							*/

							bool ischildtouched = false;
							this->CheckUIForMouse(UIElement, &ischildtouched);

							if (ischildtouched == false)
							{
								if (tn::IsPointWithinRect(this->Mouse->Position, UIElement->TouchableRect) == true)
								{
									if (UIElement->IsCursorTouching == false)
									{
										if (UIElement->MouseEnterFunction != nullptr)
										{
											UIElement->MouseEnterFunction(this, UIElement, this->Mouse);
										}
									}
									UIElement->IsCursorTouching = true;
									/*
									if (this->WhichUI_MouseIsTouching != nullptr)
									{
										if (this->WhichUI_MouseIsTouching != UIElement)
										{
											std::vector<Object*> v = this->GameHierachy.GUI->GetChildrenVector();
											auto it = std::find(v.begin(), v.end(), this->WhichUI_MouseIsTouching);
											if (it != v.end())
											{
												std::vector<Object*> BackObjList = {};
												BackObjList.insert(BackObjList.begin(), it, v.end());
												for (Object* BackObj : BackObjList)
												{
													if (UI_Frame* UIElementInTheBack = dynamic_cast<UI_Frame*>(BackObj))
													{
														UIElementInTheBack->IsCursorTouching = false;
													}
												}
											}
										}
									}
									*/
									this->WhichUI_MouseIsTouching = UIElement;
									if (IsChildTouched != nullptr)
									{
										(*IsChildTouched) = true;
									}
									break;
								}
							}
							else
							{
								if (IsChildTouched != nullptr)
								{
									(*IsChildTouched) = true;
								}
							}
						}
					}
				}
			}
		}
	}
}

void Program::CheckCollidersForMouse(ProgramObject* Obj)
{
	if (Obj != nullptr)
	{
		if (Obj->IsActive())
		{
			std::vector<Object*> ObjectList = Obj->GetChildrenVector();
			if (ObjectList.size() > 0)
			{
				//Chuột có chạm vào gì không?
				for (size_t i = ObjectList.size(); i > 0; i--)
				{
					ProgramObject* expected_collider = dynamic_cast<ProgramObject*>(ObjectList.at(i - 1));
					if (expected_collider != nullptr)
					{
						if (expected_collider->IsActive())
						{
							if (Collider2D* ActualCollider = dynamic_cast<Collider2D*>(expected_collider))
							{
								if (ActualCollider->IsActive())
								{
									if (ActualCollider->BaseObject != nullptr)
									{
										if (Sprite* SpriteObj = dynamic_cast<Sprite*>(ActualCollider->BaseObject))
										{
											tn::Rect rt = {};
											tn::Vector2 Camera2DPos = { this->CurrentCamera->ViewportPosition.X, this->CurrentCamera->ViewportPosition.Y };
											tn::Vector3 Vector3ofObj = { SpriteObj->Position.X, SpriteObj->Position.Y, SpriteObj->ZDepth };
											/*
											tn::Vector3 defaultscale = { 1, 1, 1 };
											tn::Matrix4x4 defaultrotation = tn::MatrixRotationX(0) * tn::MatrixRotationY(0) * tn::MatrixRotationZ(0);
											tn::Matrix4x4 world = tn::MatrixScaling(defaultscale) * tn::MatrixTranslation(Vector3ofObj);
											tn::Matrix4x4 view = tn::MatrixTranslation(this->CurrentCamera->ViewportPosition);
											tn::Matrix4x4 viewmatrix = world * view;
											tn::Matrix4x4 clip = viewmatrix * projectionmatrix;
											tn::Vertex vertices[4] = { SpriteObj->Vertices[0], SpriteObj->Vertices[1], SpriteObj->Vertices[2], SpriteObj->Vertices[3] };
											float w1, w2, w3, w4;
											tn::TransformVertex(&vertices[0], clip, &w1);
											tn::TransformVertex(&vertices[1], clip, &w2);
											tn::TransformVertex(&vertices[2], clip, &w3);
											tn::TransformVertex(&vertices[3], clip, &w4);

											tn::Vertex screen_vertices[4] = {};
											screen_vertices[0].Position.X = ((vertices[0].Position.X / w1) + 1) / 2; //* this->CurrentCamera->ViewportSize.X;
											screen_vertices[0].Position.Y = (1 - (vertices[0].Position.Y / w1)) / 2; //* this->CurrentCamera->ViewportSize.Y;
											screen_vertices[1].Position.X = ((vertices[1].Position.X / w2) + 1) / 2; //* this->CurrentCamera->ViewportSize.X;
											screen_vertices[1].Position.Y = (1 - (vertices[1].Position.Y / w2)) / 2; //* this->CurrentCamera->ViewportSize.Y;
											screen_vertices[2].Position.X = ((vertices[2].Position.X / w3) + 1) / 2; //* this->CurrentCamera->ViewportSize.X;
											screen_vertices[2].Position.Y = (1 - (vertices[2].Position.Y / w3)) / 2; //* this->CurrentCamera->ViewportSize.Y;
											screen_vertices[3].Position.X = ((vertices[3].Position.X) / w4 + 1) / 2; //* this->CurrentCamera->ViewportSize.X;
											screen_vertices[3].Position.Y = (1 - (vertices[3].Position.Y / w4)) / 2; //* this->CurrentCamera->ViewportSize.Y;

											tn::Convert2DVertexIntoRect(screen_vertices, &rt);
											*/
											tn::Convert2DVertexIntoRect(SpriteObj->Vertices, &rt);
											tn::TransformRect(&rt, Vector3ofObj);
											tn::TransformRect(&rt, Camera2DPos - this->CurrentCamera->ViewportOffset);

											if (tn::IsPointWithinRect(this->Mouse->Position, rt) == true)
											{
												ActualCollider->IsCursorTouching = true;
												if (this->Mouse->LeftMouseDown == true)
												{
													if (ActualCollider->ColliderMousePressed == false)
													{
														ActualCollider->ColliderMousePressed = true;
														if (ActualCollider->MouseClickFunc != nullptr)
														{
															ActualCollider->MouseClickFunc(ActualCollider, this->Mouse);
														}
													}

													/*
													if (ActualCollider->OnMouseButton1DownFunction != nullptr)
													{
														ActualCollider->OnMouseButton1DownFunction(m_Program, tb, m_Program->Mouse);
													}
													*/
												}
												else if (this->Mouse->LeftMouseUp == true)
												{
													if (ActualCollider->ColliderMousePressed == true)
													{
														ActualCollider->ColliderMousePressed = false;
														/*
														if (ActualCollider->OnMouseButton1UpFunction != nullptr)
														{
															ActualCollider->OnMouseButton1UpFunction(m_Program, tb, m_Program->Mouse);
														}
														*/
													}
												}
												else
												{
													ActualCollider->ColliderMousePressed = false;
												}
											}
											else
											{
												ActualCollider->IsCursorTouching = false;
											}
										}
										else
										{
											ActualCollider->IsCursorTouching = false;
										}
									}
									else
									{
										ActualCollider->IsCursorTouching = false;
									}
								}
								else
								{
									ActualCollider->IsCursorTouching = false;
								}
							}

							size_t childrenamount = 0;
							expected_collider->GetChildren(&childrenamount);
							if (childrenamount != 0)
							{
								this->CheckCollidersForMouse(expected_collider);
							}
						}
					}
				}
			}
			ObjectList.clear();
		}
	}
}

bool TabAlreadyDown = true;
bool KeyEAlreadyDown = true;

bool MouseOnTile = false;
bool IsHighlightedPlaceableEntityCollided = false;
DWORD CALLBACK Program::TaskThreadFunction(LPVOID ThreadParameter)
{
	(void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	Program* m_Program = static_cast<Program*>(ThreadParameter);

	LARGE_INTEGER start, end, frequency = {};
	float interval = 0.0f;
	Sprite* WhichTilePlayerSteppedOn = nullptr;

	while (m_Program->TriggerStop2 == false)
	{
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);
		Sleep(1);
		//m_Program->CoreEvents.Heartbeat->Fire();

		///DEBUGGING CLIENTS 
		if (m_Program->Debug.ShowDebug == true)
		{
			m_Program->Debug.RIText->Text = L"Render interval: " + std::to_wstring(m_Program->ProgramAnalysis.RenderInterval) + L" ms";
			m_Program->Debug.RIText->Visible = true;
			m_Program->Debug.FrequencyText->Text = L"Frequency (Interval): " + std::to_wstring(m_Program->ProgramAnalysis.Frequency) + L" Hz";
			m_Program->Debug.FrequencyText->Visible = true;
			m_Program->Debug.MemoryUsageMBText->Text = L"Memory usage: ~" + std::to_wstring(m_Program->ProgramAnalysis.MemoryUsageMB) + L" MB";
			m_Program->Debug.MemoryUsageMBText->Visible = true;
		}
		else
		{
			m_Program->Debug.RIText->Visible = false;
			m_Program->Debug.FrequencyText->Visible = false;
			m_Program->Debug.MemoryUsageMBText->Visible = false;
		}

		///WORLD TASKING
		RECT rt = {};
		GetClientRect(m_Program->MainWnd->GetHWND(), &rt);

		m_Program->CheckCollidersForMouse(m_Program->CurrentScene);

		if (m_Program->GameState == GameStates::MapEditor)
		{
			float HalfWorldWidth = (m_Program->CurrentScene->Description.WorldWidth * PixelsOfTile + PixelsOfTile) / 2;
			float HalfWorldHeight = (m_Program->CurrentScene->Description.WorldHeight * PixelsOfTile + PixelsOfTile) / 2;

			bool PressLeft = false, PressRight = false, PressTop = false, PressBottom = false;
			tn::Vector2 inputDir = { 0.f, 0.f };
			if (m_Program->KeyPressed.KeyW) {
				inputDir.Y -= 1.f; PressTop = true;
			};
			if (m_Program->KeyPressed.KeyS) {
				inputDir.Y += 1.f; PressBottom = true;
			};
			if (m_Program->KeyPressed.KeyA) {
				inputDir.X -= 1.f; PressLeft = true;
			};
			if (m_Program->KeyPressed.KeyD) {
				inputDir.X += 1.f; PressRight = true;
			}

			const float dt = interval;
			const float speed = m_Program->PlayerMovementSpeed;
			tn::Vector2 movement = inputDir * (speed * dt);

			tn::Rect PlayerRect = {};
			PlayerRect.Left = -m_Program->Player->Size.X / 2;
			PlayerRect.Right = m_Program->Player->Size.X / 2;
			PlayerRect.Top = -m_Program->Player->Size.Y / 2;
			PlayerRect.Bottom = m_Program->Player->Size.Y / 2;

			tn::Vector2 ExpectedPlayerPos = m_Program->Player->Position + movement;

			tn::Rect ExpectedTransformedPlayerRect = PlayerRect;
			tn::TransformRect(&ExpectedTransformedPlayerRect, ExpectedPlayerPos);

			{
				if (ExpectedTransformedPlayerRect.Left < -HalfWorldWidth)
				{
					ExpectedPlayerPos.X = -HalfWorldWidth - PlayerRect.Left;
				}

				if (ExpectedTransformedPlayerRect.Right > HalfWorldWidth)
				{
					ExpectedPlayerPos.X = HalfWorldWidth - PlayerRect.Right;
				}

				if (ExpectedTransformedPlayerRect.Top < -HalfWorldHeight)
				{
					ExpectedPlayerPos.Y = -HalfWorldHeight - PlayerRect.Top;
				}

				if (ExpectedTransformedPlayerRect.Bottom > HalfWorldHeight)
				{
					ExpectedPlayerPos.Y = HalfWorldHeight - PlayerRect.Bottom;
				}
			}

			m_Program->Player->Position = ExpectedPlayerPos;

			if (m_Program->Mouse->LeftMouseDown == true)
			{
				if (m_Program->Toolbox.BuildMode == true and MouseOnTile == true)
				{
					if (m_Program->WhichUI_MouseIsTouching == nullptr and m_Program->Toolbox.HighlightedTileInBuildMode != nullptr)
					{
						if (IsHighlightedPlaceableEntityCollided == false)
						{
							if (m_Program->Toolbox.HighlightedPlaceableEntity != nullptr)
							{
								if (m_Program->Toolbox.SelectedInventoryBoxWithEntityData)
								{
									m_Program->PlaceEntity(m_Program->FindEntity(m_Program->Toolbox.SelectedInventoryBoxWithEntityData->ReferenceInventoryItem->BaseItem->EntityDataClass.ReferenceEntityProtoName.data()), m_Program->Toolbox.HighlightedTileInBuildMode->Position);
								}
							}
						}
					}
				}
			}

			if (m_Program->Toolbox.BuildMode == true)
			{
				if (m_Program->Toolbox.SelectedInventoryBoxWithEntityData)
				{
					m_Program->Toolbox.SelectedInventoryBoxWithEntityData->BackgroundColor = { 0.0,1.0f,0.0 };
				}

				if (m_Program->Toolbox.HighlightedPlaceableEntity != nullptr)
				{
					if (IsHighlightedPlaceableEntityCollided == true)
					{
						m_Program->Toolbox.HighlightedPlaceableEntity->Color = { 1,0,0 };
					}
					else
					{
						m_Program->Toolbox.HighlightedPlaceableEntity->Color = { 0,1,0 };
					}
					m_Program->Toolbox.HighlightedPlaceableEntity->SetParent(m_Program->CurrentScene);
				}
				else
				{
					m_Program->Toolbox.HighlightedPlaceableEntity = new Sprite(m_Program);
					m_Program->Toolbox.HighlightedPlaceableEntity->SetName(L"HighlightedEntity");
					m_Program->Toolbox.HighlightedPlaceableEntity->Size = { 5,5 };
					m_Program->Toolbox.HighlightedPlaceableEntity->Color = { 0,1,0 };
					m_Program->Toolbox.HighlightedPlaceableEntity->Alpha = 1.0f;
					m_Program->Toolbox.HighlightedPlaceableEntity->SetParent(m_Program->CurrentScene);
				}

				if (m_Program->WhichUI_MouseIsTouching == nullptr)
				{
					if (Scene* cs = m_Program->CurrentScene)
					{
						if (TerrainFolder* Tiles = cs->Terrain)
						{
							size_t numberofchildren = 0;
							ObjArray array = ObjArray(Tiles->GetChildren(&numberofchildren));

							bool IsMouseOnTile = false;
							for (size_t i = 0; i < numberofchildren; i++)
							{
								if (Sprite* tile = dynamic_cast<Sprite*>(array[i]))
								{
									tn::Rect rectoftile = {
										-tile->Size.X / 2,
										tile->Size.X / 2,
										-tile->Size.Y / 2,
										tile->Size.Y / 2,
									};
									tn::TransformRect(&rectoftile, tile->Position);
									tn::Vector2 Camera2DPos = { m_Program->CurrentCamera->ViewportPosition.X, m_Program->CurrentCamera->ViewportPosition.Y };
									tn::TransformRect(&rectoftile, Camera2DPos - m_Program->CurrentCamera->ViewportOffset);
									if (tn::IsPointWithinRect(m_Program->Mouse->Position, rectoftile) == true)
									{
										if (m_Program->Toolbox.HighlightedPlaceableEntity != nullptr)
										{
											m_Program->Toolbox.HighlightedPlaceableEntity->Position = tile->Position;
											m_Program->Toolbox.HighlightedPlaceableEntity->Size = {
												PixelsOfTile, PixelsOfTile
											};
											IsMouseOnTile = true;
											MouseOnTile = true;
											m_Program->Toolbox.HighlightedPlaceableEntity->SetParent(m_Program->CurrentScene);
										}
										m_Program->Toolbox.HighlightedTileInBuildMode = tile;
										break;
									}
								}
							}

							if (IsMouseOnTile == false)
							{
								MouseOnTile = false;
								m_Program->Toolbox.HighlightedTileInBuildMode = nullptr;
								if (m_Program->Toolbox.HighlightedPlaceableEntity != nullptr)
								{
									m_Program->Toolbox.HighlightedPlaceableEntity->SetParent(nullptr);
								}
							}
						}

						if (Workspace* workspace = cs->SceneWorkspace)
						{
							size_t numberofentities = 0;
							ObjArray array = ObjArray(workspace->GetChildren(&numberofentities));

							bool IsMouseOnTile = false;
							for (size_t i = 0; i < numberofentities; i++)
							{
								if (Entity* ent = dynamic_cast<Entity*>(array[i]))
								{
									tn::Rect rectofentity = {
										-ent->Size.X / 2 + 0.01,
										ent->Size.X / 2 - 0.01,
										-ent->Size.Y / 2 + 0.01,
										ent->Size.Y / 2 - 0.01,
									};
									tn::TransformRect(&rectofentity, ent->Position);
									if (m_Program->Toolbox.HighlightedPlaceableEntity != nullptr)
									{
										tn::Rect RectOfHPE = m_Program->Toolbox.HighlightedPlaceableEntity->GetRect();
										if (tn::AABB_Collision(RectOfHPE, rectofentity) == true)
										{
											IsHighlightedPlaceableEntityCollided = true;
											break;
										}
										else
										{
											IsHighlightedPlaceableEntityCollided = false;
										}
									}
								}
							}
						}
					}
				}
				else
				{
					MouseOnTile = false;
					m_Program->Toolbox.HighlightedTileInBuildMode = nullptr;
					if (m_Program->Toolbox.HighlightedPlaceableEntity != nullptr)
					{
						m_Program->Toolbox.HighlightedPlaceableEntity->SetParent(nullptr);
					}
				}
			}
			else
			{
				m_Program->Toolbox.HighlightedTileInBuildMode = nullptr;
				if (m_Program->Toolbox.HighlightedPlaceableEntity != nullptr)
				{
					m_Program->Toolbox.HighlightedPlaceableEntity->SetParent(nullptr);
				}
			}

			if (m_Program->Mouse->RightMouseDown) {
				if (Scene* cs = m_Program->CurrentScene)
				{
					if (Workspace* workspace = cs->SceneWorkspace)
					{
						size_t numberofentities = 0;
						ObjArray array = ObjArray(workspace->GetChildren(&numberofentities));

						bool IsMouseOnTile = false;
						for (size_t i = 0; i < numberofentities; i++)
						{
							if (Entity* ent = dynamic_cast<Entity*>(array[i]))
							{
								if (ent->IsActive() == true)
								{
									tn::Rect rectofentity = {
										-ent->Size.X / 2 + 0.01,
										ent->Size.X / 2 - 0.01,
										-ent->Size.Y / 2 + 0.01,
										ent->Size.Y / 2 - 0.01,
									};
									tn::TransformRect(&rectofentity, ent->Position);
									tn::Vector2 Camera2DPos = { m_Program->CurrentCamera->ViewportPosition.X, m_Program->CurrentCamera->ViewportPosition.Y };
									tn::TransformRect(&rectofentity, Camera2DPos - m_Program->CurrentCamera->ViewportOffset);
									if (tn::IsPointWithinRect(m_Program->Mouse->Position, rectofentity) == true)
									{
										ent->Destroy();
										break;
									}
								}
							}
						}
					}
				}
			}

			m_Program->CurrentCamera->ViewportPosition = { -m_Program->Player->Position.X, -m_Program->Player->Position.Y, 0 };
		}
		else if (m_Program->GameState == GameStates::Gameplay)
		{
			if (m_Program->GameplayState == GameplayStates::Run)
			{
				if (m_Program->CurrentScene->Description.Type == ST_2DSurface)
				{
					bool CanPlayWalkSound = false;
					//UP AND DOWN
#pragma region Keys
					float HalfWorldWidth = (m_Program->CurrentScene->Description.WorldWidth * PixelsOfTile + PixelsOfTile) / 2;
					float HalfWorldHeight = (m_Program->CurrentScene->Description.WorldHeight * PixelsOfTile + PixelsOfTile) / 2;
					bool PressLeft = false, PressRight = false, PressTop = false, PressBottom = false;

					tn::Vector2 inputDir = { 0.f, 0.f };
					if (m_Program->KeyPressed.KeyW) {
						inputDir.Y -= 1.f; PressTop = true; CanPlayWalkSound = true;
					};
					if (m_Program->KeyPressed.KeyS) {
						inputDir.Y += 1.f; PressBottom = true; CanPlayWalkSound = true;
					};
					if (m_Program->KeyPressed.KeyA) {
						inputDir.X -= 1.f; PressLeft = true; CanPlayWalkSound = true;
					};
					if (m_Program->KeyPressed.KeyD) {
						inputDir.X += 1.f; PressRight = true; CanPlayWalkSound = true;
					}

					const float dt = interval;
					const float speed = m_Program->PlayerMovementSpeed;
					tn::Vector2 movement = inputDir * (speed * dt);

					tn::Vector2 ExpectedPlayerPos = m_Program->Player->Position + movement;
					tn::Rect PlayerRect = {};
					PlayerRect.Left = -m_Program->Player->Size.X / 2;
					PlayerRect.Right = m_Program->Player->Size.X / 2;
					PlayerRect.Top = -m_Program->Player->Size.Y / 2;
					PlayerRect.Bottom = m_Program->Player->Size.Y / 2;

					tn::Rect ExpectedTransformedPlayerRect = PlayerRect;
					tn::TransformRect(&ExpectedTransformedPlayerRect, ExpectedPlayerPos);

					//X handle
					Entity* TouchedEntity = nullptr;
					bool Touch = false;

					{
						for (size_t i = 0; i < m_Program->CurrentScene->SceneWorkspace->Children->Size(); i++)
						{
							if (Entity* entity = dynamic_cast<Entity*>(m_Program->CurrentScene->SceneWorkspace->Children->operator[](i)))
							{
								if (entity != m_Program->Player)
								{
									tn::Rect rt = entity->GetRect();
									if (tn::AABB_Collision(ExpectedTransformedPlayerRect, rt) == true)
									{
										if (ExpectedTransformedPlayerRect.Right >= rt.Left and ExpectedTransformedPlayerRect.Left <= rt.Right)
										{
											TouchedEntity = entity;
											Touch = true;
										}
									}
								}
							}
						}
					}

					{
						if (ExpectedTransformedPlayerRect.Left < -HalfWorldWidth)
						{
							ExpectedPlayerPos.X = -HalfWorldWidth - PlayerRect.Left;
						}

						if (ExpectedTransformedPlayerRect.Right > HalfWorldWidth)
						{
							ExpectedPlayerPos.X = HalfWorldWidth - PlayerRect.Right;
						}

						if (ExpectedTransformedPlayerRect.Top < -HalfWorldHeight)
						{
							ExpectedPlayerPos.Y = -HalfWorldHeight - PlayerRect.Top;
						}

						if (ExpectedTransformedPlayerRect.Bottom > HalfWorldHeight)
						{
							ExpectedPlayerPos.Y = HalfWorldHeight - PlayerRect.Bottom;
						}
					}

					if (Touch == true)
					{
						if (TouchedEntity != nullptr)
						{
							ExpectedPlayerPos = m_Program->Player->Position;
						}
					}

					m_Program->Player->Position = ExpectedPlayerPos;

					//////////////////

					if (m_Program->KeyPressed.KeyE == true)
					{
						if (KeyEAlreadyDown == false)
						{
							KeyEAlreadyDown = true;
							if (m_Program->Toolbox.CharacterInventory.Body != nullptr)
							{
								if (m_Program->Toolbox.CharacterInventory.Body->Visible == true)
								{
									m_Program->Toolbox.CharacterInventory.Body->Visible = false;
								}
								else
								{
									m_Program->Toolbox.CharacterInventory.Body->Visible = true;
								}
							}
						}
					}
					else
					{
						KeyEAlreadyDown = false;
					}

					if (m_Program->KeyPressed.KeyQ == true)
					{
						if (m_Program->Toolbox.SelectedInventoryBoxWithEntityData)
						{
							m_Program->Toolbox.SelectedInventoryBoxWithEntityData->BackgroundColor = { 0.0f, 0.0f, 0.0f };
							//m_Program->Toolbox.SelectedInventoryBoxWithEntityData = nullptr;
							m_Program->Toolbox.BuildMode = false;
						}
					}

					if (m_Program->KeyPressed.KeyTab == true)
					{
						if (TabAlreadyDown == false)
						{
							TabAlreadyDown = true;
							m_Program->Toolbox.ActiveItemIndex++;
							if (m_Program->Toolbox.ActiveItemIndex >= m_Program->NumberOfActiveItemSlots)
							{
								m_Program->Toolbox.ActiveItemIndex = 0;
							}
						}
					}
					else
					{
						TabAlreadyDown = false;
					}

					if (m_Program->RechargeTime <= 0)
					{
						if (m_Program->KeyPressed.Space == true)
						{
							if (InventoryItem* invitem = m_Program->Inventory.ActiveItemSlots[m_Program->Toolbox.ActiveItemIndex])
							{
								if (ItemDescription* itemdesc = invitem->BaseItem)
								{
									std::wstring wdirectoryfilename = L"./scripts/base/prototypes/" + itemdesc->PrototypeName + L".lua";

									size_t sizeforwchar = 0;
									wcstombs_s(&sizeforwchar, nullptr, 0, wdirectoryfilename.data(), 0);

									char* newcharstring = new char[sizeforwchar];
									wcstombs_s(nullptr, newcharstring, sizeforwchar, wdirectoryfilename.data(), _TRUNCATE);

									luaL_dofile(m_Program->LuaWeaponScript, newcharstring);

									delete[] newcharstring;

									m_Program->RechargeTimeStart = itemdesc->WeaponClass.Recharge;
									m_Program->RechargeTime = itemdesc->WeaponClass.Recharge;
								}
							}
						}
					}
					else
					{
						m_Program->RechargeTime -= interval;
					}

					if (m_Program->Mouse->LeftMouseDown == true)
					{
						if (m_Program->Toolbox.BuildMode == true and MouseOnTile == true)
						{
							if (m_Program->WhichUI_MouseIsTouching == nullptr and m_Program->Toolbox.HighlightedTileInBuildMode != nullptr)
							{
								if (m_Program->Toolbox.HighlightedPlaceableEntity != nullptr)
								{
									if (IsHighlightedPlaceableEntityCollided == false)
									{
										if (m_Program->Toolbox.SelectedInventoryBoxWithEntityData)
										{
											m_Program->PlaceEntity(m_Program->FindEntity(m_Program->Toolbox.SelectedInventoryBoxWithEntityData->ReferenceInventoryItem->BaseItem->EntityDataClass.ReferenceEntityProtoName.data()), m_Program->Toolbox.HighlightedTileInBuildMode->Position);
											if (InventoryItem* it = m_Program->Toolbox.SelectedInventoryBoxWithEntityData->ReferenceInventoryItem)
											{
												it->Size -= 1;
											}
											m_Program->Toolbox.SelectedInventoryBoxWithEntityData->BackgroundColor = { 0.0f, 0.0f, 0.0f };
											//m_Program->Toolbox.SelectedInventoryBoxWithEntityData = nullptr;
											m_Program->Toolbox.BuildMode = false;
										}
									}
									else
									{
										std::wcout << L"Character is on the way!" << std::endl;
									}
								}
							}
						}
					}

					if (m_Program->Mouse->LeftMouseUp == true and m_Program->Mouse->LeftMouseHolded == false)
					{
						bool is_index_taken = false;
						size_t index_taken = 0;

						if (GameplayRelatedObjects::SelectedInventoryItemBox* dtib = m_Program->Toolbox.DraggedTool_InventoryBox)
						{
							if (InventoryItem* inv = dtib->ReferenceInventoryItem)
							{
								m_Program->Toolbox.DraggedTool_InventoryBox->Visible = false;
								for (size_t i = 0; i < m_Program->NumberOfActiveItemSlots; i++)
								{
									if (tn::IsPointWithinRect(m_Program->Mouse->Position, m_Program->Toolbox.ActiveSlots[i]->TouchableRect) == true)
									{
										if (ItemDescription* id = inv->BaseItem)
										{
											if (id->Type == ItemType::Weapon)
											{
												m_Program->Inventory.ActiveItemSlots[i] = inv;
												index_taken = i;
												is_index_taken = true;
												break;
											}
										}
									}
								}

								if (is_index_taken == true)
								{
									for (size_t i = 0; i < m_Program->NumberOfActiveItemSlots; i++)
									{
										if (i != index_taken)
										{
											InventoryItem* slot = m_Program->Inventory.ActiveItemSlots[i];
											if (slot == inv)
											{
												m_Program->Inventory.ActiveItemSlots[i] = nullptr;
											}
										}
									}
								}
							}
						}
					}
#pragma endregion
#pragma region Tile System
					bool IsNewTileFound = false;
					if (CanPlayWalkSound == true)
					{
						size_t numberofchildren = 0;
						ObjArray Entities = ObjArray(m_Program->CurrentScene->Terrain->GetChildren(&numberofchildren));

						bool condition = false;
						if (WhichTilePlayerSteppedOn != nullptr)
						{
							condition =
								m_Program->Player->Position.X >= (-WhichTilePlayerSteppedOn->Size.X / 2) + WhichTilePlayerSteppedOn->Position.X and
								m_Program->Player->Position.X <= (WhichTilePlayerSteppedOn->Size.X / 2) + WhichTilePlayerSteppedOn->Position.X and
								m_Program->Player->Position.Y >= (-WhichTilePlayerSteppedOn->Size.Y / 2) + WhichTilePlayerSteppedOn->Position.Y and
								m_Program->Player->Position.Y <= (WhichTilePlayerSteppedOn->Size.Y / 2) + WhichTilePlayerSteppedOn->Position.Y;
							if (condition == true)
							{
								goto SKIP_FINDTILE;
							}
						}

						for (size_t i = 0; i < numberofchildren; i++)
						{
							if (Sprite* tilesprite = dynamic_cast<Sprite*>(Entities[i]))
							{
								condition =
									m_Program->Player->Position.X >= (-tilesprite->Size.X / 2) + tilesprite->Position.X and
									m_Program->Player->Position.X <= (tilesprite->Size.X / 2) + tilesprite->Position.X and
									m_Program->Player->Position.Y >= (-tilesprite->Size.Y / 2) + tilesprite->Position.Y and
									m_Program->Player->Position.Y <= (tilesprite->Size.Y / 2) + tilesprite->Position.Y;
								if (condition == true)
								{
									WhichTilePlayerSteppedOn = tilesprite;
									IsNewTileFound = true;
									break;
								}
							}
						}
						if (IsNewTileFound == false)
						{
							WhichTilePlayerSteppedOn = nullptr;
						}
					SKIP_FINDTILE:
						if (WhichTilePlayerSteppedOn != nullptr)
						{
							for (const SoundWhenSteppingOnTile& ssot : m_Program->TileWalkingSounds)
							{
								if (ssot.TileTexture != nullptr)
								{
									if (ssot.TileTexture == WhichTilePlayerSteppedOn->Texture)
									{
										if (ssot.Sound != nullptr)
										{
											if (ssot.Sound->IsCurrentlyPlaying() == false)
											{
												ssot.Sound->Play();
											}
										}
										break;
									}
								}
							}
						}
					}
#pragma endregion
#pragma region Entity System
					{
						size_t numberofentities = 0;
						ObjArray entities = m_Program->CurrentScene->SceneWorkspace->GetChildren(&numberofentities);
						for (size_t i = 0; i < numberofentities; i++)
						{
							if (m_Program->Mouse->RightMouseHolded == true)
							{
								if (Entity* entity = dynamic_cast<Entity*>(entities[i]))
								{
									tn::Rect rt = entity->GetRect();
									tn::Vector2 Camera2DPos = { m_Program->CurrentCamera->ViewportPosition.X, m_Program->CurrentCamera->ViewportPosition.Y };
									tn::TransformRect(&rt, Camera2DPos - m_Program->CurrentCamera->ViewportOffset);
									if (tn::IsPointWithinRect(m_Program->Mouse->Position, rt) == true)
									{
										if (EntityPrototype* proto = entity->ReferencePrototype)
										{
											tn::NormalArray<std::wstring>& results = proto->Minable.Result;
											for (size_t i = 0; i < results.GetMaximumSize(); i++)
											{
												m_Program->GiveItemToPlayer(results[i].c_str());
											}
										}
										entity->Destroy();
									}
								}
							}
						}
					}
#pragma endregion
#pragma region Inventory System
					if (m_Program->Toolbox.Loaded == true)
					{
						for (size_t i = 0; i < m_Program->NumberOfActiveItemSlots; i++)
						{
							UI_Frame* Slot = m_Program->Toolbox.ActiveSlots[i];
							if (Slot != nullptr)
							{
								if (m_Program->RechargeTime <= 0)
								{
									if (i == m_Program->Toolbox.ActiveItemIndex)
									{
										Slot->BackgroundColor = { 1.0f, 0.5f, 0.0f };
									}
									else
									{
										Slot->BackgroundColor = { 0.1f, 0.1f, 0.1f };
									}
								}
								else
								{
									if (i == m_Program->Toolbox.ActiveItemIndex)
									{
										Slot->BackgroundColor = { 1.0f, 1.0f, 1.0f };
									}
									else
									{
										Slot->BackgroundColor = { 1.0f, 0.1f, 0.1f };
									}
								}

								if (ImageLabel* aiip = dynamic_cast<ImageLabel*>(Slot))
								{
									InventoryItem* item = m_Program->Inventory.ActiveItemSlots[i];
									if (item != nullptr)
									{
										if (item->BaseItem != nullptr)
										{
											aiip->TargetTexture = item->BaseItem->Icon;
										}
										else
										{
											aiip->TargetTexture = nullptr;
										}
									}
									else
									{
										aiip->TargetTexture = nullptr;
									}
								}
							}
						}

						for (size_t i = 0; i < m_Program->InventorySize; i++)
						{
							InventoryItem* Item = m_Program->Inventory.CharInventoryItemSlots[i];

							if (Item != nullptr)
							{
								if (Item->Size <= 0)
								{
									m_Program->Inventory.CharInventoryItemSlots[i] = nullptr;
									Item->BaseItem = nullptr;
									delete Item;
									Item = nullptr;
								}
							}

							Object* RawFrame = m_Program->Toolbox.CharacterInventory.Body->GetChild(std::to_wstring(i));
							if (UI_Frame* f = dynamic_cast<UI_Frame*>(RawFrame))
							{
								bool OK = true;
								if (GameplayRelatedObjects::InventorySlot* prof = dynamic_cast<GameplayRelatedObjects::InventorySlot*>(f->GetChild(L"Profile")))
								{
									if (Item != nullptr)
									{
										prof->ReferenceInventoryItem = Item;
										ItemDescription* Desc = Item->BaseItem;
										if (Desc != nullptr)
										{
											tn::Texture* icon = Desc->Icon;
											if (icon != nullptr)
											{
												if (icon->Filename.length() > 0)
												{
													prof->TargetTexture = icon;
												}
											}
										}
										else
										{
											OK = false;
										}
									}
									else
									{
										OK = false;
									}

									if (OK == false)
									{
										prof->ReferenceInventoryItem = nullptr;
										prof->TargetTexture = nullptr;
									}
								}
							}
						}
#pragma endregion
#pragma region Build Mode
						static bool BM_messagesent = false;
						if (m_Program->Toolbox.BuildMode == true)
						{
							if (BM_messagesent == false)
							{
								std::wcout << L"You are in build mode. Press Q to cancel it.\n";
								BM_messagesent = true;
							}

							if (m_Program->Toolbox.SelectedInventoryBoxWithEntityData)
							{
								m_Program->Toolbox.SelectedInventoryBoxWithEntityData->BackgroundColor = { 0.0,1.0f,0.0 };
							}

							if (m_Program->Toolbox.HighlightedPlaceableEntity != nullptr)
							{
								if (IsHighlightedPlaceableEntityCollided == true)
								{
									m_Program->Toolbox.HighlightedPlaceableEntity->Color = { 1,0,0 };
								}
								else
								{
									m_Program->Toolbox.HighlightedPlaceableEntity->Color = { 0,1,0 };
								}
							}
							else
							{
								m_Program->Toolbox.HighlightedPlaceableEntity = new Sprite(m_Program);
								m_Program->Toolbox.HighlightedPlaceableEntity->SetName(L"HighlightedEntity");
								m_Program->Toolbox.HighlightedPlaceableEntity->Size = { 5,5 };
								m_Program->Toolbox.HighlightedPlaceableEntity->Color = { 0,1,0 };
								m_Program->Toolbox.HighlightedPlaceableEntity->Alpha = 1.0f;
								m_Program->Toolbox.HighlightedPlaceableEntity->SetParent(m_Program->CurrentScene);
							}

							if (m_Program->WhichUI_MouseIsTouching == nullptr)
							{
								if (Scene* cs = m_Program->CurrentScene)
								{
									if (TerrainFolder* Tiles = cs->Terrain)
									{
										size_t numberofchildren = 0;
										ObjArray array = ObjArray(Tiles->GetChildren(&numberofchildren));

										bool IsMouseOnTile = false;
										for (size_t i = 0; i < numberofchildren; i++)
										{
											if (Sprite* tile = dynamic_cast<Sprite*>(array[i]))
											{
												tn::Rect rectoftile = {
													-tile->Size.X / 2,
													tile->Size.X / 2,
													-tile->Size.Y / 2,
													tile->Size.Y / 2,
												};
												tn::TransformRect(&rectoftile, tile->Position);
												tn::Vector2 Camera2DPos = { m_Program->CurrentCamera->ViewportPosition.X, m_Program->CurrentCamera->ViewportPosition.Y };
												tn::TransformRect(&rectoftile, Camera2DPos - m_Program->CurrentCamera->ViewportOffset);
												if (tn::IsPointWithinRect(m_Program->Mouse->Position, rectoftile) == true)
												{
													if (m_Program->Toolbox.HighlightedPlaceableEntity)
													{
														m_Program->Toolbox.HighlightedPlaceableEntity->Position = tile->Position;
														m_Program->Toolbox.HighlightedPlaceableEntity->Size = {
															PixelsOfTile, PixelsOfTile
														};
														IsMouseOnTile = true;
														MouseOnTile = true;
														m_Program->Toolbox.HighlightedPlaceableEntity->SetParent(m_Program->CurrentScene);
													}
													m_Program->Toolbox.HighlightedTileInBuildMode = tile;
													break;
												}
											}
										}
										if (IsMouseOnTile == false)
										{
											MouseOnTile = false;
											m_Program->Toolbox.HighlightedTileInBuildMode = nullptr;
											if (m_Program->Toolbox.HighlightedPlaceableEntity) m_Program->Toolbox.HighlightedPlaceableEntity->SetParent(nullptr);
										}
									}

									tn::Rect RectOfHPE = m_Program->Toolbox.HighlightedPlaceableEntity->GetRect();
									tn::Rect PlayerRectWithPos = m_Program->Player->GetRect();
									if (tn::AABB_Collision(RectOfHPE, PlayerRectWithPos))
									{
										IsHighlightedPlaceableEntityCollided = true;
									}
									else
									{
										IsHighlightedPlaceableEntityCollided = false;
									}

									if (Workspace* workspace = cs->SceneWorkspace)
									{
										size_t numberofentities = 0;
										ObjArray array = ObjArray(workspace->GetChildren(&numberofentities));

										bool IsMouseOnTile = false;
										for (size_t i = 0; i < numberofentities; i++)
										{
											if (Entity* ent = dynamic_cast<Entity*>(array[i]))
											{
												tn::Rect rectofentity = {
													-ent->Size.X / 2,
													ent->Size.X / 2,
													-ent->Size.Y / 2,
													ent->Size.Y / 2,
												};
												tn::TransformRect(&rectofentity, ent->Position);
												if (m_Program->Toolbox.HighlightedPlaceableEntity != nullptr)
												{
													tn::Rect RectOfHPE = m_Program->Toolbox.HighlightedPlaceableEntity->GetRect();
													if (tn::AABB_Collision(RectOfHPE, rectofentity) == true)
													{
														IsHighlightedPlaceableEntityCollided = true;
														break;
													}
												}
											}
										}
									}
								}
							}
							else
							{
								MouseOnTile = false;
								m_Program->Toolbox.HighlightedTileInBuildMode = nullptr;
								if (m_Program->Toolbox.HighlightedPlaceableEntity) m_Program->Toolbox.HighlightedPlaceableEntity->SetParent(nullptr);
							}
						}
						else
						{
							BM_messagesent = false;
							m_Program->Toolbox.HighlightedTileInBuildMode = nullptr;
							if (m_Program->Toolbox.HighlightedPlaceableEntity) m_Program->Toolbox.HighlightedPlaceableEntity->SetParent(nullptr);
						}
					}
				}
#pragma endregion
				if (m_Program->Toolbox.ObjectiveFrameText != nullptr)
				{
					m_Program->Toolbox.ObjectiveFrameText->Text = m_Program->Toolbox.ObjectiveMsg;
				}

				if (m_Program->CallToEndGameplay == true)
				{
					m_Program->EndGameplay();
					m_Program->CallToEndGameplay = false;
				}
			}
			m_Program->CurrentCamera->ViewportPosition = { -m_Program->Player->Position.X, -m_Program->Player->Position.Y, 0 };
		}
		else
		{
			m_Program->CurrentCamera->ViewportPosition = { 0, 0, 0 };
		}

		//////////////////////////////////////////////////////////////////////////////////

		m_Program->CurrentCamera->ViewportSize = {
			(rt.right > 600) ? static_cast<float>(rt.right) : 600,
			(rt.bottom > 480) ? static_cast<float>(rt.bottom) : 480
		};
		m_Program->CurrentCamera->ViewportOffset = {
			-m_Program->CurrentCamera->ViewportSize.X / 2,
			-m_Program->CurrentCamera->ViewportSize.Y / 2
		};
		m_Program->CurrentCamera->NearView = 0.0f;
		m_Program->CurrentCamera->FarView = 100.0f;

		//////////////////////////////////////////////////////////////////////////////////
#pragma region UI Logistic
		////////////////UI////////////////////
		if (m_Program->UI_UnderContinuousPress == nullptr)
		{
			m_Program->CheckUIForMouse(m_Program->GameHierachy.GUI);
		}
		else if (m_Program->Mouse->LeftMouseHolded == false)
		{
			m_Program->UI_UnderContinuousPress = nullptr;
		}

		if (m_Program->WhichUI_MouseIsTouching != nullptr)
		{
			if (m_Program->WhichUI_MouseIsTouching->Visible == true)
			{
				if (m_Program->WhichUI_MouseIsTouching->MouseTouchedFunction != nullptr)
				{
					m_Program->WhichUI_MouseIsTouching->MouseTouchedFunction(m_Program, m_Program->WhichUI_MouseIsTouching, m_Program->Mouse);
				}

				if (MessageBoxUI* messageBox = dynamic_cast<MessageBoxUI*>(m_Program->WhichUI_MouseIsTouching))
				{
					if (m_Program->Mouse->LeftMouseHolded == true)
					{
						if (tn::IsPointWithinRect(m_Program->Mouse->Position, messageBox->AbsolutePosition, tn::Vector2({ messageBox->AbsoluteSize.X, messageBox->FinalBarSizeY })) == true)
						{
							if (messageBox->IsBeingDragged == false)
							{
								messageBox->IsBeingDragged = true;
								m_Program->UI_UnderContinuousPress = m_Program->WhichUI_MouseIsTouching;
								messageBox->InitialMousePosOnUI = (m_Program->Mouse->Position - messageBox->AbsolutePosition);
							}
						}
					}
					else if (m_Program->Mouse->LeftMouseUp == true)
					{
						m_Program->UI_UnderContinuousPress = nullptr;
						messageBox->IsBeingDragged = false;
					}
				}
				else if (DragInterface* draggableInterface = dynamic_cast<DragInterface*>(m_Program->WhichUI_MouseIsTouching))
				{
					if (UI_Frame* draggableFrame = dynamic_cast<UI_Frame*>(m_Program->WhichUI_MouseIsTouching))
					{
						if (m_Program->Mouse->LeftMouseHolded == true)
						{
							if (tn::IsPointWithinRect(m_Program->Mouse->Position, draggableFrame->TouchableRect) == true)
							{
								draggableInterface->IsBeingDragged = true;
								m_Program->UI_UnderContinuousPress = m_Program->WhichUI_MouseIsTouching;
								draggableInterface->InitialMousePosOnUI = (m_Program->Mouse->Position - draggableFrame->AbsolutePosition);
							}
						}
						else if (m_Program->Mouse->LeftMouseUp == true)
						{
							m_Program->UI_UnderContinuousPress = nullptr;
							if (draggableInterface->IsBeingDragged == true)
							{
								if (draggableInterface->ConnectedStopDragFunc != nullptr)
								{
									draggableInterface->ConnectedStopDragFunc(m_Program, draggableInterface, m_Program->Mouse);
								}
							}
							draggableInterface->IsBeingDragged = false;
						}
					}
				}
				else if (m_Program->WhichUI_MouseIsTouching->GetType() == ObjectClass::ScrollingFrame)
				{
					ScrollingFrame* sf = dynamic_cast<ScrollingFrame*>(m_Program->WhichUI_MouseIsTouching);
					if (m_Program->Mouse->LeftMouseHolded == true)
					{
						if (sf->IsScrolling == false)
						{
							if (tn::IsPointWithinRect(m_Program->Mouse->Position, sf->ThumbPosRelativeToBaseUI, sf->ThumbSize) == true)
							{
								sf->IsScrolling = true;
								m_Program->UI_UnderContinuousPress = m_Program->WhichUI_MouseIsTouching;
								sf->InitialMousePosOnThumb = m_Program->Mouse->Position - sf->ThumbPosRelativeToBaseUI;
							}
						}
					}
					else if (m_Program->Mouse->LeftMouseUp == true)
					{
						m_Program->UI_UnderContinuousPress = nullptr;
						sf->IsScrolling = false;
					}
					else if (m_Program->Mouse->MouseWheelDelta != 0)
					{
						tn::Vector2 offset = { 0, -m_Program->Mouse->MouseWheelDelta };
						sf->MoveThumb(sf->ThumbPos + offset);
					}
				}
				else if (TextButton* tb = dynamic_cast<TextButton*>(m_Program->WhichUI_MouseIsTouching))
				{
					if (m_Program->Mouse->LeftMouseDown == true)
					{
						if (tb->ButtonPressed == false)
						{
							tb->ButtonPressed = true;
							if (tb->OnMouseButton1ClickFunction != nullptr)
							{
								tb->OnMouseButton1ClickFunction(m_Program, tb, m_Program->Mouse);
							}
						}

						if (tb->OnMouseButton1DownFunction != nullptr)
						{
							tb->OnMouseButton1DownFunction(m_Program, tb, m_Program->Mouse);
						}
					}
					else if (m_Program->Mouse->LeftMouseUp == true)
					{
						if (tb->ButtonPressed == true)
						{
							tb->ButtonPressed = false;
							if (tb->OnMouseButton1UpFunction != nullptr)
							{
								tb->OnMouseButton1UpFunction(m_Program, tb, m_Program->Mouse);
							}
						}
					}
					else
					{
						tb->ButtonPressed = false;
					}
				}
			}
		}

		if (m_Program->UI_UnderContinuousPress != nullptr)
		{
			if (m_Program->UI_UnderContinuousPress->GetType() == ObjectClass::MessageBoxUI)
			{
				MessageBoxUI* draggableFrame = dynamic_cast<MessageBoxUI*>(m_Program->UI_UnderContinuousPress);
				{
					std::vector<Object*> guichildren = m_Program->GameHierachy.GUI->GetChildrenVector();
					auto it = std::find(guichildren.begin(), guichildren.end(), m_Program->WhichUI_MouseIsTouching);
					if (it != guichildren.end())
					{
						m_Program->GameHierachy.GUI->Children->Swap(m_Program->WhichUI_MouseIsTouching, guichildren.back());
					}

					tn::Vector2 Offset = (m_Program->Mouse->Position - draggableFrame->InitialMousePosOnUI);
					draggableFrame->Position.Offset = { 0, 0 };
					draggableFrame->Position.Scale = tn::Vector2({
						Offset.X / m_Program->CurrentCamera->ViewportSize.X,
						Offset.Y / m_Program->CurrentCamera->ViewportSize.Y
						});
				}
			}
			else if (DragInterface* dragInterface = dynamic_cast<DragInterface*>(m_Program->UI_UnderContinuousPress))
			{
				if (UI_Frame* draggableFrame = dynamic_cast<UI_Frame*>(m_Program->UI_UnderContinuousPress))
				{
					std::vector<Object*> guichildren = m_Program->GameHierachy.GUI->GetChildrenVector();
					auto it = std::find(guichildren.begin(), guichildren.end(), m_Program->WhichUI_MouseIsTouching);
					if (it != guichildren.end())
					{
						m_Program->GameHierachy.GUI->Children->Swap(m_Program->WhichUI_MouseIsTouching, guichildren.back());
					}

					tn::Vector2 Offset = (m_Program->Mouse->Position - dragInterface->InitialMousePosOnUI);
					draggableFrame->Position.Offset = { 0, 0 };
					draggableFrame->Position.Scale = tn::Vector2({
						Offset.X / m_Program->CurrentCamera->ViewportSize.X,
						Offset.Y / m_Program->CurrentCamera->ViewportSize.Y
						});

					if (dragInterface->ConnectedDragFunc != nullptr)
					{
						dragInterface->ConnectedDragFunc(m_Program, dragInterface, m_Program->Mouse);
					}
				}
			}
			else if (m_Program->UI_UnderContinuousPress->GetType() == ObjectClass::ScrollingFrame)
			{
				ScrollingFrame* sf = dynamic_cast<ScrollingFrame*>(m_Program->UI_UnderContinuousPress);
				{
					tn::Vector2 Offset = ((m_Program->Mouse->Position - sf->InitialMousePosOnThumb) - sf->AbsolutePosition);
					sf->MoveThumb(Offset);

					sf->ScrollBar.ScrollRatio = tn::math::clamp(sf->ScrollBar.ScrollRatio, 0.0f, 1.0f);
				}
			}
		}
#pragma endregion
		//////////////////////////////////////////////////
		//ANIMATIONS
		if (m_Program->RefAnimations.size() > 0)
		{
			for (int i = static_cast<int>(m_Program->RefAnimations.size()) - 1; i >= 0; --i) {
				Animation* Anim = m_Program->RefAnimations[i];
				if (Anim) {
					Anim->AddStep(interval);
					if (Anim->IsEnded()) {
						delete Anim;
						m_Program->RefAnimations.erase(m_Program->RefAnimations.begin() + i);
					}
				}
			}
		}

		//TASK

		//GARBAGE COLLECTION
		size_t sizeofgarbagecollection_run = m_Program->GarbageCollection.OnCleanup.GetSize();
		if (sizeofgarbagecollection_run > 0)
		{
			tn::ReferenceArray<ProgramObject> ListOfObjToRelease(m_Program->GarbageCollection.OnCleanup);
			for (size_t i = 0; i < sizeofgarbagecollection_run; i++) {
				if (ProgramObject* P_Obj = ListOfObjToRelease[i])
				{
					P_Obj->Release();
				}
			}
			m_Program->GarbageCollection.OnCleanup.Clear();
		}

		size_t sizeofgarbagecollection_queued = m_Program->GarbageCollection.Queued.GetSize();
		if (sizeofgarbagecollection_queued > 0)
		{
			for (size_t i = 0; i < sizeofgarbagecollection_run; i++) {
				if (ProgramObject* P_Obj = m_Program->GarbageCollection.Queued[i])
				{
					if (P_Obj->Active == false)
					{
						m_Program->GarbageCollection.OnCleanup.Add(P_Obj);
					}
				}
			}
			m_Program->GarbageCollection.Queued.Clear();
		}

		QueryPerformanceCounter(&end);
		interval = static_cast<float>(end.QuadPart - start.QuadPart) / frequency.QuadPart;
	}

	CoUninitialize();
	return 0;
}

void Program::AddAnimation(Animation* Anim)
{
	if (Anim != nullptr)
	{
		if (Anim->ProgramDataLinkedAnimation == nullptr)
		{
			Animation* newnaim = new Animation(Anim);
			Anim->ProgramDataLinkedAnimation = newnaim;
			//Anim->ProgramDataLinkedAnimation->ProgramDataLinkedAnimation = nullptr;
			this->RefAnimations.push_back(newnaim);
		}
	}
}

void Program::AddDebris(ProgramObject* Obj, float timetodestroy)
{
	Debris* d = new Debris();
	d->obj = Obj;
	d->timetodestroy = timetodestroy;
	for (size_t i = 0; i < DebrisList.size(); i++)
	{
		if (DebrisList.at(i) == nullptr)
		{
			DebrisList.at(i) = d;
			return;
		}
	}
	DebrisList.push_back(d);
}

Scene* Program::GetSceneByName(std::wstring name)
{
	Scene* result = nullptr;

	std::vector<Object*> ListOfScenes = this->GameHierachy.Scenes->GetChildrenWithClassVector(ObjectClass::Scene);
	for (size_t i = 0; i < ListOfScenes.size(); i++)
	{
		if (ListOfScenes[i]->GetName() == name)
		{
			if (result = dynamic_cast<Scene*>(ListOfScenes.at(i)))
			{
				break;
			};
		}
	}
	return result;
}

void Program::SetCurrentScene(Scene* targetscene)
{
	this->CurrentScene = targetscene;
}

void Program::PauseWhileProcessWindow(float Seconds)
{
	LARGE_INTEGER start, end, frequency;

	float t = 0.0f;
	while (t < Seconds)
	{
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);
		Sleep(1);
		if (!this->MainWnd->ProcessMessage()) break;
		QueryPerformanceCounter(&end);
		this->interval = static_cast<float>(end.QuadPart - start.QuadPart) / frequency.QuadPart;
		t += this->interval;
	}
}

tn::Texture* Program::GetFirstLoadedTexture(std::wstring Name)
{
	for (tn::Texture* texture : this->GameHierachy.Textures) {
		if (texture->GetName() == Name)
		{
			return texture;
		}
	}

	return nullptr;
}

bool Program::CheckGDIPlusRun()
{
	return this->IsGDIPlusRun;
}

////////////////////////////////////////////////////////////////////////////////////////////

void Program::InventoryFrameProfileEnter(Program* p, UI_Frame* Body, MouseObject* Mouse)
{
	Body->BackgroundColor = { 0.5f, 0.25f, 0.0f };
}

void Program::InventoryFrameProfileLeave(Program* p, UI_Frame* Body, MouseObject* Mouse)
{
	Body->BackgroundColor = { 0.0f, 0.0f, 0.0f };
}

void Program::SelectedInventoryFrameUndragged(Program* p, DragInterface* Body, MouseObject* Mouse)
{
	if (GameplayRelatedObjects::SelectedInventoryItemBox* SIIB = dynamic_cast<GameplayRelatedObjects::SelectedInventoryItemBox*>(Body))
	{
		SIIB->Visible = false;
	}
}

void Program::InventoryFrameClicked(Program* p, TextButton* Body, MouseObject* Mouse)
{
	if (ImageButton* ImageLabelBody = dynamic_cast<ImageButton*>(Body))
	{
		if (GameplayRelatedObjects::InventorySlot* InvSlot = dynamic_cast<GameplayRelatedObjects::InventorySlot*>(ImageLabelBody))
		{
			if (InventoryItem* item = InvSlot->ReferenceInventoryItem)
			{
				if (ItemDescription* desc = item->BaseItem)
				{
					if (desc->Type == ItemType::ItemWithEntityData)
					{
						if (p->Toolbox.SelectedInventoryBoxWithEntityData)
						{
							p->Toolbox.SelectedInventoryBoxWithEntityData->BackgroundColor = { 0,0,0 };
						}
						p->Toolbox.SelectedInventoryBoxWithEntityData = InvSlot;
						p->Toolbox.BuildMode = true;
					}
				}

				p->Toolbox.DraggedTool_InventoryBox->BackgroundColor = { 1.0f, 0.5f, 0.25f };
				p->Toolbox.DraggedTool_InventoryBox->BackgroundTransparencyAlpha = 1.0f;
				p->Toolbox.DraggedTool_InventoryBox->TargetTexture = InvSlot->TargetTexture;
				p->Toolbox.DraggedTool_InventoryBox->Position.Scale = { 0,0 };
				p->Toolbox.DraggedTool_InventoryBox->Position.Offset = Body->AbsolutePosition;
				p->Toolbox.DraggedTool_InventoryBox->ReferenceInventoryItem = item;
				p->Toolbox.DraggedTool_InventoryBox->Visible = true;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void Program::LoadInventoryUI()
{
	auto& ToolboxFrame = this->Toolbox.ToolboxFrame;
	auto& ToolboxFrameSlots = this->Toolbox.ToolboxFrameSlots;
	auto& ActiveItemInventory = this->Toolbox.ActiveItemInventory;
	auto& ActiveItemInventoryProfile = this->Toolbox.ActiveItemInventoryProfile;
	auto& ActiveSlots = this->Toolbox.ActiveSlots;
	auto& CharacterInventory = this->Toolbox.CharacterInventory;

	ToolboxFrame = new UI_Frame(this);
	ToolboxFrame->SetName(L"ToolboxFrame");
	ToolboxFrame->BackgroundColor = { 0.2f, 0.2f, 0.2f };
	ToolboxFrame->Size.Offset.X = 400;
	ToolboxFrame->Size.Offset.Y = 50;
	ToolboxFrame->Position.Scale.X = 0.5f;
	ToolboxFrame->Position.Offset.X = -ToolboxFrame->Size.Offset.X / 2;
	ToolboxFrame->Position.Scale.Y = 1.0f;
	ToolboxFrame->Position.Offset.Y = -ToolboxFrame->Size.Offset.Y;
	ToolboxFrame->GraphicDevice = this->MainGraphicDevice;
	ToolboxFrame->Load();

	{
		for (int i = 1; i <= this->ToolboxItemSlots; i++)
		{
			std::wstring t = L"";
			t += i;

			UI_Frame* Slot = new UI_Frame(this);
			Slot->SetName(t);
			Slot->BackgroundColor = { 0.1f, 0.1f, 0.1f };
			Slot->Size.Offset = { 36, 36 };
			Slot->Position.Offset = { 5.0f + (5 * (i - 1)) + (Slot->Size.Offset.X * (i - 1)), 5 };
			Slot->GraphicDevice = this->MainGraphicDevice;
			Slot->Load();
			ToolboxFrame->AddChild(Slot);

			ToolboxFrameSlots.Add(Slot);
		}
	}

	this->GameHierachy.GUI->AddChild(ToolboxFrame);

	ActiveItemInventory = new UI_Frame(this);
	ActiveItemInventory->SetName(L"ActiveItemInventory");
	ActiveItemInventory->BackgroundColor = { 0.5f, 0.5f, 0.5f };
	ActiveItemInventory->Size.Offset.X = 175;
	ActiveItemInventory->Size.Offset.Y = 50;
	ActiveItemInventory->Position.Scale.X = 0;
	ActiveItemInventory->Position.Offset.X = 0;
	ActiveItemInventory->Position.Scale.Y = 1.0f;
	ActiveItemInventory->Position.Offset.Y = -ActiveItemInventory->Size.Offset.Y;
	ActiveItemInventory->GraphicDevice = this->MainGraphicDevice;
	ActiveItemInventory->Load();

	{
		ActiveItemInventoryProfile = new ImageLabel(this);
		ActiveItemInventoryProfile->SetName(L"ActiveItemInventoryProfile");
		ActiveItemInventoryProfile->TargetTexture = this->GetFirstLoadedTexture(L"Player");
		ActiveItemInventoryProfile->Size.Offset.X = 36;
		ActiveItemInventoryProfile->Size.Offset.Y = 36;
		ActiveItemInventoryProfile->Position.Offset.X = 5;
		ActiveItemInventoryProfile->Position.Offset.Y = 5;
		ActiveItemInventoryProfile->GraphicDevice = this->MainGraphicDevice;
		ActiveItemInventoryProfile->Load();
		ActiveItemInventory->AddChild(ActiveItemInventoryProfile);

		for (int i = 1; i <= this->NumberOfActiveItemSlots; i++)
		{
			std::wstring t = std::to_wstring(i);

			ImageLabel* Slot = new ImageLabel(this);
			Slot->SetName(L"Slot" + t);
			Slot->BackgroundColor = { 0.1f, 0.1f, 0.1f };
			Slot->Size.Offset = { 36, 36 };
			Slot->Position.Offset = { 36 + 5.0f + (5 * i) + (Slot->Size.Offset.X * (i - 1)), 5 };
			Slot->GraphicDevice = this->MainGraphicDevice;
			Slot->Load();
			ActiveItemInventory->AddChild(Slot);

			ActiveSlots.Add(Slot);
		}
	}

	this->GameHierachy.GUI->AddChild(ActiveItemInventory);

	CharacterInventory.Body = new MessageBoxUI(this);
	CharacterInventory.Body->SetName(L"CharacterInvetory");
	CharacterInventory.Body->BarText = this->GetLocaleText(this->CurrentLang.data(), L"inventory");
	CharacterInventory.Body->GraphicDevice = this->MainGraphicDevice;
	CharacterInventory.Body->BarSizeY = { 0, 30 };
	CharacterInventory.Body->BarColor = { 0.1f,0.1f,0.1f };
	CharacterInventory.Body->BackgroundColor = { 0.45f, 0.45f, 0.45f };
	CharacterInventory.Body->Size = { 0, 0, this->InventoryFrameSize.X, this->InventoryFrameSize.Y };
	CharacterInventory.Body->Position = { 0.5f, 0.5f, -CharacterInventory.Body->Size.Offset.X / 2, -CharacterInventory.Body->Size.Offset.Y / 2 };
	CharacterInventory.Body->ZDepth = 1.0f;
	CharacterInventory.Body->Visible = false;
	CharacterInventory.Body->Load();

	size_t maxX = this->InventorySlotPosLimitX / InventorySlotFrameOffsetSize.X;

	size_t x = 0;
	size_t y = 0;

	for (int i = 0; i < this->InventorySize; i++)
	{
		UI_Frame* InventoryFrame = new UI_Frame(this);
		InventoryFrame->SetName(std::to_wstring(i));
		InventoryFrame->Size.Offset = InventorySlotFrameOffsetSize;

		if (x >= maxX)
		{
			x = 0;
			y++;
		}

		tn::Vector2 Position = {
			1 + (InventoryFrame->Size.Offset.X * x) + ((x > 0) ? x * 2 : 0),
			(CharacterInventory.Body->BarSizeY.Y + 1) + (InventoryFrame->Size.Offset.X * y) + ((y > 0) ? y * 2 : 0)
		};

		InventoryFrame->Position.Offset = Position;
		InventoryFrame->ZDepth = 1.0f;
		InventoryFrame->BackgroundColor = { 0.2f, 0.2f, 0.2f };
		InventoryFrame->GraphicDevice = this->MainGraphicDevice;
		InventoryFrame->Visible = true;
		InventoryFrame->Load();

		GameplayRelatedObjects::InventorySlot* InventoryFrameProfile = new GameplayRelatedObjects::InventorySlot(this);
		InventoryFrameProfile->SetName(L"Profile");
		InventoryFrameProfile->Size.Offset = InventorySlotFrameOffsetSize;
		InventoryFrameProfile->Position.Offset = { 0,0 };
		InventoryFrameProfile->GraphicDevice = this->MainGraphicDevice;
		InventoryFrameProfile->ZDepth = 1.0f;
		InventoryFrameProfile->Visible = true;
		InventoryFrameProfile->Load();
		InventoryFrameProfile->OnMouseEnter(this->InventoryFrameProfileEnter);
		InventoryFrameProfile->OnMouseLeave(this->InventoryFrameProfileLeave);
		InventoryFrameProfile->OnMouseButton1Down(this->InventoryFrameClicked);

		InventoryFrame->AddChild(InventoryFrameProfile);

		CharacterInventory.FrameSlots.Add(InventoryFrame);
		CharacterInventory.Body->AddChild(InventoryFrame);
		x++;
	}

	this->GameHierachy.GUI->AddChild(CharacterInventory.Body);

	this->Inventory.ActiveItemSlots.Resize(this->NumberOfActiveItemSlots);
	for (size_t i = 0; i < this->Inventory.ActiveItemSlots.GetMaximumSize(); i++)
	{
		this->Inventory.ActiveItemSlots[i] = nullptr;
	}
	this->Inventory.CharInventoryItemSlots.Resize(this->InventorySize);
	for (size_t i = 0; i < this->Inventory.CharInventoryItemSlots.GetMaximumSize(); i++)
	{
		this->Inventory.CharInventoryItemSlots[i] = nullptr;
	}

	this->Toolbox.DraggedTool_InventoryBox = new GameplayRelatedObjects::SelectedInventoryItemBox(this);
	this->Toolbox.DraggedTool_InventoryBox->Size.Offset = InventorySlotFrameOffsetSize;
	this->Toolbox.DraggedTool_InventoryBox->ZDepth = 1.0f;
	this->Toolbox.DraggedTool_InventoryBox->Visible = false;
	this->Toolbox.DraggedTool_InventoryBox->GraphicDevice = this->MainGraphicDevice;
	this->Toolbox.DraggedTool_InventoryBox->Load();
	this->Toolbox.DraggedTool_InventoryBox->ConnectStopDraggingFunctiom(this->SelectedInventoryFrameUndragged);
	this->GameHierachy.GUI->AddChild(this->Toolbox.DraggedTool_InventoryBox);

	Sleep(1);
	this->Toolbox.Loaded = true;
}

void Program::LoadObjectiveUI()
{
	this->Toolbox.ObjectiveFrame = new UI_Frame(this);
	this->Toolbox.ObjectiveFrame->SetName(L"ObjectiveFrame");
	this->Toolbox.ObjectiveFrame->Size.Offset = { 325, 200 };
	this->Toolbox.ObjectiveFrame->Position = { 0,0,0,0 };
	this->Toolbox.ObjectiveFrame->BackgroundColor = { 0.2f, 0.2f, 0.2f };
	this->Toolbox.ObjectiveFrame->GraphicDevice = this->MainGraphicDevice;
	this->Toolbox.ObjectiveFrame->ZDepth = 2.0f;
	this->Toolbox.ObjectiveFrame->Load();

	this->Toolbox.ObjectiveFrameTitle = new TextLabel(this);
	this->Toolbox.ObjectiveFrameTitle->SetName(L"ObjectiveFrameTitle");
	this->Toolbox.ObjectiveFrameTitle->Text = this->GetLocaleText(this->CurrentLang.data(), L"nhiemvu");
	this->Toolbox.ObjectiveFrameTitle->TextSize = 14;
	this->Toolbox.ObjectiveFrameTitle->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentLeft;
	this->Toolbox.ObjectiveFrameTitle->Font = L"Verdana";
	this->Toolbox.ObjectiveFrameTitle->Size.Offset = { 325, 25 };
	this->Toolbox.ObjectiveFrameTitle->BackgroundColor = { 0.1f, 0.1f, 0.1f };
	this->Toolbox.ObjectiveFrameTitle->GraphicDevice = this->MainGraphicDevice;
	this->Toolbox.ObjectiveFrameTitle->ZDepth = 2.0f;
	this->Toolbox.ObjectiveFrameTitle->Load();
	this->Toolbox.ObjectiveFrame->AddChild(this->Toolbox.ObjectiveFrameTitle);

	this->Toolbox.ObjectiveFrameText = new TextLabel(this);
	this->Toolbox.ObjectiveFrameText->SetName(L"ObjectiveFrameText");
	this->Toolbox.ObjectiveFrameText->Size.Offset = { 325, 175 };
	this->Toolbox.ObjectiveFrameText->Position.Offset = { 0, 25 };
	this->Toolbox.ObjectiveFrameText->BackgroundTransparencyAlpha = 0.0f;
	this->Toolbox.ObjectiveFrameText->GraphicDevice = this->MainGraphicDevice;
	this->Toolbox.ObjectiveFrameText->ZDepth = 2.0f;
	this->Toolbox.ObjectiveFrameText->TextSize = 14;
	this->Toolbox.ObjectiveFrameText->Font = L"Verdana";
	this->Toolbox.ObjectiveFrameText->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentLeft;
	this->Toolbox.ObjectiveFrameText->TextAlignment = TextAlignmentEnum::TextAlignmentTop;
	this->Toolbox.ObjectiveFrameText->WordBreak = true;
	this->Toolbox.ObjectiveFrameText->Load();
	this->Toolbox.ObjectiveFrame->AddChild(this->Toolbox.ObjectiveFrameText);

	this->GameHierachy.GUI->AddChild(this->Toolbox.ObjectiveFrame);
}

void Program::AddObjectToGarbageCollection(ProgramObject* Obj)
{
	this->GarbageCollection.Queued.Add(Obj);
}

void Program::GiveItemToPlayer(const wchar_t* PrototypeItemName, const wchar_t* AltText)
{
	ItemDescription* Item = nullptr;

	bool Stage1Success = false;
	for (ItemDescription& i : this->ListOfItems)
	{
		if (i.PrototypeName == PrototypeItemName)
		{
			Stage1Success = true;
			Item = &i;
			break;
		}
	}

	if (Stage1Success == true)
	{
		size_t SizeOfItemSlots = this->Inventory.CharInventoryItemSlots.GetMaximumSize();
		if (SizeOfItemSlots > 0)
		{
			for (size_t i = 0; i < SizeOfItemSlots; i++)
			{
				InventoryItem* inv = this->Inventory.CharInventoryItemSlots[i];
				if (inv == nullptr)
				{
					inv = new InventoryItem();
					inv->BaseItem = Item;
					wchar_t* new_str = nullptr;
					if (wcslen(AltText) > 0)
					{
						(void)wcscpy_s(new_str, wcslen(AltText), AltText);
					}
					inv->AltText = new_str;
					inv->Size = 1;
					this->Inventory.CharInventoryItemSlots[i] = inv;
					break;
				}
				else
				{
					ItemDescription* Desc = inv->BaseItem;
					if (Desc != nullptr)
					{
						if (Desc->PrototypeName == Item->PrototypeName)
						{
							if (inv->Size < Item->StackSize)
							{
								inv->Size++;
								break;
							}
						}
					}
				}
			}
		}
	}
}

EntityPrototype* Program::FindEntity(const wchar_t* PrototypeEntityName)
{
	size_t size = this->ListOfEntities.size();
	for (size_t i = 0; i < size; i++)
	{
		EntityPrototype* proto = &this->ListOfEntities.at(i);
		if (proto->PrototypeName == PrototypeEntityName)
		{
			return proto;
		}
	}

	return nullptr;
}

void Program::PlaceEntity(EntityPrototype* prototype, tn::Vector2 Position)
{
	if (prototype != nullptr)
	{
		Entity* newEntity = new Entity(this);
		newEntity->SetName(prototype->PrototypeName);
		newEntity->Size = prototype->SpriteSize;
		newEntity->Position = Position;
		newEntity->CollisionBox = prototype->CollisionBox;
		newEntity->SelectionBox = prototype->SelectionBox;
		newEntity->CurrentHealth = prototype->MaxHealth;
		newEntity->MaxHealth = prototype->MaxHealth;
		newEntity->ReferencePrototype = prototype;
		newEntity->SetTexture(prototype->Sprites[0]);

		this->CurrentScene->AddObjectToScene(newEntity);
	}
}

void Program::SetObjectiveMessage(std::wstring ObjectiveMsg)
{
	this->Toolbox.ObjectiveMsg = ObjectiveMsg;
}

void Program::StartGameplayScript()
{
	GameplayScriptThread = CreateThread(nullptr, 0, GameplayScript, this, 0, nullptr);
}

void Program::SetBuildMode(bool Set)
{
	this->Toolbox.BuildMode = Set;
}

std::wstring Program::GetLocaleText(const wchar_t* lang, const wchar_t* textname)
{
	bool IsLangFound = false;
	std::wstring deftext = L"";
	size_t foundlangindex = 0;
	size_t numberoflang = this->Localization.GetMaximumSize();
	for (size_t i = 0; i < numberoflang; i++)
	{
		if (this->Localization[i].Lang == lang)
		{
			foundlangindex = i;
			IsLangFound = true;
			break;
		}
	}
	if (IsLangFound == true)
	{
		size_t numberoftext = this->Localization[foundlangindex].Texts.GetMaximumSize();
		for (size_t i = 0; i < numberoftext; i++)
		{
			if (this->Localization[foundlangindex].Texts[i].Textname == textname)
			{
				deftext = this->Localization[foundlangindex].Texts[i].Text;
				break;
			}
		}
	}

	return deftext;
}

bool Program::GetLangIndex(size_t* Output, const std::wstring& lang)
{
	size_t numberoflang = this->Localization.GetMaximumSize();
	for (size_t i = 0; i < numberoflang; i++)
	{
		if (this->Localization[i].Lang == lang)
		{
			(*Output) = i;
			return true;
		}
	}

	return false;
}

bool Program::GetLangByIndex(std::wstring* Output, const size_t& index)
{
	size_t numberoflang = this->Localization.GetMaximumSize();
	for (size_t i = 0; i < numberoflang; i++)
	{
		if (i == index)
		{
			(*Output) = this->Localization[i].Lang;
			return true;
		}
	}

	return false;
}

bool Program::CreateMessageBox(const std::wstring& Caption, const std::wstring& Text, MessageBoxUI** OutputBody)
{
	MessageBoxUI* MsgBox = new MessageBoxUI(this);
	MsgBox = new MessageBoxUI(this);
	MsgBox->GraphicDevice = this->MainGraphicDevice;
	MsgBox->SetName(L"MessageBox");
	MsgBox->Visible = true;
	MsgBox->BarSizeY = { 0, 30 };
	MsgBox->BarColor = { 0.25f,0.25f,0.25f };
	MsgBox->BarText = Caption;
	MsgBox->Size = { 0, 0, 550, 150 };
	MsgBox->Position = { 0.5f, 0.5f, -(MsgBox->Size.Offset.X / 2), -(MsgBox->Size.Offset.Y / 2) };
	MsgBox->ZDepth = 1.0f;
	MsgBox->Load();

	TextLabel* textLabel = new TextLabel(this);
	textLabel->GraphicDevice = this->MainGraphicDevice;
	textLabel->SetName(L"Text");
	textLabel->Visible = true;
	textLabel->Size = { 1, 1, 0, -(MsgBox->BarSizeY.Y + 35) };
	textLabel->BackgroundTransparencyAlpha = 0.0f;
	textLabel->TextColor = { 1,1,1 };
	textLabel->Text = Text;
	textLabel->TextSize = 16.0f;
	textLabel->Position = { 0,0,0,MsgBox->BarSizeY.Y + 35 };
	textLabel->ZDepth = 1.0f;
	textLabel->Load();
	MsgBox->AddChild(textLabel);

	TextButton* CloseButton = new TextButton(this);
	CloseButton->SetName(L"CloseButton");
	CloseButton->Size = { 0, 0, 100, 25 };
	CloseButton->BackgroundColor = { 0.8f, 0.04f, 0.05f };
	CloseButton->Position = { 0, 1.0f, 10, -(CloseButton->Size.Offset.Y + 10) };
	CloseButton->ZDepth = 1.0f;
	CloseButton->TextSize = 32.0f;
	CloseButton->Text = this->GetLocaleText(this->CurrentLang.data(), L"close");
	CloseButton->Font = VerdanaFont;
	CloseButton->Italic = false;
	CloseButton->Bold = false;
	CloseButton->TextColor = { 1.0f,1.0f,1.0f };
	CloseButton->GraphicDevice = this->MainGraphicDevice;
	CloseButton->TextAlignment = TextAlignmentEnum::TextAlignmentCenter;
	CloseButton->ParagraphAlignment = ParagraphAlignmentEnum::ParagraphAlignmentMiddle;
	CloseButton->Alpha = 1.0f;
	CloseButton->Load();
	MsgBox->AddChild(CloseButton);
	CloseButton->OnMouseButton1Down([](Program* m_Program, TextButton* Body, MouseObject* Mouse) -> void
		{
			Body->Visible = false;
			Body->GetParent()->Destroy();
		});

	this->GameHierachy.GUI->AddChild(MsgBox);

	if (OutputBody != nullptr)
		(*OutputBody) = MsgBox;

	return true;
}