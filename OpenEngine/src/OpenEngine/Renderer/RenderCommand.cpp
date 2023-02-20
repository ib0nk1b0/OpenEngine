#include "oepch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace OpenEngine {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}