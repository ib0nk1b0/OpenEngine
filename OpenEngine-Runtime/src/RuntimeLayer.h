#pragma once

#include "OpenEngine.h"

namespace OpenEngine {

	class RuntimeLayer : public OpenEngine::Layer
	{
	public:
		RuntimeLayer();
		virtual ~RuntimeLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(OpenEngine::Timestep ts) override;
		void OnEvent(OpenEngine::Event& e) override;
	private:
		Ref<Scene> m_Scene;
		Ref<Framebuffer> m_Framebuffer;
	};

}
