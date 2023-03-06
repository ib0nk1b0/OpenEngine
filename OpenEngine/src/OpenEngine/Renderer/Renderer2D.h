#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"

namespace OpenEngine
{

	struct Quad
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec2 size = { 1.0f, 1.0f };
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float rotation = 0.0f;
		float scale = 1.0f;
		Ref<Texture2D> texture;
	};

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void Flush();

		// Primitives
		static void DrawQuad(Quad quad);

		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float scale = 1.0f);

		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float scale = 1.0f);
	};

}
