#include <OpenEngine.h>
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class ExampleLayer : public OpenEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
		m_VertexArray.reset(OpenEngine::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.9f, 0.3f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.2f, 0.9f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.3f, 1.0f
		};

		OpenEngine::Ref<OpenEngine::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(OpenEngine::VertexBuffer::Create(vertices, sizeof(vertices)));
		OpenEngine::BufferLayout layout = {
			{ OpenEngine::ShaderDataType::Float3, "a_Position" },
			{ OpenEngine::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		OpenEngine::Ref<OpenEngine::IndexBuffer> indexBuffer;
		indexBuffer.reset(OpenEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(OpenEngine::VertexArray::Create());

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		OpenEngine::Ref<OpenEngine::VertexBuffer> squareVB;
		squareVB.reset(OpenEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ OpenEngine::ShaderDataType::Float3, "a_Position" },
			{ OpenEngine::ShaderDataType::Float2, "a_TexCoord" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		OpenEngine::Ref<OpenEngine::IndexBuffer> squareIB;
		squareIB.reset(OpenEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader = OpenEngine::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

		m_ShaderLibrary.Load("assets/shaders/FlatColourShader.glsl");
		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = OpenEngine::Texture2D::Create("assets/textures/Chekerboard.png");
		m_OpenEngineLogoTexture = OpenEngine::Texture2D::Create("assets/textures/OpenEngineLogo.png");

		std::dynamic_pointer_cast<OpenEngine::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<OpenEngine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

	}

	void OnUpdate(OpenEngine::Timestep ts) override
	{
		//Update
		m_CameraController.OnUpdate(ts);

		//Render
		OpenEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		OpenEngine::RenderCommand::Clear();

		OpenEngine::Renderer::BeginScene(m_CameraController.GetCamera());

		//glm::mat4 triangleTransform = glm::translate(glm::mat4(1.0f), m_TrianglePosition);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		
		auto flatColorShader = m_ShaderLibrary.Get("FlatColourShader");
		std::dynamic_pointer_cast<OpenEngine::OpenGLShader>(flatColorShader)->Bind();
		std::dynamic_pointer_cast<OpenEngine::OpenGLShader>(flatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				OpenEngine::Renderer::Submit(flatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		OpenEngine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_OpenEngineLogoTexture->Bind();
		OpenEngine::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		// Triangle draw call
		// OpenEngine::Renderer::Submit(m_Shader, m_VertexArray, triangleTransform);

		OpenEngine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		//ImGui::Text("Fps: %i", m_FramesPerSecond);
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		//ImGui::SliderFloat3("Camera Position", m_ImGuiCameraPosition, -5.0f, 5.0f);
		ImGui::End();
	}

	void OnEvent(OpenEngine::Event& e) override
	{
		m_CameraController.OnEvent(e);
		if (e.GetEventType() == OpenEngine::EventType::WindowResize)
		{
			auto& re = (OpenEngine::WindowResizeEvent&)e;

			//float zoom = re.GetWidth() / 1280.0f;
			//m_CameraController.SetZoomLevel(zoom);
		}
	}
private:
	OpenEngine::ShaderLibrary m_ShaderLibrary;
	OpenEngine::Ref<OpenEngine::Shader> m_Shader;

	OpenEngine::Ref<OpenEngine::VertexArray> m_VertexArray;
	OpenEngine::Ref<OpenEngine::VertexArray> m_SquareVA;

	OpenEngine::Ref<OpenEngine::Texture2D> m_Texture, m_OpenEngineLogoTexture;

	OpenEngine::OrthographicCameraController m_CameraController;

	//glm::vec3 m_TrianglePosition;
	float m_TriangleMoveSpeed = 1.0f;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };

	//int m_FramesPerSecond = 0;
	//float m_ImGuiCameraPosition[3] = { 0.0f, 0.0f, 0.0f };
};

class Sandbox : public OpenEngine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
	~Sandbox()
	{
	}
};

OpenEngine::Application* OpenEngine::CreateApplication()
{
	return new Sandbox();
}