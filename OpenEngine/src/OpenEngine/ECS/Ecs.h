#pragma once

#include "OpenEngine.h"

namespace OpenEngine {

	class Container {
	public:
		Container() {}

		template<typename T, typename... Args>
		T& Add(uint32_t entityHandle, Args&&... args)
		{
			m_Components.push_back(entityHandle, T);
		}

		template<typename T>
		bool Has()
		{
			
		}

		Container Create()
		{
			return new Container();
		}
	private:
		std::vector<uint32_t, struct> m_Components;
	};

}