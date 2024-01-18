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

	EditorAssetManager::EditorAssetManager()
	{
	}

	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
    {
		if (!IsAssetHandleValid(handle))
			return nullptr;

		Ref<Asset> asset;
		if (IsAssetLoaded(handle))
			asset = m_LoadedAssets.at(handle);
		else
		{
			const AssetMetadata& metadata = GetMetadata(handle);
			asset = AssetImporter::ImportAsset(handle, metadata);
			if (!asset)
				OE_CORE_ERROR("EditorAssetManager::GetAsset - asset import failed!");

			m_LoadedAssets[handle] = asset;
		}

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
		for (auto [h, a] : m_AssetRegistry)
		{
			auto path = GetMetadata(h).FilePath;
			if (path == filepath)
			{
				Ref<Asset> asset = AssetImporter::ImportAsset(h, GetMetadata(h));
				if (asset)
					m_LoadedAssets[h] = asset;

				return;
			}
		}

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

	std::vector<UUID> EditorAssetManager::GetAllAssetHandles()
	{
		std::vector<UUID> assetHandles;
		for (auto [handle, asset] : m_AssetRegistry)
			assetHandles.push_back(handle);
		return assetHandles;
	}

	const bool EditorAssetManager::IsFileValidAsset(const std::filesystem::path& filepath)
	{
		std::string extension = filepath.extension().string();
		return s_AssetExtensionMap.find(filepath.extension()) != s_AssetExtensionMap.end();
	}

	AssetHandle EditorAssetManager::AddToRegistry(const std::filesystem::path& filepath)
	{
		AssetHandle handle;
		AssetMetadata metadata;
		metadata.FilePath = filepath;
		metadata.Type = GetAssetTypeFromFileExtension(filepath.extension());
		OE_CORE_ASSERT(metadata.Type != AssetType::None, "");

		m_AssetRegistry[handle] = metadata;
		SerializeAssetRegistry();
		return handle;
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
			asset["FilePath"] = metadata.FilePath;
			asset["Type"] = AssetTypeToString(metadata.Type);

			jsonData["AssetRegsitry"] += asset;
		};

		std::ofstream jsonOut(path);
		jsonOut << std::setw(4) << jsonData;
	}

	std::map<std::filesystem::path, AssetHandle> EditorAssetManager::DeserializeAssetRegistry()
	{
		m_AssetRegistry.clear();
		m_LoadedAssets.clear();
		std::map<std::filesystem::path, AssetHandle> assetsInRegistry;
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
				AssetMetadata metadata;

				metadata.FilePath = value["FilePath"].get<std::string>();
				metadata.Type = AssetTypeFromString(value["Type"].get<std::string>());

				if (std::filesystem::exists(Project::GetActiveAssetDirectory() / metadata.FilePath))
				{
					m_AssetRegistry[handle] = metadata;
					assetsInRegistry[metadata.FilePath] = handle;
				}
			}
		}

		return assetsInRegistry;
	}

}
