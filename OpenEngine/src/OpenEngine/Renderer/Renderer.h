#pragma once

#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Buffer.h"
#include "OpenEngine/Renderer/EditorCamera.h"

namespace OpenEngine {

	class Renderer
	{
	public:
		struct Material
		{
			glm::vec3 Albedo{ 1.0f };
			float Roughness = 1.0f;
			float Metalic = 0.0f;
		};

		struct Cube
		{
			glm::mat4 Transform{ 1.0f };

			Material Mat;
		};

		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);
		
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void DrawCube(const glm::mat4& transform);
		static void LoadModel();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(Cube cube, int entityID = -1);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};

}