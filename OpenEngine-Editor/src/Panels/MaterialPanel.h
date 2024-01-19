#pragma once

#include "Panel.h"

#include "OpenEngine/Core/Core.h"
#include "OpenEngine/Scene/Scene.h"

namespace OpenEngine {

	class MaterialPanel : public Panel
	{
	public:
		MaterialPanel() = default;
		MaterialPanel(const Ref<Scene>& scene);

		void Display(bool display) { m_Display = display; }

		void SetContext(const Ref<Scene>& scene);
		static std::string GetName() { return "MaterialPanel"; }
		void OnImGuiRender() override;
	private:
		bool m_Display = true;

		Ref<Scene> m_Context;
	};

}