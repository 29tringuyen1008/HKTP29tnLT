#pragma once
#include "Base.h"
#include "Lua/lua.hpp"

namespace Lua29tnlibs
{
	int wait(lua_State* L);
	int lua_IsProgramRunning(lua_State* L);
	int lua_CreateThread(lua_State* L);
	int lua_SuspendThread(lua_State* L);
	int lua_ResumeThread(lua_State* L);
	int lua_CloseThread(lua_State* L);
	int lua_WaitForThread(lua_State* L);
	int SetBackgroundColor(lua_State* L);
	int lua_GetDate(lua_State* L);
	int lua_GetDay(lua_State* L);
	int lua_GetDayOfTheWeek(lua_State* L);
	int lua_GetMonth(lua_State* L);
	int lua_GetYear(lua_State* L);
	int lua_GetTimeHour(lua_State* L);
	int lua_GetTimeMinute(lua_State* L);
	int lua_GetTimeSecond(lua_State* L);
}

namespace LuaObjectLibs
{
	int lua_GetWorldObject(lua_State* L);
	int lua_GetGUIs(lua_State* L);
	int lua_GetAssets(lua_State* L);

	int lua_GetChildObject(lua_State* L);
	int lua_FindFirstChild(lua_State* L);
	int lua_WaitForChild(lua_State* L);
	int lua_GetParentObject(lua_State* L);
	int lua_GetInfoObject(lua_State* L);
	int lua_ModifyObject(lua_State* L);
	int lua_ReleaseObject(lua_State* L);
	int lua_LoadTexture(lua_State* L);

	int lua_CreateObject(lua_State* L);

	int lua_SetAudioBuffer(lua_State* L);
	int lua_PlaySound(lua_State* L);
	int lua_PauseSound(lua_State* L);
	int lua_StopSound(lua_State* L);
}