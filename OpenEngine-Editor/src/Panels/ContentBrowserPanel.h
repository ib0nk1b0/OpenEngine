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

		std::vector<std::filesystem::directory_entry> m_Directories;

		Ref<Texture2D> m_ForlderIcon, m_FileIcon;
	};

}