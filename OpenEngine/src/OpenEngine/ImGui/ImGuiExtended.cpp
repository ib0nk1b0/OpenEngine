#include "oepch.h"
#include "ImGuiExtended.h"

#include "OpenEngine/ImGui/ImGuiFonts.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace OpenEngine::UI {

	float OpenEngine::UI::GetLineHeight()
	{
		return GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	}

	bool OpenEngine::UI::DragFloat(const std::string& label, float* value, float speed, float min, float max, float columnWidth, const char* format)
	{
		bool dragFloatUsed = false;
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		if (ImGui::DragFloat("##", value, speed, min, max, format))
			dragFloatUsed = true;

		ImGui::Columns(1);
		
		ImGui::PopID();

		return dragFloatUsed;
	}

	bool OpenEngine::UI::DragInt(const std::string& label, int* value, int speed, int min, int max, float columnWidth)
	{
		bool used = false;

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		if (ImGui::DragInt("##", value, speed, min, max))
			used = true;

		ImGui::Columns(1);

		ImGui::PopID();

		return used;
	}

	void OpenEngine::UI::Vec3Controls(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GetLineHeight();

		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.4f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.65f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.4f, 0.1f, 0.15f, 1.0f });
		OpenEngine::UI::Fonts::PushFont("Bold");
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		OpenEngine::UI::Fonts::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.35f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.55f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.35f, 0.2f, 1.0f });
		OpenEngine::UI::Fonts::PushFont("Bold");
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		OpenEngine::UI::Fonts::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.4f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.4f, 1.0f });
		OpenEngine::UI::Fonts::PushFont("Bold");
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		OpenEngine::UI::Fonts::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void OpenEngine::UI::ColorEdit4(const char* label, float* value)
	{
		ImGui::PushID(label);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::ColorEdit4("##", value);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void OpenEngine::UI::ColorEdit3(const char* label, float* value)
	{
		ImGui::PushID(label);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::ColorEdit3("##", value);

		ImGui::Columns(1);

		ImGui::PopID();
	}

}