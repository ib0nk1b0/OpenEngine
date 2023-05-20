#pragma once

#include "OpenEngine/Renderer/Framebuffer.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace OpenEngine {

	struct VulkanFramebufferInputSpecification
	{
		vk::Device Device;
		vk::RenderPass RenderPass;
		vk::Extent2D SwapchainExtent;
	};

	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		//VulkanFramebuffer(VulkanFramebufferInputSpecification inputChunk, std::vector<SwapChainFrame>& frames);
		virtual ~VulkanFramebuffer();

		static void MakeFramebuffer(VulkanFramebufferInputSpecification inputChunk, std::vector<SwapChainFrame>& frames);

		void Invalidate();
		
		virtual void Bind() override;
		virtual void UnBind() override;
		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		virtual void ClearColorAttachment(uint32_t index, int value) override;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { OE_CORE_ASSERT(index < m_ColorAttachments.size(), ""); return m_ColorAttachments[index]; }

		virtual const FramebufferSpecification& GetSpecification() const override { /*return m_Specification;*/ return FramebufferSpecification(); }

	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		//std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		//FramebufferTextureSpecification m_DepthAttachmentSpecifications = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		//uint32_t m_DepthAttachment;
	};

}