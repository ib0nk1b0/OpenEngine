#include "oepch.h"
#include "VulkanVertexArray.h"

namespace OpenEngine {
	
	VulkanVertexArray::VulkanVertexArray()
	{
	}

	VulkanVertexArray::~VulkanVertexArray()
	{
	}

	void VulkanVertexArray::Bind() const
	{
	}

	void VulkanVertexArray::UnBind() const
	{
	}

	void VulkanVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		vertexBuffer->Bind();
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VulkanVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		indexBuffer->Bind();
		m_IndexBuffer = indexBuffer;
	}

}