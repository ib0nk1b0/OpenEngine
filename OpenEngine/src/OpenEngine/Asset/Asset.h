#pragma once

#include "OpenEngine/Core/UUID.h"

namespace OpenEngine {

	using AssetHandle = UUID;

	enum class AssetType : uint16_t
	{
		None = 0,
		Scene = 1,
		Texture2D = 2
	};

	std::string_view AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(std::string_view assetType);

	class Asset
	{
	public:
		AssetHandle Handle;

		virtual AssetType GetType() const = 0;
	};

}
