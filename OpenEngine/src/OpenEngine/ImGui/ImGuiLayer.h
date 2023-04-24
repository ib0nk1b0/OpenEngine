#pragma once

#include "OpenEngine/Core/Layer.h"

#include "OpenEngine/Events/KeyEvent.h"
#include "OpenEngine/Events/MouseEvent.h"
#include "OpenEngine/Events/ApplicationEvent.h"

namespace OpenEngine {

	class OPENENGINE_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void InitOpenGL();
		void InitVulkan();

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		void SetDarkThemeColors();
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};

}