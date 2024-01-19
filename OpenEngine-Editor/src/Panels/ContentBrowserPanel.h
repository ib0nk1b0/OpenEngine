#pragma once

#include "Panel.h"
#include "OpenEngine.h"

#include <filesystem>

namespace OpenEngine {

	class ContentBrowserPanel : public Panel
	{
	public:
		ContentBrowserPanel();

		static std::string GetName() { return "ContentBrowserPanel"; }
		void OnImGuiRender() override;
		void LoadAssets();
	private:
	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_PreviousDirectory;

		std::map<std::filesystem::path, AssetHandle> m_AssetMap;

		Ref<Texture2D> m_FolderIcon, m_FileIcon, m_RefreshIcon;
	};

}