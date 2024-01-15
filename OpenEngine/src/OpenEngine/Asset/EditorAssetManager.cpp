#include "oepch.h"
#include "EditorAssetManager.h"

#include "OpenEngine/Asset/AssetImporter.h"
#include "OpenEngine/Project/Project.h"

#include <json.hpp>

namespace OpenEngine {

	static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
		{ ".openengine", AssetType::Scene },
		{ ".png", AssetType::Texture2D },
		{ ".jpg", AssetType::Texture2D },
		{ ".jpeg", AssetType::Texture2D }
	};

	static AssetType GetAssetTypeFromFileExtension(const std::filesystem::path& extension)
	{
		if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
		{
			OE_CORE_WARN("Could not find AssetType for {}", extension);
			return AssetType::None;
		}

		return s_AssetExtensionMap.at(extension);
	}

    Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
    {
		// 1. check if handle is valid
		if (!IsAssetHandleValid(handle))
			return nullptr;

		// 2. check if asset needs load (and if so, load)
		Ref<Asset> asset;
		if (IsAssetLoaded(handle))
		{
			asset = m_LoadedAssets.at(handle);
		}
		else
		{
			// load asset
			const AssetMetadata& metadata = GetMetadata(handle);
			asset = AssetImporter::ImportAsset(handle, metadata);
			if (!asset)
			{
				// import failed
				OE_CORE_ERROR("EditorAssetManager::GetAsset - asset import failed!");
			}
			m_LoadedAssets[handle] = asset;
		}
		// 3. return asset
		return asset;
    }

	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	AssetType EditorAssetManager::GetAssetType(AssetHandle handle) const
	{
		if (!IsAssetHandleValid(handle))
			return AssetType::None;

		return m_AssetRegistry.at(handle).Type;
	}

	void EditorAssetManager::ImportAsset(const std::filesystem::path& filepath)
	{
		AssetHandle handle; // generate new handle
		AssetMetadata metadata;
		metadata.FilePath = filepath;
		metadata.Type = GetAssetTypeFromFileExtension(filepath.extension());
		OE_CORE_ASSERT(metadata.Type != AssetType::None, "");
		Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
		if (asset)
		{
			asset->Handle = handle;
			m_LoadedAssets[handle] = asset;
			m_AssetRegistry[handle] = metadata;
			SerializeAssetRegistry();
		}
	}

	const AssetMetadata& EditorAssetManager::GetMetadata(AssetHandle handle) const
	{
		static AssetMetadata s_NullMetadata;
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
			return s_NullMetadata;

		return it->second;
	}

	const std::filesystem::path& EditorAssetManager::GetFilePath(AssetHandle handle) const
	{
		return GetMetadata(handle).FilePath;
	}

	void EditorAssetManager::SerializeAssetRegistry()
	{
		auto path = Project::GetActiveAssetRegistryPath();

		nlohmann::ordered_json jsonData;
		jsonData["AssetRegsitry"] = {};
		std::vector<nlohmann::ordered_json> jsonAssets;

		for (const auto& [handle, metadata] : m_AssetRegistry)
		{
			nlohmann::ordered_json asset;
			asset["Handle"] = (uint64_t)handle;
			asset["Filepath"] = metadata.FilePath;
			asset["Type"] = metadata.Type;

			jsonData["AssetRegsitry"] += asset;
		};

		std::ofstream jsonOut(path);
		jsonOut << std::setw(4) << jsonData;
	}

	bool EditorAssetManager::DeserializeAssetRegistry()
	{
		auto path = Project::GetActiveAssetRegistryPath();

		std::ifstream file(path);
		nlohmann::json deserializedJson;
		file >> deserializedJson;

		if (deserializedJson.contains("AssetRegsitry"))
		{
			auto& assets = deserializedJson["AssetRegsitry"];

			for (auto& [key, value] : assets.items())
			{
				AssetHandle handle = value["Handle"].get<uint64_t>();
				auto& metadata = m_AssetRegistry[handle];

				metadata.FilePath = value["FilePath"].get<std::string>();
				metadata.Type = AssetTypeFromString(value["Type"].get<std::string>());
			}
			return true;
		}

		return false;
	}

}
