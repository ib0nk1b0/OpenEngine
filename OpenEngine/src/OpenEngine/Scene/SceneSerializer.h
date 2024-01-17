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

		void Serialize(const std::filesystem::path& filepath);
		void Deserialize(const std::filesystem::path& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}