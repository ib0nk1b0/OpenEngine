#pragma once

#include "OpenEngine/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace OpenEngine {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		virtual ~OpenGLContext();

		virtual void Init();
		virtual void SwapBuffers();
	private:
		GLFWwindow* m_WindowHandle;
	};

}