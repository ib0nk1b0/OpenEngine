#pragma once

#include "OpenEngine/Asset/AssetMetadata.h"

#include "OpenEngine/Renderer/Texture.h"

namespace OpenEngine {

	class TextureImporter
	{
	public:
		static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Texture2D> LoadTexture2D(const std::filesystem::path& path);
	};

}
