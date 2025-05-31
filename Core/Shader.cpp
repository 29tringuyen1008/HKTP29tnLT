#include "Shader.h"

tn::Shader::Shader() {

}

void tn::Shader::GetCurrentDescription(ShaderDescription* desc) {
	(*desc) = m_desc;
}

void tn::Shader::Release() {
	delete this;
}