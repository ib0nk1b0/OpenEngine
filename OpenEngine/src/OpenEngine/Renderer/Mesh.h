#pragma once

#include "OpenEngine/Renderer/Buffer.h"

namespace OpenEngine {

	class Mesh
	{
	public:
		Mesh();
		Mesh(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer);
		~Mesh();

		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) { m_VertexBuffer = vertexBuffer; }
		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }

		void AddIndexBuffer(const Ref<IndexBuffer>& indexBuffer) { m_IndexBuffer = indexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
	private:
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};

}