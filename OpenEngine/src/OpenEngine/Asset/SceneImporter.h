#pragma once

#include "OpenEngine/Asset/AssetMetadata.h"

#include "OpenEngine/Scene/Scene.h"

namespace OpenEngine {

	class SceneImporter
	{
	public:
		static Ref<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Scene> LoadScene(const std::filesystem::path& path);

		static void SaveScene(Ref<Scene> scene, const std::filesystem::path& path);
	};

}
