#include "oepch.h"
#include "TextureImporter.h"

#include "OpenEngine/Project/Project.h"

#include <stb_image.h>

namespace OpenEngine {

    Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
    {
        return LoadTexture2D(Project::GetAssetDirectory() / metadata.FilePath);
    }

    Ref<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& path)
    {
		OE_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			OE_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
		}
		OE_CORE_ASSERT(data, "Failed to load image!");

		TextureSpecification spec;
		spec.Width = width;
		spec.Height = height;

		switch (channels)
		{
		case 3:
			spec.Format = ImageFormat::RGB8;
			break;
		case 4:
			spec.Format = ImageFormat::RGBA8;
			break;
		}

		stbi_image_free(data);

		Ref<Texture2D> texture = Texture2D::Create(spec);
		texture->SetData(data, width * height * channels);
		return texture;
    }

}
