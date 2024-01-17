#pragma once

#include "OpenEngine.h"

#include <filesystem>

namespace OpenEngine {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		
		void OnImGuiRender();
	private:
		void LoadAssets();
	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_PreviousDirectory;

		std::map<std::filesystem::path, AssetHandle> m_AssetMap;

		Ref<Texture2D> m_FolderIcon, m_FileIcon, m_RefreshIcon;
	};

}