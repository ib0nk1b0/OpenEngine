#pragma once

#include "OpenEngine/Scene/Components.h"
#include "OpenEngine/Scene/Scene.h"
#include "OpenEngine/Utils/PlatformUtils.h"

namespace OpenEngine {

	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);
		void SetContext(const Ref<Scene>& scene);

		void Serialize(const std::string& filepath);
		void Deserialize(const std::string& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}