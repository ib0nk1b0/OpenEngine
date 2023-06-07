#include "oepch.h"
#include "VulkanFramebuffer.h"

namespace OpenEngine {

	static const uint32_t s_MaxFramebufferSize = 8192;

#if 0
	namespace Utils {

		static GLenum TextureTarget(bool multisampling)
		{
			return multisampling ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}
		
		static void CreateTextures(bool multisampling, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampling), count, outID);
		}
		
		static void BindTexture(bool multisampling, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampling), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampling = samples > 1;

			if (multisampling)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampling), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampling = samples > 1;

			if (multisampling)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampling), id, 0);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
			}

			return false;
		}

		static GLenum OEFramebufferTextureFormatToGL(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::RGBA8: return GL_RGBA8;
				case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}
			OE_CORE_ASSERT(false, "");
			return 0;
		}
	
	}
#endif

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec) {}

#if 0
	VulkanFramebuffer::VulkanFramebuffer(VulkanFramebufferInputSpecification inputChuck, std::vector<SwapChainFrame>& frames)
	{

		for (int i = 0; i < frames.size(); i++)
		{
			std::vector<vk::ImageView> attachments = {
				frames[i].ImageView
			};

			vk::FramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.flags = vk::FramebufferCreateFlags();
			framebufferCreateInfo.renderPass = inputChuck.RenderPass;
			framebufferCreateInfo.attachmentCount = attachments.size();
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = inputChuck.SwapchainExtent.width;
			framebufferCreateInfo.height = inputChuck.SwapchainExtent.height;
			framebufferCreateInfo.layers = 1;

			try
			{
				frames[i].FrameBuffer = inputChuck.Device.createFramebuffer(framebufferCreateInfo);

				if (OE_DEBUG)
					OE_CORE_INFO("Created framebuffer for frame {0}", i);
			}
			catch (vk::SystemError error)
			{
				OE_CORE_ASSERT(false, "Failed to create framebuffer for frame {0}", i);
			}

		}

	}
#endif

	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	void VulkanFramebuffer::MakeFramebuffer(VulkanFramebufferInputSpecification inputChuck, std::vector<SwapChainFrame>& frames)
	{

		for (int i = 0; i < frames.size(); i++)
		{
			std::vector<vk::ImageView> attachments = {
				frames[i].ImageView
			};

			vk::FramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.flags = vk::FramebufferCreateFlags();
			framebufferCreateInfo.renderPass = inputChuck.RenderPass;
			framebufferCreateInfo.attachmentCount = attachments.size();
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = inputChuck.SwapchainExtent.width;
			framebufferCreateInfo.height = inputChuck.SwapchainExtent.height;
			framebufferCreateInfo.layers = 1;

			try
			{
				frames[i].FrameBuffer = inputChuck.Device.createFramebuffer(framebufferCreateInfo);

				OE_CORE_INFO("Created framebuffer for frame {0}", i);
			}
			catch (vk::SystemError error)
			{
				OE_CORE_ASSERT(false, "Failed to create framebuffer for frame {0}", i);
			}

		}

	}

	void VulkanFramebuffer::Invalidate()
	{
	}

	void VulkanFramebuffer::Bind()
	{
	}

	void VulkanFramebuffer::UnBind()
	{
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
	}

	int VulkanFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		return 0;
	}

	void VulkanFramebuffer::ClearColorAttachment(uint32_t index, int value)
	{
	}

}
