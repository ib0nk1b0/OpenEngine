#pragma once

#include "OpenEngine/Project/Project.h"
#include "OpenEngine/Serialization/Serializer.h"

namespace OpenEngine {

	class ProjectSerializer
	{
	public:
		ProjectSerializer(Ref<Project> project);

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);
	private:
		Ref<Project> m_Project;
	};

}