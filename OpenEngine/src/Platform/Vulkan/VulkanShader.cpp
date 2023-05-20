#include "oepch.h"
#include "VulkanShader.h"

#include "VulkanGraphicsPipeline.h"

#include <fstream>

#include <glm/gtc/type_ptr.hpp>

namespace OpenEngine {

	VulkanShader::VulkanShader(const std::string& filepath)
	{
	}

	VulkanShader::VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
	}

	VulkanShader::~VulkanShader()
	{
	}

	std::vector<char> VulkanShader::ReadFile(const std::string& filepath)
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		OE_CORE_ASSERT(file.is_open(), "Failed to open the file!");

		size_t filesize{ static_cast<size_t>(file.tellg()) };
		std::vector<char> result(filesize);
		file.seekg(0);
		file.read(result.data(), filesize);
		file.close();

		return result;
	}

	vk::ShaderModule VulkanShader::CreateModule(const std::string& filepath, vk::Device device)
	{
		std::vector<char> sourceCode = ReadFile(filepath);

		vk::ShaderModuleCreateInfo moduleInfo = {};
		moduleInfo.flags = vk::ShaderModuleCreateFlags();
		moduleInfo.codeSize = sourceCode.size();
		moduleInfo.pCode = reinterpret_cast<const uint32_t*>(sourceCode.data());

		try
		{
			return device.createShaderModule(moduleInfo);
		}
		catch (vk::SystemError error)
		{
			OE_CORE_ASSERT(false, "Failed to create shader module!");
		}
	}

#if 0
	std::unordered_map<GLenum, std::string> VulkanShader::PreProcess(const std::string& source)
	{
	}
#endif

	void VulkanShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
	}

	void VulkanShader::Bind() const
	{
	}

	void VulkanShader::UnBind() const
	{
	}

	void VulkanShader::SetInt(const std::string& name, int value)
	{
	}

	void VulkanShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
	}

	void VulkanShader::SetFloat(const std::string& name, int value)
	{
	}

	void VulkanShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
	}

	void VulkanShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
	}

	void VulkanShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
	}

	void VulkanShader::UploadUniformInt(const std::string& name, int value)
	{
	}

	void VulkanShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
	}

	void VulkanShader::UploadUniformFloat(const std::string& name, float value)
	{
	}

	void VulkanShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
	}

	void VulkanShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
	}

	void VulkanShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
	}

	void VulkanShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
	}

	void VulkanShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
	}

}