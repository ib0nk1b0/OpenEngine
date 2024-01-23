#pragma once

#include "OpenEngine/Core/Core.h"
#include "OpenEngine/Scene/Scene.h"

namespace OpenEngine {

	class MaterialPanel
	{
	public:
		static void Init();
		static void SetContext(const Ref<Scene>& scene);
		static void ToggleDisplay();
		static void OnImGuiRender();
	private:
		inline static bool s_Display;

		inline static Ref<Scene> s_Context;
	};

}