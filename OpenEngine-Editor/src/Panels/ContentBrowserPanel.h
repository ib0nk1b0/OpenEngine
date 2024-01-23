#pragma once

#include "OpenEngine.h"

#include <filesystem>

namespace OpenEngine {

	class ContentBrowserPanel
	{
	public:
		static void Init();

		static void OnImGuiRender();
		static void LoadAssets();
	private:
		inline static std::filesystem::path m_BaseDirectory;
		inline static std::filesystem::path m_CurrentDirectory;
		inline static std::filesystem::path m_PreviousDirectory;

		inline static std::map<std::filesystem::path, AssetHandle> m_AssetMap;

		inline static Ref<Texture2D> m_FolderIcon, m_FileIcon, m_RefreshIcon;
	};

}