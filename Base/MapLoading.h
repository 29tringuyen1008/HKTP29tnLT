#pragma once

#define LOAD2DMAP_ERR_NO_SCENE -1
#define LOAD2DMAP_FILE_NOT_FOUND -2
#define LOAD2DMAP_ERR_SCENE_NOT_2D_SURFACE -3

class Program;
class Scene;

namespace MapLoading
{
	int Load2DMap(Program* Program, Scene** TargetScene, const wchar_t* FilenameJSON);
}