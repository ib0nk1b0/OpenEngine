#pragma once

#include "OpenEngine/Renderer/Camera.h"
#include "OpenEngine/Renderer/OrthographicCamera.h"
#include "OpenEngine/Renderer/Texture.h"
#include "OpenEngine/Renderer/SubTexture2D.h"
#include "OpenEngine/Renderer/EditorCamera.h"

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

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthographicCamera& camera);
		static void StartBatch();
		static void EndScene();

		static void Flush();

		// Primitives
		static void DrawQuad(Quad quad);

		// Transforms
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float scale = 1.0f);
		static void DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subtexture, float scale = 1.0f);

		//Quads
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float scale = 1.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float scale = 1.0f);

		// Rotated Quads
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float scale = 1.0f);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float scale = 1.0f);
	
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};
		static void ResetStats();
		static Statistics GetStats();
	private:
		static void FlushAndReset();
	};

}
