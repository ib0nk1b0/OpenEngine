#include "EditorLayer.h"
#include "OpenEngine/Renderer/Renderer2D.h"
#include "OpenEngine/Scene/Components.h"

#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include <EnTT/include/entt.hpp>

namespace OpenEngine {

    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
    {
    }

    void EditorLayer::OnAttach()
    {
        OE_PROFILE_FUNCTION();

        m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
        m_OpenEngineTexture = Texture2D::Create("assets/textures/OpenEngineLogo.png");

        FramebufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_ActiveScene = CreateRef<Scene>();

        Entity mainCamera = m_ActiveScene->CreateEntity("Main Camera");
        mainCamera.AddComponent<CameraComponent>();
        m_MainCamera = mainCamera;

        Entity secondCamera = m_ActiveScene->CreateEntity("Second Camera");
        secondCamera.AddComponent<CameraComponent>().Primary = false;
        m_SecondCamera = secondCamera;

        Entity square = m_ActiveScene->CreateEntity("OpenEngine Logo");
        square.AddComponent<SpriteRendererComponent>(m_OpenEngineTexture);
        m_SquareEntity = square;

        Entity square2 = m_ActiveScene->CreateEntity("White Square");
        //square2.AddComponent<SpriteRendererComponent>();
        auto& transformComponent = square2.GetComponent<TransformComponent>().Transform;
        transformComponent = glm::mat4(0.5f);


       /* class CameraController : public ScriptableEntity
        {
        public:
            void OnCreate()
            {
            }

            void OnDestroy()
            {
            }

            void OnUpdate(Timestep ts)
            {
            }
        };
        secondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();*/
    }

    void EditorLayer::OnDetach()
    {
        OE_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        OE_PROFILE_FUNCTION();

        if (m_ViewportFocused)
            m_CameraController.OnUpdate(ts);

        Renderer2D::ResetStats();

        m_Framebuffer->Bind();

        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        RenderCommand::Clear();
  
        m_ActiveScene->OnUpdate(ts);

        m_Framebuffer->UnBind();
    }

    void EditorLayer::OnImGuiRender()
    {
        OE_PROFILE_FUNCTION();

        static bool dockspaceOpen = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

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
                if (ImGui::MenuItem("Exit")) Application::Get().Close();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::Begin("Settings");

        auto stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quad Count: %d", stats.QuadCount);
        ImGui::Text("Verticies: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indicies: %d", stats.GetTotalIndexCount());

        if (m_SquareEntity)
        {
            ImGui::Separator();
            auto& tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
            ImGui::Text("%s", tag.c_str());

            auto& squareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
            ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));
            ImGui::Separator();
        }

        if (ImGui::Checkbox("Main Camera", &m_MainCameraPrimary))
        {
            m_MainCamera.GetComponent<CameraComponent>().Primary = m_MainCameraPrimary;
            m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_MainCameraPrimary;
        }
        {
            auto& camera = m_SecondCamera.GetComponent<CameraComponent>().Camera;
            float orthoSize = camera.GetOrthographicSize();
            if (ImGui::DragFloat("Second Camera", &orthoSize))
                camera.SetOrthographicSize(orthoSize);
        }
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
        {
            m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
            m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

            m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);

            m_ActiveScene->OnViewportResize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
        }

        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& e)
    {
        m_CameraController.OnEvent(e);
    }

}
