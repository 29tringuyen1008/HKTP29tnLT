#include "Lua29tnlibs.h"
#include "Lua/lstate.h"

#include <Windows.h>

typedef struct LuaThread {
	HANDLE threadHandle;
	DWORD threadId;
} LuaThread;


DWORD CALLBACK threadFuncWrapper(LPVOID lpParam) {
	lua_State* threadState = (lua_State*)lpParam;

	if (lua_pcall(threadState, 0, LUA_MULTRET, 0) != LUA_OK) {
		fprintf(stderr, "Error in thread function: %s\n", lua_tostring(threadState, -1));
		lua_pop(threadState, 1);
	}

	return 0;
}

int Lua29tnlibs::wait(lua_State* L)
{
	size_t n = luaL_optnumber(L, 1, 1);
	Sleep(n);
	return 0;
}

int Lua29tnlibs::lua_IsProgramRunning(lua_State* L)
{
	if (L->ReferenceProgram->MainWnd->IsPresent() == true)
	{
		lua_pushboolean(L, TRUE);
	}
	else
	{
		lua_pushboolean(L, FALSE);
	}
	return 1;
}

int Lua29tnlibs::lua_CreateThread(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TFUNCTION);

	lua_State* newThreadState = luaL_newstate(L->ReferenceProgram);
	luaL_openlibs(newThreadState);

	for (size_t i = 0; i < L->ReferenceProgram->ProgramEmbeddedFunctions.size(); i++) {
		lua_register(newThreadState, L->ReferenceProgram->ProgramEmbeddedFunctions.at(i).name, L->ReferenceProgram->ProgramEmbeddedFunctions.at(i).func);
	}

	lua_pushvalue(L, 1);
	lua_xmove(L, newThreadState, 1);

	LuaThread* luaThread = new LuaThread;
	luaThread->threadHandle = CreateThread(
		NULL,
		0,
		threadFuncWrapper,
		newThreadState,
		0,
		NULL
	);

	if (luaThread->threadHandle == NULL) {
		lua_close(newThreadState); // Clean up the Lua state
		luaL_error(L, "Failed to create thread.");
	}

	lua_pushlightuserdata(L, luaThread);

	return 1;
}

int Lua29tnlibs::lua_SuspendThread(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
	LuaThread* Thread = (LuaThread*)lua_touserdata(L, 1);
	if (Thread != nullptr)
	{
		SuspendThread(Thread->threadHandle);
	}
	return 0;
}

int Lua29tnlibs::lua_ResumeThread(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
	LuaThread* Thread = (LuaThread*)lua_touserdata(L, 1);
	if (Thread != nullptr)
	{
		ResumeThread(Thread->threadHandle);
	}
	return 0;
}

int Lua29tnlibs::lua_CloseThread(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
	LuaThread* Thread = (LuaThread*)lua_touserdata(L, 1);
	if (Thread != nullptr)
	{
		DWORD result = WaitForSingleObject(Thread->threadHandle, 1);
		if (result == WAIT_TIMEOUT) {
			SuspendThread(Thread->threadHandle);
			Sleep(1);
		}

		CloseHandle(Thread->threadHandle);
		Thread->threadHandle = NULL;

		delete Thread;
	}
	return 0;
}

int Lua29tnlibs::lua_WaitForThread(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
	LuaThread* Thread = (LuaThread*)lua_touserdata(L, 1);
	if (Thread != nullptr)
	{
		luaL_checktype(L, 2, LUA_TNUMBER);
		double num = lua_tonumber(L, 2);
		if (num < 0)
		{
			WaitForSingleObject(L, INFINITE);
		}
		else
		{
			WaitForSingleObject(L, static_cast<DWORD>(num));
		}
	}
	return 0;
}

int Lua29tnlibs::SetBackgroundColor(lua_State* L)
{
	tn::Color3 clr = {0, 0, 0};
	luaL_checktype(L, 1, LUA_TTABLE);

	lua_pushnil(L);
	if (lua_next(L, 1) != 0)
	{
		clr.Red = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);
	
	if (lua_next(L, 1) != 0)
	{
		clr.Green = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	if (lua_next(L, 1) != 0)
	{
		clr.Blue = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	L->ReferenceProgram->CurrentCamera->BackgroundColor = clr;
	return 0;
}

int Lua29tnlibs::lua_GetDate(lua_State* L)
{
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wDay);
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wMonth);
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wYear);
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wHour);
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wMinute);
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wSecond);

	return 6;
}

int Lua29tnlibs::lua_GetDay(lua_State* L)
{
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wDay);
	return 1;
}

