#pragma once

#include "OpenEngine/Core/Core.h"
#include "OpenEngine/Renderer/Texture.h"

#include <filesystem>

namespace OpenEngine {

	struct MSDFData;

	class Font
	{
	public:
		Font(const std::filesystem::path& filepath);
		~Font();

		Ref<Texture2D> GetAtlasTexture() { return m_AtlasTexture; }
	private:
		MSDFData* m_Data;
		Ref<Texture2D> m_AtlasTexture;
	};

}
