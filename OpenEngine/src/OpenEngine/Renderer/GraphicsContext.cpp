#include "oepch.h"
#include "GraphicsContext.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace OpenEngine {

	GraphicsContext* GraphicsContext::Create(GLFWwindow* window)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return new OpenGLContext(window);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}