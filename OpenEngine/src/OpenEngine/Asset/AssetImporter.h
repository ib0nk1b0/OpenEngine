#pragma once

#include "OpenEngine/Asset/AssetMetadata.h"

namespace OpenEngine {

	class AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};

}
