#pragma once

#include "OpenEngine/Core/Core.h"
#include "OpenEngine/Core/Log.h"

#include <filesystem>
#include <string>

namespace OpenEngine {

	struct ProjectConfig
	{
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path ScriptModulePath;
	};

	class Project
	{
	public:

		static const std::filesystem::path& GetProjectDirectory()
		{
			OE_CORE_ASSERT(s_ActiveProject, "");
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			OE_CORE_ASSERT(s_ActiveProject, "");
			return GetProjectDirectory() / s_ActiveProject->GetConfig().AssetDirectory;
		}

		static std::filesystem::path GetAssetFilesystemPath(const std::filesystem::path& path)
		{
			OE_CORE_ASSERT(s_ActiveProject, "");
			return GetAssetDirectory() / path;
		}

		static Ref<Project>& GetActive() { return s_ActiveProject; }

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& filepath);
		static bool SaveActive(const std::filesystem::path& filepath);
	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;

		inline static Ref<Project> s_ActiveProject;
	};

}
