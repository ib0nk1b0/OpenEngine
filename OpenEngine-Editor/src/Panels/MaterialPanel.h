#pragma once

#include "OpenEngine/Core/Core.h"
#include "OpenEngine/Scene/Scene.h"

namespace OpenEngine {

	class MaterialPanel
	{
	public:
		MaterialPanel() = default;
		MaterialPanel(const Ref<Scene>& scene);

		void Display(bool display) { m_Display = display; }

		void SetContext(const Ref<Scene>& scene);
		void OnImGuiRender();
	private:
		bool m_Display = true;

		Ref<Scene> m_Context;
	};

}