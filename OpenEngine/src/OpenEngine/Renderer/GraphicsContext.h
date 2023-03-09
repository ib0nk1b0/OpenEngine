#pragma once

#include "OpenEngine/Renderer/Renderer.h"

struct GLFWwindow;

namespace OpenEngine {

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static GraphicsContext* Create(GLFWwindow* window);
	};

}
