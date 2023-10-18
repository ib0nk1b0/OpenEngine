#include "oepch.h"
#include "Renderer.h"

#include "Renderer2D.h"
#include "OpenEngine/Scene/Entity.h"
#include "OpenEngine/Core/Application.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "glad/glad.h"

namespace OpenEngine {

	struct CubeVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		int EntityID;
	};

	struct SceneData
	{
		static const uint32_t MaxCubes = 1000;
		static const uint32_t MaxCubeVerticies = MaxCubes * 8;
		static const uint32_t MaxCubeIndicies = MaxCubes * 36;

		Ref<VertexArray> CubeVertexArray;
		Ref<VertexBuffer> CubeVertexBuffer;
		Ref<Shader> CubeShader;

		uint32_t CubeIndexCount = 0;
		CubeVertex* CubeVertexBufferBase = nullptr;
		CubeVertex* CubeVertexBufferPtr = nullptr;

		glm::vec4 CubeVertexPositions[8] =
		{
			{ 0.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 0.0f },

			{ 0.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 0.0f }
		};

		glm::mat4 ViewProjectionMatrix{ 1.0f };

		Renderer::Statistics Stats;
	};

	static SceneData s_SceneData;

	void Renderer::Init()
	{
		OE_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();

		s_SceneData.CubeShader = Shader::Create("assets/shaders/PBR_Shader.glsl");

		s_SceneData.CubeVertexBufferPtr = s_SceneData.CubeVertexBufferBase;
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const EditorCamera& camera, std::vector<Entity> lights)
	{
		OE_PROFILE_FUNCTION();

		s_SceneData.CubeShader->Bind();
		s_SceneData.CubeShader->SetMat4("u_ViewProjection", camera.GetViewProjection());
		auto& cameraPosition = camera.GetViewProjection()[0];
		s_SceneData.CubeShader->SetFloat3("u_CameraPosition", glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z));

		glm::vec3 lightPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 lightColor = { 0.0f, 0.0f, 0.0f };
		float ambientIntensity = 0.0f;

		if (!lights.empty())
		{
			auto& tc = lights[0].GetComponent<TransformComponent>();
			auto& pl = lights[0].GetComponent<PointLightComponent>();

			lightPosition = tc.Translation;
			lightColor = pl.Color;
			ambientIntensity = pl.AmbientIntensity;
		}

		s_SceneData.CubeShader->SetFloat3("u_LightPosition", lightPosition);
		s_SceneData.CubeShader->SetFloat3("u_LightColor", lightColor);
		s_SceneData.CubeShader->SetFloat("u_AmbientIntensity", ambientIntensity);
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform, std::vector<Entity> lights)
	{
		OE_PROFILE_FUNCTION();

		s_SceneData.CubeShader->Bind();
		s_SceneData.CubeShader->SetMat4("u_ViewProjection", camera.GetProjection() * glm::inverse(transform));
		auto& cameraPosition = camera.GetProjection()[0];
		s_SceneData.CubeShader->SetFloat3("u_CameraPosition", glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z));

		glm::vec3 lightPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 lightColor = { 0.0f, 0.0f, 0.0f };
		float ambientIntensity = 0.0f;

		if (!lights.empty())
		{
			auto& tc = lights[0].GetComponent<TransformComponent>();
			auto& pl = lights[0].GetComponent<PointLightComponent>();

			lightPosition = tc.Translation;
			lightColor = pl.Color;
			ambientIntensity = pl.AmbientIntensity;
		}

		s_SceneData.CubeShader->SetFloat3("u_LightPosition", lightPosition);
		s_SceneData.CubeShader->SetFloat3("u_LightColor", lightColor);
		s_SceneData.CubeShader->SetFloat("u_AmbientIntensity", ambientIntensity);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
	}

	void Renderer::EndScene()
	{
		s_SceneData.CubeIndexCount = 0;
		s_SceneData.CubeVertexBufferPtr = s_SceneData.CubeVertexBufferBase;
	}

	void Renderer::EndScene(std::vector<Mesh>& meshes, bool wireframe)
	{
		OE_PROFILE_FUNCTION();
		OE_PERF_FUNC();

		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		for (auto& mesh : meshes)
		{
			if (!mesh.HasMeshes())
				continue;

			mesh.Flush();
			RenderCommand::DrawInstanced(mesh.GetVertexArray(), mesh.GetIndexCount(), mesh.InstanceCount());
			mesh.ResetData();
			
			s_SceneData.Stats.DrawCalls++;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_SceneData.ViewProjectionMatrix);
		
		uint32_t dataSize = (uint32_t)((uint8_t*)s_SceneData.CubeVertexBufferPtr - (uint8_t*)s_SceneData.CubeVertexBufferBase);
		s_SceneData.CubeVertexBuffer->SetData(s_SceneData.CubeVertexBufferBase, dataSize);

		RenderCommand::DrawIndexed(vertexArray, s_SceneData.CubeIndexCount);
	}

	/*void Renderer::Submit(Cube cube, int entityID)
	{
		s_SceneData.CubeShader->Bind();
		s_SceneData.CubeShader->SetMat4("u_ViewProjection", s_SceneData.ViewProjectionMatrix);

		for (size_t i = 0; i < 8; i++)
		{
			s_SceneData.CubeVertexBufferPtr->Position = cube.Transform * s_SceneData.CubeVertexPositions[i];
			s_SceneData.CubeVertexBufferPtr->Color = { cube.Mat.Albedo.x, cube.Mat.Albedo.y, cube.Mat.Albedo.z, 1.0f };
			s_SceneData.CubeVertexBufferPtr->EntityID = entityID;
			s_SceneData.CubeVertexBufferPtr++;
		}

		s_SceneData.CubeIndexCount += 36;

		uint32_t dataSize = (uint32_t)((uint8_t*)s_SceneData.CubeVertexBufferPtr - (uint8_t*)s_SceneData.CubeVertexBufferBase);
		s_SceneData.CubeVertexBuffer->SetData(s_SceneData.CubeVertexBufferBase, dataSize);

		RenderCommand::DrawIndexed(s_SceneData.CubeVertexArray, s_SceneData.CubeIndexCount);

	}*/

	/*void Renderer::Submit(Cube cube, const glm::mat4& transform, Camera* camera, int entityID)
	{
		s_SceneData.CubeShader->Bind();
		s_SceneData.CubeShader->SetMat4("u_ViewProjection", s_SceneData.ViewProjectionMatrix);
		auto cameraPosition = camera->GetProjection()[0];
		s_SceneData.CubeShader->SetFloat3("u_CameraPosition", glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z));
		s_SceneData.CubeShader->SetMat4("u_Model", transform);
		cube.SetData(transform, entityID);

		RenderCommand::DrawIndexed(cube.GetVertexArray());
	}*/


	void Renderer::Submit(Mesh& mesh, const glm::mat4& transform, Material material, int entityID)
	{
		mesh.SetData(transform, material, entityID);
		s_SceneData.Stats.Meshes++;
	}

	void Renderer::ResetStats()
	{
		memset(&s_SceneData.Stats, 0, sizeof(Statistics));
	}

	Renderer::Statistics Renderer::GetStatistics()
	{
		return s_SceneData.Stats;
	}

}