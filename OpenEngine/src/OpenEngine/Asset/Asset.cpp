#include "oepch.h"
#include "Asset.h"

namespace OpenEngine {

	std::string_view AssetTypeToString(AssetType type)
	{
		switch (type)
		{
			case AssetType::None:      return "None";
			case AssetType::Scene:     return "Scene";
			case AssetType::Texture2D: return "Texture2D";
		}

		return "<Invalid>";
	}

	AssetType AssetTypeFromString(std::string_view assetType)
	{
		if (assetType == "None")      return AssetType::None;
		if (assetType == "Scene")     return AssetType::Scene;
		if (assetType == "Texture2D") return AssetType::Texture2D;

		return AssetType::None;
	}

}