int Lua29tnlibs::lua_GetDayOfTheWeek(lua_State* L)
{
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wDayOfWeek);
	return 1;
}

int Lua29tnlibs::lua_GetMonth(lua_State* L)
{
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wMonth);
	return 1;
}

int Lua29tnlibs::lua_GetYear(lua_State* L)
{
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wYear);
	return 1;
}

int Lua29tnlibs::lua_GetTimeHour(lua_State* L)
{
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wHour);
	return 1;
}

int Lua29tnlibs::lua_GetTimeMinute(lua_State* L)
{
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wMinute);
	return 1;
}

int Lua29tnlibs::lua_GetTimeSecond(lua_State* L)
{
	lua_pushnumber(L, L->ReferenceProgram->LocalTime.wSecond);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int LuaObjectLibs::lua_GetWorldObject(lua_State* L)
{
	size_t narg = lua_gettop(L);

	const char* strname = luaL_tolstring(L, 1, nullptr);

	Object* obj = nullptr;
	size_t sizeofwchar;
	mbstowcs_s(&sizeofwchar, nullptr, 0, strname, 0);
	wchar_t* wbuffer = new wchar_t[sizeofwchar];
	mbstowcs_s(nullptr, wbuffer, sizeofwchar, strname, _TRUNCATE);

	obj = L->ReferenceProgram->CurrentScene->GetChild(wbuffer);
	if (obj == nullptr)
	{
		luaL_error(L, "%s is not a valid object of the current scene\n", strname);
		delete[] wbuffer;
		return 0;
	}

	lua_pushlightuserdata(L, static_cast<void*>(obj));

	delete[] wbuffer;

	return 1;
}

int LuaObjectLibs::lua_GetGUIs(lua_State* L)
{
	size_t narg = lua_gettop(L);
	Object* obj = nullptr;

	obj = L->ReferenceProgram->GameHierachy.GUI;;
	lua_pushlightuserdata(L, static_cast<void*>(obj));

	return 1;
}

int LuaObjectLibs::lua_GetAssets(lua_State* L)
{
	size_t narg = lua_gettop(L);

	Object* obj = nullptr;

	obj = L->ReferenceProgram->GameHierachy.Assets;
	lua_pushlightuserdata(L, static_cast<void*>(obj));

	return 1;
}

int LuaObjectLibs::lua_GetChildObject(lua_State* L)
{
	ProgramObject* parentobj = (ProgramObject*)lua_touserdata(L, 1);
	if (parentobj == nullptr)
	{
		luaL_error(L, "No object (Instance) is selected");
		return 0;
	}

	Object* childobj = nullptr;
	const char* childname = lua_tostring(L, 2);
	size_t sizeofwchar;
	mbstowcs_s(&sizeofwchar, nullptr, 0, childname, 0);
	wchar_t* wbuffer = new wchar_t[sizeofwchar];
	mbstowcs_s(nullptr, wbuffer, sizeofwchar, childname, _TRUNCATE);

	childobj = dynamic_cast<Object*>(parentobj->GetChild(wbuffer));
	if (childobj == nullptr)
	{
		luaL_error(L, "%s is not a valid object of the parent object.\n", childname);
		delete[] wbuffer;
		return 0;
	}

	lua_pushlightuserdata(L, static_cast<void*>(childobj));
	delete[] wbuffer;

	return 1;
}

int LuaObjectLibs::lua_FindFirstChild(lua_State* L)
{
	ProgramObject* parentobj = (ProgramObject*)lua_touserdata(L, 1);
	if (parentobj == nullptr)
	{
		luaL_error(L, "No object (Instance) is selected");
		return 0;
	}

	Object* childobj = nullptr;
	const char* childname = lua_tostring(L, 2);
	size_t sizeofwchar;
	mbstowcs_s(&sizeofwchar, nullptr, 0, childname, 0);
	wchar_t* wbuffer = new wchar_t[sizeofwchar];
	mbstowcs_s(nullptr, wbuffer, sizeofwchar, childname, _TRUNCATE);

	childobj = dynamic_cast<Object*>(parentobj->GetChild(wbuffer));
	if (childobj != nullptr)
	{
		lua_pushboolean(L, TRUE);
		lua_pushlightuserdata(L, childobj);
	}
	else
	{
		lua_pushboolean(L, FALSE);
		lua_pushnil(L);
	}
	delete[] wbuffer;

	return 2;
}

int LuaObjectLibs::lua_WaitForChild(lua_State* L)
{
	ProgramObject* parentobj = (ProgramObject*)lua_touserdata(L, 1);
	if (parentobj == nullptr)
	{
		luaL_error(L, "No object (Instance) is selected\n");
		return 0;
	}

	ProgramObject* childobj = nullptr;
	const char* childname = lua_tostring(L, 2);
	size_t sizeofwchar;
	mbstowcs_s(&sizeofwchar, nullptr, 0, childname, 0);
	wchar_t* wbuffer = new wchar_t[sizeofwchar];
	mbstowcs_s(nullptr, wbuffer, sizeofwchar, childname, _TRUNCATE);

	childobj = dynamic_cast<ProgramObject*>(parentobj->WaitForChild(wbuffer));

	delete[] wbuffer;

	lua_pushlightuserdata(L, static_cast<void*>(childobj));
	return 1;
}

int LuaObjectLibs::lua_GetParentObject(lua_State* L)
{
	ProgramObject* obj = (ProgramObject*)lua_touserdata(L, 1);
	if (obj == nullptr)
	{
		luaL_error(L, "No object (Instance) is selected\n");
		return 0;
	}

	ProgramObject* parentobj = nullptr;
	const char* childname = lua_tostring(L, 2);
	size_t sizeofwchar;
	mbstowcs_s(&sizeofwchar, nullptr, 0, childname, 0);
	wchar_t* wbuffer = new wchar_t[sizeofwchar];
	mbstowcs_s(nullptr, wbuffer, sizeofwchar, childname, _TRUNCATE);

	parentobj = dynamic_cast<ProgramObject*>(obj->GetChild(wbuffer));
	if (parentobj == nullptr)
	{
		luaL_error(L, "%s is not a valid object of the parent object.\n", childname);
		delete[] wbuffer;
		return 0;
	}

	lua_pushlightuserdata(L, static_cast<void*>(parentobj));
	delete[] wbuffer;

	return 1;
}

int LuaObjectLibs::lua_GetInfoObject(lua_State* L)
{
	ProgramObject* obj = (ProgramObject*)lua_touserdata(L, 1);
	if (obj == nullptr)
	{
		luaL_error(L, "No object (Instance) is selected");
		return 0;
	}

	std::wstring wchar_name = obj->GetName();
	size_t sizeofchar;
	wcstombs_s(&sizeofchar, nullptr, 0, wchar_name.data(), 0);
	char* cbuffer = new char[sizeofchar];
	wcstombs_s(nullptr, cbuffer, sizeofchar, wchar_name.data(), _TRUNCATE);

	lua_createtable(L, 0, 2); //1 {}

	lua_pushstring(L, "Name"); //2 Name = ?
	lua_pushstring(L, cbuffer);  //3 Name = strname
	lua_settable(L, -3);

	lua_pushstring(L, "Class");
	lua_pushnumber(L, (lua_Number)obj->GetType());
	lua_settable(L, -3);

	lua_pop(L, 0);
	return 1;
}

int LuaObjectLibs::lua_ModifyObject(lua_State* L)
{
	Object* obj = (Object*)lua_touserdata(L, 1);
	if (obj == nullptr)
	{
		luaL_error(L, "No object (Instance) is selected");
		return 0;
	}

	if (!lua_istable(L, 2)) {
		luaL_error(L, "Expected a table as the first argument");
		return 0;
	}

	lua_pushnil(L);
	while (lua_next(L, 2) != 0) {
		const char* keyname = lua_tostring(L, -2);
		if (keyname)
		{
			if (strcmp(keyname, "Name") == 0)
			{
				const char* strname = lua_tostring(L, -1);
				size_t sizeofwchar;
				mbstowcs_s(&sizeofwchar, nullptr, 0, strname, 0);
				wchar_t* wbuffer = new wchar_t[sizeofwchar];
				std::wstring wstr = L"";
				mbstowcs_s(nullptr, wbuffer, sizeofwchar, strname, _TRUNCATE);
				obj->SetName(wbuffer);
				delete[] wbuffer;
			}
			else if (strcmp(keyname, "Parent") == 0)
			{
				if (lua_isnil(L, -1))
				{
					if (ProgramObject* pObj = dynamic_cast<ProgramObject*>(obj))
					{
						pObj->SetParent(L->ReferenceProgram->CurrentScene);
					}
				}
				else
				{
					if (ProgramObject* pObj = dynamic_cast<ProgramObject*>(obj))
					{
						ProgramObject* newparent = (ProgramObject*)lua_touserdata(L, -1);
						if (newparent != nullptr)
						{
							pObj->SetParent(newparent);
						}
						else
						{
							pObj->SetParent(L->ReferenceProgram->CurrentScene);
						}
					}
					else
					{
						ProgramObject* newparent = (ProgramObject*)lua_touserdata(L, -1);
						if (newparent != nullptr)
						{

							newparent->AddChild(obj);
						}
					}
				}
			}
			else if (strcmp(keyname, "Position") == 0)
			{
				if (lua_istable(L, -1))
				{
					tn::Vector2 v2 = {};

					// Retrieve "X" from the table
					lua_getfield(L, -1, "X");
					if (lua_isnumber(L, -1)) {
						v2.X = (float)lua_tonumber(L, -1);
					}
					lua_pop(L, 1); // Remove "X" value from stack

					// Retrieve "Y" from the table
					lua_getfield(L, -1, "Y");
					if (lua_isnumber(L, -1)) {
						v2.Y = (float)lua_tonumber(L, -1);
					}
					lua_pop(L, 1); // Remove "Y" value from stack


					if (Sprite* spr = dynamic_cast<Sprite*>(obj))
					{
						spr->Position = v2;
					}
				}
			}
		}

		lua_pop(L, 1);
	}

	return 0;
}

int LuaObjectLibs::lua_ReleaseObject(lua_State* L)
{
	Object* obj = static_cast<Object*>(lua_touserdata(L, 1));
	if (tn::Texture* tp = dynamic_cast<tn::Texture*>(obj))
	{
		tp->Release();
	}

	return 0;
}

int LuaObjectLibs::lua_LoadTexture(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TSTRING);

	tn::Texture* TexturePackageMain = new tn::Texture();
	TexturePackageMain->SetName(L"TexturePackage");

	const char* charstr = luaL_tolstring(L, 1, nullptr);
	size_t sizeofwchar;
	mbstowcs_s(&sizeofwchar, nullptr, 0, charstr, 0);
	wchar_t* wbuffer = new wchar_t[sizeofwchar];
	mbstowcs_s(nullptr, wbuffer, sizeofwchar, charstr, _TRUNCATE);
	TexturePackageMain->Filename = wbuffer;

	if (TexturePackageMain->ImportImage() == true and L->ReferenceProgram->MainGraphicDevice->ExportTexture(TexturePackageMain) == true)
	{
		lua_pushboolean(L, TRUE);
		L->ReferenceProgram->GameHierachy.Textures.push_back(TexturePackageMain);
	}
	else
	{
		lua_pushboolean(L, FALSE);
		TexturePackageMain->Release();
		TexturePackageMain = nullptr;
	}
	delete[] wbuffer;

	lua_pushlightuserdata(L, static_cast<void*>(dynamic_cast<Object*>((TexturePackageMain))));

	return 2;
}

