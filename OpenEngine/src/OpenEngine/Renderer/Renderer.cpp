#include "oepch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer2D.h"
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
		static const uint32_t MaxCubes = 10;
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
	};

	static SceneData s_SceneData;

	void Renderer::Init()
	{
		OE_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();

		s_SceneData.CubeVertexArray = VertexArray::Create();

		s_SceneData.CubeVertexBuffer = VertexBuffer::Create(s_SceneData.MaxCubeVerticies * sizeof(CubeVertex));
		s_SceneData.CubeVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityID" }
		});

		s_SceneData.CubeVertexArray->AddVertexBuffer(s_SceneData.CubeVertexBuffer);

		s_SceneData.CubeVertexBufferBase = new CubeVertex[s_SceneData.MaxCubeVerticies];

		uint32_t* cubeIndicies = new uint32_t[s_SceneData.MaxCubeIndicies];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_SceneData.MaxCubeIndicies; i += 36)
		{
			// front face
			cubeIndicies[i + 0] = offset + 0;
			cubeIndicies[i + 1] = offset + 1;
			cubeIndicies[i + 2] = offset + 2;

			cubeIndicies[i + 3] = offset + 2;
			cubeIndicies[i + 4] = offset + 3;
			cubeIndicies[i + 5] = offset + 0;

			//right face
			cubeIndicies[i + 6] = offset + 3;
			cubeIndicies[i + 7] = offset + 2;
			cubeIndicies[i + 8] = offset + 6;

			cubeIndicies[i + 9] = offset + 6;
			cubeIndicies[i + 10] = offset + 7;
			cubeIndicies[i + 11] = offset + 3;

			//back face
			cubeIndicies[i + 12] = offset + 7;
			cubeIndicies[i + 13] = offset + 6;
			cubeIndicies[i + 14] = offset + 5;

			cubeIndicies[i + 15] = offset + 5;
			cubeIndicies[i + 16] = offset + 4;
			cubeIndicies[i + 17] = offset + 7;
			
			//left face
			cubeIndicies[i + 18] = offset + 4;
			cubeIndicies[i + 19] = offset + 5;
			cubeIndicies[i + 20] = offset + 1;

			cubeIndicies[i + 21] = offset + 1;
			cubeIndicies[i + 22] = offset + 0;
			cubeIndicies[i + 23] = offset + 4;

			//top face
			cubeIndicies[i + 24] = offset + 1;
			cubeIndicies[i + 25] = offset + 5;
			cubeIndicies[i + 26] = offset + 6;

			cubeIndicies[i + 27] = offset + 6;
			cubeIndicies[i + 28] = offset + 2;
			cubeIndicies[i + 29] = offset + 1;

			//bottom face
			cubeIndicies[i + 30] = offset + 0;
			cubeIndicies[i + 31] = offset + 4;
			cubeIndicies[i + 32] = offset + 7;

			cubeIndicies[i + 33] = offset + 7;
			cubeIndicies[i + 34] = offset + 3;
			cubeIndicies[i + 35] = offset + 0;

			offset += 8;
		}

		Ref<IndexBuffer> cubeIB = IndexBuffer::Create(cubeIndicies, s_SceneData.MaxCubeIndicies);
		s_SceneData.CubeVertexArray->SetIndexBuffer(cubeIB);
		delete[] cubeIndicies;

		s_SceneData.CubeVertexPositions[0] = { -0.5f, -0.5f, -0.5f, 1.0f };
		s_SceneData.CubeVertexPositions[1] = { 0.5f, -0.5f, -0.5f, 1.0f };
		s_SceneData.CubeVertexPositions[2] = { 0.5f,  0.5f, -0.5f, 1.0f };
		s_SceneData.CubeVertexPositions[3] = { -0.5f,  0.5f, -0.5f, 1.0f };

		s_SceneData.CubeVertexPositions[4] = { -0.5f, -0.5f, 0.5f, 1.0f };
		s_SceneData.CubeVertexPositions[5] = { 0.5f, -0.5f, 0.5f, 1.0f };
		s_SceneData.CubeVertexPositions[6] = { 0.5f,  0.5f, 0.5f, 1.0f };
		s_SceneData.CubeVertexPositions[7] = { -0.5f,  0.5f, 0.5f, 1.0f };

		s_SceneData.CubeShader = Shader::Create("assets/shaders/CubeShader.glsl");

		s_SceneData.CubeVertexBufferPtr = s_SceneData.CubeVertexBufferBase;
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		s_SceneData.ViewProjectionMatrix = camera.GetViewProjection();
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_SceneData.ViewProjectionMatrix = camera.GetProjection() * glm::inverse(transform);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
		s_SceneData.CubeIndexCount = 0;
		s_SceneData.CubeVertexBufferPtr = s_SceneData.CubeVertexBufferBase;
	}

	void Renderer::DrawCube(const glm::mat4& transform)
	{
		constexpr size_t cubeVertexCount = 8;

		for (size_t i = 0; i < cubeVertexCount; i++)
		{
			s_SceneData.CubeVertexBufferPtr->Position = transform * s_SceneData.CubeVertexPositions[i];
			s_SceneData.CubeVertexBufferPtr->Color = { 0.2f, 0.3f, 1.0f, 1.0f, };
			s_SceneData.CubeVertexBufferPtr->EntityID = -1;
			s_SceneData.CubeVertexBufferPtr++;
		}

		s_SceneData.CubeIndexCount += 36;

		Submit(s_SceneData.CubeShader, s_SceneData.CubeVertexArray, transform);
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_SceneData.ViewProjectionMatrix);
		
		uint32_t dataSize = (uint32_t)((uint8_t*)s_SceneData.CubeVertexBufferPtr - (uint8_t*)s_SceneData.CubeVertexBufferBase);
		s_SceneData.CubeVertexBuffer->SetData(s_SceneData.CubeVertexBufferBase, dataSize);

		RenderCommand::DrawIndexed(vertexArray, s_SceneData.CubeIndexCount);
	}

	void Renderer::Submit(Cube cube, int entityID)
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

	}

}