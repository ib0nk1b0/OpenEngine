#include "oepch.h"
#include "MaterialPanel.h"

#include "OpenEngine/ImGui/ImGuiExtended.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace OpenEngine {
	
	void MaterialPanel::Init()
	{
		s_Display = true;
	}

	void MaterialPanel::SetContext(const Ref<Scene>& scene)
	{
		s_Context = scene;
	}

	void MaterialPanel::ToggleDisplay()
	{
		s_Display = !s_Display;
	}

	void OpenEngine::MaterialPanel::OnImGuiRender()
	{
		if (!s_Display)
			return;

		ImGui::Begin("Materials");

		if (ImGui::Button("Add Material"))
		{
			s_Context->m_Materials.emplace_back();
		}

		for (size_t i = 0; i < s_Context->m_Materials.size(); i++)
		{
			ImGui::PushID(i);

			ImGui::Text("Material index: %d", i);

			Material& material = s_Context->m_Materials[i];
			UI::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			UI::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
			UI::DragFloat("Metalic", &material.Metalic, 0.05f, 0.0f, 1.0f);

			ImGui::Separator();

			ImGui::PopID();
		}
		ImGui::End();
	}

}