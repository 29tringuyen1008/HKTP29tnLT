#pragma once

enum DrawTypes {
	DrawType_PointList = 1,
	DrawType_LineList = 2,
	DrawType_LineStrip = 3,
	DrawType_TriangleList = 4,
	DrawType_TriangleStrip = 5,
	DrawType_TriangleFan = 6,
};

enum DrawStyles {
	DrawStyle_Normal = 1,
	DrawStyle_UP = 2,
	DrawStyle_Indexed = 3,
};

enum SurfacesDimensionalTypes {
	SDT_Unknown = 0, //Không có gì hoặc bí mật - Unknown
	SDT_16Bits = 1,
	SDT_32Bits = 2,
	SDT_16Bits_3DOnly = 3,
	SDT_32Bits_3DOnly = 4,
	SDT_64Bits = 5,
};

enum DepthStencilTypes {
	D16 = 1,
	D32 = 2,
}; 

enum SwapChainEffects {
	Discard = 1,
	Flip = 2,
	Copy = 3,
	Overlay = 4,
};

enum ShaderTypes {
	Shader_None = 0,
	Shader_Vertex = 1,
	Shader_Pixel = 2,
};