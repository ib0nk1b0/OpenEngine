#include "oepch.h"
#include "VulkanGraphicsPipeline.h"

#include "Platform/Vulkan/VulkanShader.h"

#include <glm/glm.hpp>

namespace OpenEngine {

	struct PushConstantData
	{
		glm::vec2 Offset;
		alignas(16) glm::vec4 Color;
	};

	VulkanGraphicsPipeline::VulkanGraphicsPipeline()
	{

	}

	vk::PipelineLayout VulkanGraphicsPipeline::CreatePipelineLayout(vk::Device device)
	{
		vk::PushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstantData);

		vk::PipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.flags = vk::PipelineLayoutCreateFlags();
		layoutInfo.setLayoutCount = 0;
		layoutInfo.pushConstantRangeCount = 1;
		layoutInfo.pPushConstantRanges = &pushConstantRange;

		try
		{
			return device.createPipelineLayout(layoutInfo);
		}
		catch (vk::SystemError error)
		{
			OE_CORE_ASSERT(false, "Unable to creat the pipline layout!");
		}
	}

	vk::RenderPass VulkanGraphicsPipeline::CreateRenderPass(vk::Device device, vk::Format swapchainImageFormat)
	{
		vk::AttachmentDescription colorAttachment = {};
		colorAttachment.flags = vk::AttachmentDescriptionFlags();
		colorAttachment.format = swapchainImageFormat;
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass = {};
		subpass.flags = vk::SubpassDescriptionFlags();
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		vk::RenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.flags = vk::RenderPassCreateFlags();
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		try
		{
			return device.createRenderPass(renderPassInfo);
		}
		catch (vk::SystemError error)
		{
			OE_CORE_ASSERT(false, "Unable to create render pass!");
		}
	}

	GraphicsPipelineOutBundle VulkanGraphicsPipeline::CreateGraphicsPipeline(GraphicsPipelineInBundle specification)
	{
		vk::GraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.flags = vk::PipelineCreateFlags();

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

		// Vertex input
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		pipelineInfo.pVertexInputState = &vertexInputInfo;

		// Input assembly
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
		inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
		inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

		// Vertex shader
		Ref<VulkanShader> shader = CreateRef<VulkanShader>("shader", specification.VertexPath, specification.FragmentPath);
		auto [vertexShaderModule, fragmentShaderModule] = shader->CreateModules(specification.Device);

		vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
		vertexShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
		vertexShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertexShaderInfo.module = vertexShaderModule;
		vertexShaderInfo.pName = "main";
		shaderStages.push_back(vertexShaderInfo);
		
		// Viewport and Scissor
		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = specification.SwapchainExtent.width;
		viewport.height = specification.SwapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vk::Rect2D sissor = {};
		sissor.offset.x = 0.0f;
		sissor.offset.y = 0.0f;
		sissor.extent = specification.SwapchainExtent;

		vk::PipelineViewportStateCreateInfo viewportState = {};
		viewportState.flags = vk::PipelineViewportStateCreateFlags();
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &sissor;
		pipelineInfo.pViewportState = &viewportState;

		// Rasterizer
		vk::PipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise;
		rasterizer.depthBiasEnable = VK_FALSE;
		pipelineInfo.pRasterizationState = &rasterizer;

		// Fragment shader
		vk::PipelineShaderStageCreateInfo fragmentShaderInfo = {};
		fragmentShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
		fragmentShaderInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragmentShaderInfo.module = fragmentShaderModule;
		fragmentShaderInfo.pName = "main";
		shaderStages.push_back(fragmentShaderInfo);

		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();

		// Multisampling
		vk::PipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		pipelineInfo.pMultisampleState = &multisampling;
		
		// Color blend
		vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_FALSE;

		vk::PipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.flags = vk::PipelineColorBlendStateCreateFlags();
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;
		pipelineInfo.pColorBlendState = &colorBlending;

		// Pipeline layout

		vk::PipelineLayout layout = CreatePipelineLayout(specification.Device);
		pipelineInfo.layout = layout;

		// Renderpass 

		vk::RenderPass renderpass = CreateRenderPass(specification.Device, specification.SwapchainImageFormat);
		pipelineInfo.renderPass = renderpass;

		pipelineInfo.basePipelineHandle = nullptr;

		vk::Pipeline graphicsPipeline;
		try
		{
			graphicsPipeline = specification.Device.createGraphicsPipeline(nullptr, pipelineInfo).value;
		}
		catch (vk::SystemError error)
		{
			OE_CORE_ASSERT(false, "Failed to make the Graphics pipeline!");
		}

		GraphicsPipelineOutBundle output = {};
		output.Layout = layout;
		output.RenderPass = renderpass;
		output.Pipeline = graphicsPipeline;

		specification.Device.destroyShaderModule(vertexShaderModule);
		specification.Device.destroyShaderModule(fragmentShaderModule);
		
		return output;
	}

}