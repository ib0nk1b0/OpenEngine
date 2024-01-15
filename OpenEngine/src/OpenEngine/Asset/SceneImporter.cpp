#include "oepch.h"
#include "SceneImporter.h"

#include "OpenEngine/Project/Project.h"
#include "OpenEngine/Scene/SceneSerializer.h"

namespace OpenEngine {

    Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
    {
        return LoadScene(Project::GetAssetDirectory() / metadata.FilePath);
    }

    Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
    {
        Ref<Scene> scene = CreateRef<Scene>();
        SceneSerializer serializer(scene);
        serializer.Deserialize(path.string());
        return scene;
    }

    void SceneImporter::SaveScene(Ref<Scene> scene, const std::filesystem::path& path)
    {
        SceneSerializer serializer(scene);
        std::filesystem::path scenePath(Project::GetAssetDirectory() / path);
        serializer.Serialize(scenePath.string());
    }

}
