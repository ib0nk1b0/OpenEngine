#pragma once

#include <vulkan/vulkan.hpp>
//#include <C:/VulkanSDK/1.3.243.0/Include/vulkan/vulkan.hpp>

namespace OpenEngine {

	struct GraphicsPipelineInBundle
	{
		std::string VertexPath;
		std::string FragmentPath;
		vk::Device Device;
		vk::Extent2D SwapchainExtent;
		vk::Format SwapchainImageFormat;
	};

	struct GraphicsPipelineOutBundle
	{
		vk::PipelineLayout Layout;
		vk::RenderPass RenderPass;
		vk::Pipeline Pipeline;
	};

	class VulkanGraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline();
		virtual ~VulkanGraphicsPipeline() = default;

		vk::PipelineLayout CreatePipelineLayout(vk::Device device);
		vk::RenderPass CreateRenderPass(vk::Device device, vk::Format swapchainImageFormat);
		GraphicsPipelineOutBundle CreateGraphicsPipeline(GraphicsPipelineInBundle specification);
	};

}