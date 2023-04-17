#include "oepch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer2D.h"

namespace OpenEngine {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	struct CubeVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		int EntityID;
	};

	struct RendererData
	{
		static const uint32_t MaxCubes = 1000;
		static const uint32_t MaxCubeVerticies = MaxCubes * 24;
		static const uint32_t MaxCubeIndicies = MaxCubes * 36;

		Ref<VertexArray> CubeVertexArray;
		Ref<VertexBuffer> CubeVertexBuffer;
		Ref<Shader> Shader;

		uint32_t CubeIndexCount = 0;
		CubeVertex* CubeVertexBufferBase = nullptr;
		CubeVertex* CubeVertexBufferPtr = nullptr;

		glm::vec4 CubeVertexPositions[24] =
		{
			{ -0.5f, -0.5f, -0.5f, 1.0f }, // Front
			{  0.5f, -0.5f, -0.5f, 1.0f },
			{  0.5f,  0.5f, -0.5f, 1.0f },
			{ -0.5f,  0.5f, -0.5f, 1.0f },

			{ -0.5f, -0.5f, 0.5f, 1.0f }, // Back
			{  0.5f, -0.5f, 0.5f, 1.0f },
			{  0.5f,  0.5f, 0.5f, 1.0f },
			{ -0.5f,  0.5f, 0.5f, 1.0f },

			{ -0.5f, -0.5f,  0.5f, 1.0f }, // Left
			{ -0.5f, -0.5f, -0.5f, 1.0f },
			{ -0.5f,  0.5f, -0.5f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f },

			{ 0.5f, -0.5f,  0.5f, 1.0f }, // Right
			{ 0.5f, -0.5f, -0.5f, 1.0f },
			{ 0.5f,  0.5f, -0.5f, 1.0f },
			{ 0.5f,  0.5f, 0.0f, 1.0f },

			{ -0.5f, 0.5f, 0.0f, 1.0f }, // Top
			{  0.5f, 0.5f, 0.0f, 1.0f },
			{  0.5f, 0.5f, 0.0f, 1.0f },
			{ -0.5f, 0.5f, 0.0f, 1.0f },

			{ -0.5f, -0.5f, -0.5f, 1.0f }, // Bottom
			{  0.5f, -0.5f,  0.5f, 1.0f },
			{  0.5f, -0.5f,  0.5f, 1.0f },
			{ -0.5f, -0.5f, -0.5f, 1.0f }
		};
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		OE_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
#if 0
		s_Data.CubeVertexArray = VertexArray::Create();
		s_Data.CubeVertexBuffer = VertexBuffer::Create(s_Data.MaxCubeVerticies * sizeof(CubeVertex));
		s_Data.CubeVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Int, "a_EntityID" }
		});
		s_Data.CubeVertexArray->AddVertexBuffer(s_Data.CubeVertexBuffer);

		s_Data.CubeVertexBufferBase = new CubeVertex[s_Data.MaxCubeVerticies];

		uint32_t* CubeIndicies = new uint32_t[s_Data.MaxCubeIndicies];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxCubeIndicies; i += 6)
		{
			CubeIndicies[i + 0] = offset + 0;
			CubeIndicies[i + 1] = offset + 1;
			CubeIndicies[i + 2] = offset + 2;

			CubeIndicies[i + 3] = offset + 2;
			CubeIndicies[i + 4] = offset + 3;
			CubeIndicies[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> CubeIB = IndexBuffer::Create(CubeIndicies, s_Data.MaxCubeIndicies);
		s_Data.CubeVertexArray->SetIndexBuffer(CubeIB);
		delete[] CubeIndicies;

		s_Data.Shader = Shader::Create("assets/shaders/CubeShader.glsl");
		s_Data.Shader->Bind();
#endif
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		glm::mat4 viewProjection = camera.GetViewProjection();

		s_Data.Shader->Bind();
		s_Data.Shader->SetMat4("u_ViewProjection", viewProjection);

		StartRenderPass();
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::StartRenderPass()
	{
		s_Data.CubeIndexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;
	}

	void Renderer::Flush()
	{
		if (s_Data.CubeIndexCount == 0)
			return; // Nothing to draw

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CubeVertexBufferPtr - (uint8_t*)s_Data.CubeVertexBufferBase);
		s_Data.CubeVertexBuffer->SetData(s_Data.CubeVertexBufferBase, dataSize);
		
		RenderCommand::DrawIndexed(s_Data.CubeVertexArray, s_Data.CubeIndexCount);
		//s_Data.Stats.DrawCalls++;
	}

	void Renderer::DrawCube(const glm::mat4& transform)
	{
		for (size_t i = 0; i < 24; i++)
		{
			s_Data.CubeVertexBufferPtr->Position = transform * s_Data.CubeVertexPositions[i];
			s_Data.CubeVertexBufferPtr->Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			s_Data.CubeVertexBufferPtr->EntityID = 0;
			s_Data.CubeVertexBufferPtr++;
		}

		s_Data.CubeIndexCount += 36;

		//s_Data.Stats.CubeCount++;
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}