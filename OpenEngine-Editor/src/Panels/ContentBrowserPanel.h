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
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_ForlderIcon, m_FileIcon;
	};

}