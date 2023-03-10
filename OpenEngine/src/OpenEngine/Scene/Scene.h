#pragma once

#include "OpenEngine.h"
#include "entt.hpp"

namespace OpenEngine {

	class Scene {
	public:
		Scene();
		~Scene();

		entt::entity CreateEntity();

		// TEMP
		entt::registry& Reg() { return m_Registry; }

		void OnUpdate(Timestep ts);
	private:
		entt::registry m_Registry;
	};

}