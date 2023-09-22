#pragma once

#include "OpenEngine/Renderer/RendererAPI.h"

namespace OpenEngine {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawInstanced(const Ref<VertexArray>& vertexArray, uint32_t indexCount, uint32_t instanceCount) override;
		virtual void SetLineWidth(float width) override;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
	};

}