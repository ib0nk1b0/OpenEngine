#include "oepch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace OpenEngine {

	//RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
	RendererAPI* RenderCommand::s_RendererAPI = new VulkanRendererAPI;

}