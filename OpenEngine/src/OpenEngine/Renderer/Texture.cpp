#include "oepch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace OpenEngine {

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture2D>(path);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}