#include "oepch.h"
#include "ImGuiExtended.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ImGui {

	float ImGui::GetLineHeight()
	{
		return GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	}

	bool ImGui::OEDragFloat(const std::string& label, float* value, float speed, float min, float max, float columnWidth, const char* format)
	{
		bool dragFloatUsed = false;
		PushID(label.c_str());

		Columns(2);
		SetColumnWidth(0, columnWidth);
		Text(label.c_str());
		NextColumn();

		if (DragFloat("##", value, speed, min, max, format))
			dragFloatUsed = true;

		Columns(1);

		PopID();

		return dragFloatUsed;
	}

	bool ImGui::OEDragInt(const std::string& label, int* value, int speed, int min, int max, float columnWidth)
	{
		bool used = false;

		PushID(label.c_str());

		Columns(2);
		SetColumnWidth(0, columnWidth);
		Text(label.c_str());
		NextColumn();

		if (DragInt("##", value, speed, min, max))
			used = true;

		Columns(1);

		PopID();

		return used;
	}

	void ImGui::OEVec3Controls(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		PushID(label.c_str());

		Columns(2);
		SetColumnWidth(0, columnWidth);
		Text(label.c_str());
		NextColumn();

		PushMultiItemsWidths(3, CalcItemWidth());
		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GetLineHeight();

		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		PushStyleColor(ImGuiCol_Button, ImVec4{ 0.4f, 0.1f, 0.15f, 1.0f });
		PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.65f, 0.2f, 0.2f, 1.0f });
		PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.4f, 0.1f, 0.15f, 1.0f });
		PushFont(boldFont);
		if (Button("X", buttonSize))
			values.x = resetValue;
		PopFont();
		PopStyleColor(3);

		SameLine();
		DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		PopItemWidth();
		SameLine();

		PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.35f, 0.2f, 1.0f });
		PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.55f, 0.3f, 1.0f });
		PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.35f, 0.2f, 1.0f });
		PushFont(boldFont);
		if (Button("Y", buttonSize))
			values.y = resetValue;
		PopFont();
		PopStyleColor(3);

		SameLine();
		DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		PopItemWidth();
		SameLine();

		PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.4f, 1.0f });
		PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.4f, 1.0f });
		PushFont(boldFont);
		if (Button("Z", buttonSize))
			values.z = resetValue;
		PopFont();
		PopStyleColor(3);

		SameLine();
		DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		PopItemWidth();

		PopStyleVar();

		Columns(1);

		PopID();
	}

	void ImGui::OEColorEdit4(const char* label, float* value)
	{
		PushID(label);

		Columns(2);
		SetColumnWidth(0, 100.0f);
		Text(label);
		NextColumn();

		ColorEdit4("##", value);

		Columns(1);

		PopID();
	}

	void ImGui::OEColorEdit3(const char* label, float* value)
	{
		PushID(label);

		Columns(2);
		SetColumnWidth(0, 100.0f);
		Text(label);
		NextColumn();

		ColorEdit3("##", value);

		Columns(1);

		PopID();
	}

}