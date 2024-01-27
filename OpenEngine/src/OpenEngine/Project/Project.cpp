#include "oepch.h"
#include "Project.h"

#include "OpenEngine/Project/ProjectSerializer.h"

#include <NFD-Extended/nfd.hpp>

namespace OpenEngine {

    void Project::Create(const ProjectConfig& config, const std::filesystem::path& folderLocation)
    {
        Ref<Project> project = CreateRef<Project>();
        ProjectConfig& projectConfig = project->GetConfig();
        projectConfig = config;

        std::filesystem::path fullPath = folderLocation / config.Name;

        std::filesystem::create_directories(fullPath);
        std::filesystem::create_directories(fullPath / projectConfig.AssetDirectory);
        std::filesystem::path filename(projectConfig.Name + ".oeproj");
        std::filesystem::path projectSerializerPath = fullPath / filename;
        ProjectSerializer serializer(project);
        serializer.Serialize(projectSerializerPath);

        project->m_ProjectDirectory = fullPath;
        s_ActiveProject = project;
    }

    Ref<Project> Project::New()
    {
        s_ActiveProject = CreateRef<Project>();
        return s_ActiveProject;
    }

    Ref<Project> Project::Load(const std::filesystem::path& filepath)
    {
        Ref<Project> project = CreateRef<Project>();

        ProjectSerializer serializer(project);
        if (serializer.Deserialize(filepath))
        {
            project->m_ProjectDirectory = filepath.parent_path();
            s_ActiveProject = project;
            return s_ActiveProject;
        }

        return nullptr;

    }

    bool Project::SaveActive(const std::filesystem::path& filepath)
    {
        ProjectSerializer serializer(s_ActiveProject);
        if (serializer.Serialize(filepath))
        {
            s_ActiveProject->m_ProjectDirectory = filepath.parent_path();
            return true;
        }

        return false;
    }

}
