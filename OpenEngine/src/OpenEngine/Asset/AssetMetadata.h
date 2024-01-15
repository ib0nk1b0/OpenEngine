#pragma once

#include "OpenEngine/Asset/Asset.h"

#include <filesystem>

namespace OpenEngine {

	struct AssetMetadata
	{
		AssetType Type = AssetType::None;
		std::filesystem::path FilePath;

		operator bool() const { return Type != AssetType::None; }
	};

}
