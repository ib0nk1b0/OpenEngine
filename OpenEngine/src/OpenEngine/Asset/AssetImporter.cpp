#include "oepch.h"
#include "AssetImporter.h"

#include "OpenEngine/Asset/SceneImporter.h"
#include "OpenEngine/Asset/TextureImporter.h"

#include <map>

namespace OpenEngine {

	using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
		{ AssetType::Texture2D, TextureImporter::ImportTexture2D },
		{ AssetType::Scene, SceneImporter::ImportScene }
	};
	

	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
		{
			OE_CORE_ERROR("No importer available for asset type: {}", (uint16_t)metadata.Type);
			return nullptr;
		}

		return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
	}

}