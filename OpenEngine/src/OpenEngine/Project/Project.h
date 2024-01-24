#pragma once

#include "OpenEngine/Core/Core.h"
#include "OpenEngine/Core/Log.h"

#include "OpenEngine/Asset/RuntimeAssetManager.h"
#include "OpenEngine/Asset/EditorAssetManager.h"

#include <filesystem>
#include <string>

namespace OpenEngine {

	struct ProjectConfig
	{
		std::string Name = "Untitled";

		AssetHandle StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path AssetRegistryPath;
		std::filesystem::path ScriptModulePath;
	};

	class Project
	{
	public:

		static const std::filesystem::path& GetProjectDirectory() { return s_ActiveProject->m_ProjectDirectory; }
		static std::filesystem::path GetAssetDirectory() { return GetProjectDirectory() / s_ActiveProject->GetConfig().AssetDirectory; }
		static std::filesystem::path GetAssetRegistryPath() { return GetAssetDirectory() / s_ActiveProject->m_Config.AssetRegistryPath; }
		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path) { return GetAssetDirectory() / path; }

		static const std::filesystem::path& GetActiveProjectDirectory()
		{
			OE_CORE_ASSERT(s_ActiveProject, "");
			return s_ActiveProject->GetProjectDirectory();
		}

		static std::filesystem::path GetActiveAssetDirectory()
		{
			OE_CORE_ASSERT(s_ActiveProject, "");
			return s_ActiveProject->GetAssetDirectory();
		}

		static std::filesystem::path GetActiveAssetRegistryPath()
		{
			OE_CORE_ASSERT(s_ActiveProject, "");
			return s_ActiveProject->GetAssetRegistryPath();
		}

		static std::filesystem::path GetActiveAssetFileSystemPath(const std::filesystem::path& path)
		{
			OE_CORE_ASSERT(s_ActiveProject, "");
			return s_ActiveProject->GetAssetFileSystemPath(path);
		}

		static Ref<Project>& GetActive() { return s_ActiveProject; }

		ProjectConfig& GetConfig() { return m_Config; }

		Ref<EditorAssetManager> GetAssetManager() { return m_AssetManager; }

		static void Create(const ProjectConfig& config, const std::filesystem::path& folderLocation);
		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& filepath);
		static bool SaveActive(const std::filesystem::path& filepath);
	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;
		Ref<EditorAssetManager> m_AssetManager = CreateRef<EditorAssetManager>();

		inline static Ref<Project> s_ActiveProject;
	};

}
