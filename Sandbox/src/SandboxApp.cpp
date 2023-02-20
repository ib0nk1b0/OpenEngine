#include <OpenEngine.h>
#include <glm/glm.hpp>

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>

class ExampleLayer : public OpenEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_CameraRotation(0.0f), m_TrianglePosition(0.0f)
	{
		m_VertexArray.reset(OpenEngine::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.9f, 0.3f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.2f, 0.9f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.3f, 1.0f
		};

		std::shared_ptr<OpenEngine::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(OpenEngine::VertexBuffer::Create(vertices, sizeof(vertices)));
		OpenEngine::BufferLayout layout = {
			{ OpenEngine::ShaderDataType::Float3, "a_Position" },
			{ OpenEngine::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<OpenEngine::IndexBuffer> indexBuffer;
		indexBuffer.reset(OpenEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(OpenEngine::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		std::shared_ptr<OpenEngine::VertexBuffer> squareVB;
		squareVB.reset(OpenEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ OpenEngine::ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<OpenEngine::IndexBuffer> squareIB;
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

		m_Shader.reset(OpenEngine::Shader::Create(vertexSrc, fragmentSrc));

		////////////////////////////////////////////////////////////////////////////////////////

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec4 u_Color;

			void main()
			{
				color = u_Color;
			}
		)";

		m_FlatColorShader.reset(OpenEngine::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));
	}

	void OnUpdate(OpenEngine::Timestep ts, int& fps) override
	{
		m_FramesPerSecond = fps;

		// Camera movement
		if (OpenEngine::Input::IsKeyPressed(OE_KEY_W))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		if (OpenEngine::Input::IsKeyPressed(OE_KEY_S))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		if (OpenEngine::Input::IsKeyPressed(OE_KEY_A))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		if (OpenEngine::Input::IsKeyPressed(OE_KEY_D))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;

		if (OpenEngine::Input::IsKeyPressed(OE_KEY_LEFT))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		if (OpenEngine::Input::IsKeyPressed(OE_KEY_RIGHT))
			m_CameraRotation -= m_CameraRotationSpeed * ts;

		// Triangle movement
		if (OpenEngine::Input::IsKeyPressed(OE_KEY_I))
			m_TrianglePosition.y += m_TriangleMoveSpeed * ts;
		if (OpenEngine::Input::IsKeyPressed(OE_KEY_K))
			m_TrianglePosition.y -= m_TriangleMoveSpeed * ts;

		if (OpenEngine::Input::IsKeyPressed(OE_KEY_J))
			m_TrianglePosition.x -= m_TriangleMoveSpeed * ts;
		if (OpenEngine::Input::IsKeyPressed(OE_KEY_L))
			m_TrianglePosition.x += m_TriangleMoveSpeed * ts;

		OpenEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		OpenEngine::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		OpenEngine::Renderer::BeginScene(m_Camera);

		glm::mat4 triangleTransform = glm::translate(glm::mat4(1.0f), m_TrianglePosition);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		
		glm::vec4 redColor(0.8f, 0.2f, 0.3f, 1.0f);
		glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				if (y % 2 == 0 && x % 2 == 0)
					m_FlatColorShader->UploadUniformFloat4("u_Color", redColor);
				else if (y % 2 == 0 && x % 2 == 1)
					m_FlatColorShader->UploadUniformFloat4("u_Color", blueColor);
				else if (y % 2 == 1 && x % 2 == 0)
					m_FlatColorShader->UploadUniformFloat4("u_Color", blueColor);
				else if (y % 2 == 1 && x % 2 == 1)
					m_FlatColorShader->UploadUniformFloat4("u_Color", redColor);
				OpenEngine::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		OpenEngine::Renderer::Submit(m_Shader, m_VertexArray, triangleTransform);

		OpenEngine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Dev Tools");
		ImGui::Text("Fps: %i", m_FramesPerSecond);
		ImGui::End();
	}

	void OnEvent(OpenEngine::Event& event) override
	{
	}
private:
	std::shared_ptr<OpenEngine::Shader> m_Shader;
	std::shared_ptr<OpenEngine::Shader> m_FlatColorShader;

	std::shared_ptr<OpenEngine::VertexArray> m_VertexArray;
	std::shared_ptr<OpenEngine::VertexArray> m_SquareVA;

	OpenEngine::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 2.0f;

	float m_CameraRotation;
	float m_CameraRotationSpeed = 90.0f;

	glm::vec3 m_TrianglePosition;
	float m_TriangleMoveSpeed = 1.0f;

	int m_FramesPerSecond = 0;
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