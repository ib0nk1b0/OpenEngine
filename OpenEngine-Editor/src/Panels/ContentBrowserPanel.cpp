#include "oepch.h"
#include "ContentBrowserPanel.h"

#include "OpenEngine/Asset/TextureImporter.h"
#include "OpenEngine/Asset/AssetManager.h"
#include "OpenEngine/Utils/PlatformUtils.h"
#include "OpenEngine/ImGui/ImGuiFonts.h"

#include <imgui/imgui.h>

namespace OpenEngine {

	void ContentBrowserPanel::Init()
	{
		m_BaseDirectory = Project::GetAssetDirectory();
		m_CurrentDirectory = m_BaseDirectory;
		m_FolderIcon = TextureImporter::LoadTexture2D("Resources/Icons/FolderIcon.png");
		m_FileIcon = TextureImporter::LoadTexture2D("Resources/Icons/FileIcon.png");
		m_RefreshIcon = TextureImporter::LoadTexture2D("Resources/Icons/RefreshIcon.png");

		LoadAssets();
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));
		
		{
			UI::ScopedFont boldLarge(UI::Fonts::Get("BoldLarge"));
			if (ImGui::Button("<") && m_CurrentDirectory != m_BaseDirectory)
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

			ImGui::SameLine();

			if (ImGui::ImageButton((ImTextureID)m_RefreshIcon->GetRendererID(), { 16, 16 }, { 0, 1 }, { 1, 0 }))
			{
				LoadAssets();
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
			std::filesystem::path relativePath = path.lexically_relative(Project::GetActiveAssetDirectory());
			std::string filenameString = relativePath.filename().string();
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_FolderIcon : m_FileIcon;

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			ImGui::PopStyleColor();

			if (ImGui::BeginDragDropSource())
			{
				AssetHandle handle = m_AssetMap[relativePath];
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &handle, sizeof(handle), ImGuiCond_Once);

				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}

			if (!directoryEntry.is_directory() && ImGui::IsItemHovered())
			{
				std::filesystem::path relativePath = std::filesystem::relative(directoryEntry.path(), Project::GetActiveAssetDirectory());
				if (Project::GetActive()->GetAssetManager()->IsFileValidAsset(relativePath)
					&& m_AssetMap.find(relativePath) != m_AssetMap.end())
				{
					auto handle = m_AssetMap[relativePath];
					auto type = Project::GetActive()->GetAssetManager()->GetMetadata(handle).Type;
					if (type == AssetType::Scene)
					{
						if (ImGui::BeginPopupContextWindow(0))
						{
							if (ImGui::MenuItem("Set Start up scene."))
							{
								/*auto& config = Project::GetActive()->GetConfig();
								config.StartScene = handle;
								std::filesystem::path filename(config.Name + ".oeproj");
								std::filesystem::path filepath(Project::GetActiveProjectDirectory() / filename);
								Project::SaveActive(filepath);*/
							}

							ImGui::EndPopup();
						}
					}
				}
			}

			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::End();
	}

	void ContentBrowserPanel::LoadAssets()
	{
		auto registryPath = Project::GetActiveAssetRegistryPath();
		if (std::filesystem::exists(registryPath))
			m_AssetMap = Project::GetActive()->GetAssetManager()->DeserializeAssetRegistry();

		for (auto& directoryEntry : std::filesystem::recursive_directory_iterator(m_BaseDirectory))
		{
			if (directoryEntry.is_directory())
				continue;

			std::filesystem::path filepath = directoryEntry.path().lexically_relative(Project::GetActiveAssetDirectory());

			if (Project::GetActive()->GetAssetManager()->IsFileValidAsset(filepath) && m_AssetMap.find(filepath) == m_AssetMap.end())
			{
				UUID handle = Project::GetActive()->GetAssetManager()->AddToRegistry(filepath);
				m_AssetMap[filepath] = handle;
			}
		}
	}

}
