#pragma once

#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Buffer.h"
#include "OpenEngine/Renderer/EditorCamera.h"
#include "OpenEngine/Renderer/Mesh.h"

namespace OpenEngine {

	class Entity;

	class Renderer
	{
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);
		
		static void BeginScene(const EditorCamera& camera, std::vector<Entity> lights);
		static void BeginScene(const Camera& camera, const glm::mat4& transform, std::vector<Entity> lights);
		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();
		static void EndScene(std::vector<Mesh>& meshes);

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		//static void Submit(Cube cube, int entityID = -1);
		static void Submit(Mesh& mesh, const glm::mat4& transform, Material material, int entityID = -1);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t Meshes = 0;
		};
		static void ResetStats();
		static Statistics GetStatistics();
	};

}