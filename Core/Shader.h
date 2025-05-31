#pragma once
#include "Graphics.h"

namespace tn {
	class GraphicsDevice;

	struct ShaderDescription {
		const wchar_t* Filename;
		const char* StartingPoint;
		const char* ShaderModel;
		ShaderTypes ShaderType;
	};

	class Shader {
	public:
		void GetCurrentDescription(ShaderDescription* desc);
		void Release();

	private:
		Shader();

		friend class GraphicsDevice;

		ShaderDescription m_desc = {};
		void* Data = nullptr;
	};
}