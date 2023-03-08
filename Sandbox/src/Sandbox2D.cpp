#include "Sandbox2D.h"
#include "OpenEngine/Renderer/Renderer2D.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	OE_PROFILE_FUNCTION();

	m_CheckerboardTexture = OpenEngine::Texture2D::Create("assets/textures/Checkerboard.png");
	m_Tilesheet = OpenEngine::Texture2D::Create("assets/textures/Tilesheet.png");
	m_GrassTopLeft = OpenEngine::SubTexture2D::CreateFromCoords(m_Tilesheet, { 0, 8 }, { 32, 32 });
	m_GrassTopCenter = OpenEngine::SubTexture2D::CreateFromCoords(m_Tilesheet, { 1, 8 }, { 32, 32 });
	m_GrassTopRight = OpenEngine::SubTexture2D::CreateFromCoords(m_Tilesheet, { 2, 8 }, { 32, 32 });
	m_GrassMiddle = OpenEngine::SubTexture2D::CreateFromCoords(m_Tilesheet, { 1, 7 }, { 32, 32 });
	m_GrassAndDirtRegion = OpenEngine::SubTexture2D::CreateFromCoords(m_Tilesheet, { 0, 6 }, { 32, 32 }, { 3, 3 });
	m_CameraController.SetZoomLevel(5.0f);
}

void Sandbox2D::OnDetach()
{
	OE_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(OpenEngine::Timestep ts)
{
	OE_PROFILE_FUNCTION();

	//Update
	m_CameraController.OnUpdate(ts);
	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		OpenEngine::Quad quad1 = { { 1.0f, -0.5f, 0.0f }, { 0.8f, 0.8f }, m_SquareColor };
		OpenEngine::Quad quad2 = { { -0.75f, 0.25f, 0.0f }, { 1.0f, 0.5f }, m_Square2Color, rotation, 1.0f };
		OpenEngine::Quad quad3;
		OpenEngine::Quad quad4;

		quad3.position = { 0.0f, 0.0f, -0.1f };
		quad3.size = { 20.0f, 20.0f };
		quad3.texture = m_CheckerboardTexture;
		quad3.scale = 10.0f;

		quad4.position = { 0.0f, 2.0f, 0.0f };
		quad4.size = { 1.0f, 1.0f };
		quad4.texture = m_CheckerboardTexture;
		quad4.rotation = 45.0f;
		quad4.scale = 20.0f;


		//Render

		OpenEngine::Renderer2D::ResetStats();

		OpenEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		OpenEngine::RenderCommand::Clear();

		OpenEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
		OpenEngine::Renderer2D::DrawQuad(quad3);

		OpenEngine::Renderer2D::DrawQuad(quad1);
		OpenEngine::Renderer2D::DrawQuad(quad2);
		OpenEngine::Renderer2D::DrawQuad(quad4);


		OpenEngine::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_GrassAndDirtRegion, 1.0f);

		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.8f };
				OpenEngine::Renderer2D::DrawQuad({ x, y, 0.1f }, { 0.45f, 0.45f }, color);
			}
		}
		OpenEngine::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	OE_PROFILE_FUNCTION();

    static bool dockingEnabled = true;
    if (dockingEnabled)
    {
        static bool dockspaceOpen = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.

                if (ImGui::MenuItem("Exit")) OpenEngine::Application::Get().Close();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }


        ImGui::Begin("Settings");

        auto stats = OpenEngine::Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quad Count: %d", stats.QuadCount);
        ImGui::Text("Verticies: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indicies: %d", stats.GetTotalIndexCount());

        ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::ColorEdit4("Square2 Color", glm::value_ptr(m_Square2Color));
        uint32_t textureID = m_CheckerboardTexture->GetRendererID();
        ImGui::Image((void*)textureID, ImVec2{ 256.0f, 256.0f });

        ImGui::End();

        ImGui::End();
    }
    else
    {
        ImGui::Begin("Settings");

        auto stats = OpenEngine::Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quad Count: %d", stats.QuadCount);
        ImGui::Text("Verticies: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indicies: %d", stats.GetTotalIndexCount());

        ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::ColorEdit4("Square2 Color", glm::value_ptr(m_Square2Color));
        uint32_t textureID = m_CheckerboardTexture->GetRendererID();
        ImGui::Image((void*)textureID, ImVec2{ 256.0f, 256.0f });

        ImGui::End();
    }
}

void Sandbox2D::OnEvent(OpenEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}
