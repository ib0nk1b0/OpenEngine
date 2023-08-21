#pragma once

#include <iostream>
#include <fstream>

#include "OpenEngine/Core/Core.h"
#include "OpenEngine/Scene/Components.h"
#include "OpenEngine/Scene/Scene.h"

#include <json.hpp>

#include <glm/gtx/string_cast.hpp>

namespace OpenEngine {

	class Serializer
	{
	public:
		Serializer(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void Serialize(const std::string& filepath);
		void Deserialize(const std::string& filepath);

		std::string ConvertFloat3(nlohmann::json_abi_v3_11_2::json data);
		std::string ConvertFloat4(nlohmann::json_abi_v3_11_2::json data);

	private:
		Ref<Scene> m_Scene;
	};

}
