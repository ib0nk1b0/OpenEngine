#pragma once

#include "Panel.h"

#include <OpenEngine.h>

#include <map>

namespace OpenEngine {

	class PanelManager
	{
	public:
		PanelManager();

		template<typename T>
		void Add()
		{
			m_Panels[T::GetName()] = CreateRef<T>();
		}

		template<typename T>
		Ref<T> Get()
		{
			if (m_Panels.find(T::GetName()) != m_Panels.end())
				return std::static_pointer_cast<T>(m_Panels[T::GetName()]);
		}

		void OnImGuiRender();
	private:
		std::map<std::string, Ref<Panel>> m_Panels;
	};

}