#include "../Core/Audio.h"

int LuaObjectLibs::lua_CreateObject(lua_State* L)
{
	size_t narg = lua_gettop(L);

	std::string classname = luaL_tolstring(L, 1, nullptr);

	Object* obj = nullptr;
	
	if (classname == "Sound")
	{
		tn::Sound* audio = new tn::Sound();
		obj = dynamic_cast<Object*>(audio);
	}
	else
	{
		obj = new Object();
	}

	lua_pushlightuserdata(L, static_cast<void*>(obj));

	return 1;
}

int LuaObjectLibs::lua_SetAudioBuffer(lua_State* L)
{
	Object* obj1 = (Object*)lua_touserdata(L, 1);
	Object* obj2 = (Object*)lua_touserdata(L, 2);

	if (tn::Sound* sound = dynamic_cast<tn::Sound*>(obj1))
	{
		if (tn::SoundBuffer* buffer = dynamic_cast<tn::SoundBuffer*>(obj2))
		{
			sound->SetBuffer(buffer);
		}
	}

	return 0;
}

int LuaObjectLibs::lua_PlaySound(lua_State* L)
{
	Object* obj1 = (Object*)lua_touserdata(L, 1);

	if (tn::Sound* sound = dynamic_cast<tn::Sound*>(obj1))
	{
		sound->Play();
	}
	return 0;
}

int LuaObjectLibs::lua_PauseSound(lua_State* L)
{
	Object* obj1 = (Object*)lua_touserdata(L, 1);

	if (tn::Sound* sound = dynamic_cast<tn::Sound*>(obj1))
	{
		sound->Pause();
	}
	return 0;
}

int LuaObjectLibs::lua_StopSound(lua_State* L)
{
	Object* obj1 = (Object*)lua_touserdata(L, 1);

	if (tn::Sound* sound = dynamic_cast<tn::Sound*>(obj1))
	{
		sound->Stop();
	}
	return 0;
}