#pragma once

#include "OpenEngine/Asset/AssetManagerBase.h"
#include "OpenEngine/Asset/AssetMetadata.h"

#include <map>

namespace OpenEngine {

	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager();

		virtual Ref<Asset> GetAsset(AssetHandle handle) override;

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual AssetType GetAssetType(AssetHandle handle) const override;

		void ImportAsset(const std::filesystem::path& filepath);

		const AssetMetadata& GetMetadata(AssetHandle handle) const;
		const std::filesystem::path& GetFilePath(AssetHandle handle) const;

		const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }

		std::vector<UUID> GetAllAssetHandles();

		const bool IsFileValidAsset(const std::filesystem::path& filepath);

		void AddToRegistry(const std::filesystem::path& filepath);

		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();
	private:
		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;
	};

}