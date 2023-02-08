#pragma once

#include "OpenEngine/Core.h"
#include "OpenEngine/Events/Event.h"

namespace OpenEngine {
	class OPENENGINE_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}