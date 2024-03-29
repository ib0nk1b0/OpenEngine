#include "oepch.h"
#include "ContentBrowserPanel.h"

#include "OpenEngine/Utils/PlatformUtils.h"
#include "OpenEngine/ImGui/ImGuiFonts.h"

#include <imgui/imgui.h>

namespace OpenEngine {

	extern const std::filesystem::path g_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(g_AssetPath)
	{
		m_ForlderIcon = Texture2D::Create("assets/icons/Folder.png");
		m_FileIcon = Texture2D::Create("assets/icons/Icon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		ImGuiIO& io = ImGui::GetIO();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));
		
		{
			UI::ScopedFont boldLarge(UI::Fonts::Get("BoldLarge"));
			if (ImGui::Button("<") && m_CurrentDirectory != std::filesystem::path(g_AssetPath))
			{
				m_PreviousDirectory = m_CurrentDirectory;
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}

			ImGui::SameLine();

			if (ImGui::Button(">") && !m_PreviousDirectory.empty())
			{
				m_CurrentDirectory = m_PreviousDirectory;
				m_PreviousDirectory.clear();
			}

			ImGui::PopStyleVar();
		}

		ImGui::SameLine();
		{
			UI::ScopedFont bold(UI::Fonts::Get("Bold"));
			ImGui::Text("Path: %s", m_CurrentDirectory.generic_string().c_str());
		}

		float thumbnailSize = 64.0f;
		float padding = 12.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);
		
		int i = 0;

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			ImGui::PushID(i++);

			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			std::string filenameString = relativePath.filename().string();
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_ForlderIcon : m_FileIcon;

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			ImGui::PopStyleColor();

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);

				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}

			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::End();
	}

}