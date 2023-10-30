#include "oepch.h"
#include "MaterialPanel.h"

#include "OpenEngine/ImGui/ImGuiExtended.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace OpenEngine {

	MaterialPanel::MaterialPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void MaterialPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
	}

	void OpenEngine::MaterialPanel::OnImGuiRender()
	{
		if (!m_Display)
			return;

		ImGui::Begin("Materials");

		if (ImGui::Button("Add Material"))
		{
			m_Context->m_Materials.emplace_back();
		}

		for (size_t i = 0; i < m_Context->m_Materials.size(); i++)
		{
			ImGui::PushID(i);

			ImGui::Text("Material index: %d", i);

			Material& material = m_Context->m_Materials[i];
			UI::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			UI::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
			UI::DragFloat("Metalic", &material.Metalic, 0.05f, 0.0f, 1.0f);

			ImGui::Separator();

			ImGui::PopID();
		}
		ImGui::End();
	}

}