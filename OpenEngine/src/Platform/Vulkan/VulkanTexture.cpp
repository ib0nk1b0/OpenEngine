#include "oepch.h"
#include "VulkanTexture.h"

#include "stb_image.h"

namespace OpenEngine {

	VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
	}

	VulkanTexture2D::VulkanTexture2D(const std::string& path)
		: m_Path(path)
	{
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
	}

	void VulkanTexture2D::SetData(void* data, uint32_t size)
	{
	}

	void VulkanTexture2D::Bind(uint32_t slot) const
	{
	}

}