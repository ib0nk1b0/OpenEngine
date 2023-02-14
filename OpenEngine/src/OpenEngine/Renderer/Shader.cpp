#include "oepch.h"
#include "Shader.h"

#include "OpenEngine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace OpenEngine {

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::OpenGL:  return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}