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
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time;
	};

}