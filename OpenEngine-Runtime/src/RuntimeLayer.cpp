#include "RuntimeLayer.h"

#include <OpenEngine/Asset/AssetManager.h>
#include <OpenEngine/Project/Project.h>

namespace OpenEngine {

	RuntimeLayer::RuntimeLayer()
	{
	}

	void RuntimeLayer::OnAttach()
	{
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1600;
		fbSpec.Height = 900;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_Scene = CreateRef<Scene>();

		// OpenProject
		AssetHandle startScene = 0;
		std::filesystem::path filepath = "SandboxProject/Sandbox.oeproj";
		auto& project = Project::GetActive();
		if (Project::Load(filepath))
		{
			startScene = project->GetConfig().StartScene;
			
		}

		if (startScene)
		{
			Ref<Scene> readOnlyScene = AssetManager::GetAsset<Scene>(startScene);
			Ref<Asset> asset = project->GetAssetManager()->GetAsset(startScene);
			Ref<Scene> newScene = CreateRef<Scene>();
			readOnlyScene->CopyTo(newScene);
			m_Scene = newScene;
			auto filepath = Project::GetActiveAssetDirectory() / Project::GetActive()->GetAssetManager()->GetFilePath(startScene);
			m_Scene->SetFilepath(filepath);
			m_Scene->OnRuntimeStart();
		}

	}

	void RuntimeLayer::OnDetach()
	{
		m_Scene->OnRuntimeStop();
	}

	void RuntimeLayer::OnUpdate(OpenEngine::Timestep ts)
	{
		m_Framebuffer->Bind();

		RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		RenderCommand::Clear();

		m_Framebuffer->ClearColorAttachment(1, -1);

		m_Scene->OnUpdateRuntime(ts);

		m_Framebuffer->UnBind();
	}

	void RuntimeLayer::OnEvent(OpenEngine::Event& e)
	{
	}

}
