#pragma once

#include "OpenEngine/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace OpenEngine {

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* windowHandle);
		virtual ~VulkanContext();

		virtual void Init();
		virtual void SwapBuffers();
	private:
		GLFWwindow* m_WindowHandle;
	};

}