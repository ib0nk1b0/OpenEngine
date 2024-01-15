#include "oepch.h"
#include "ProjectSerializer.h"

namespace OpenEngine {

	ProjectSerializer::ProjectSerializer(Ref<Project> project)
		: m_Project(project)
	{
	}

	bool ProjectSerializer::Serialize(const std::filesystem::path& filepath)
	{
		const auto& config = m_Project->GetConfig();

		nlohmann::ordered_json jsonData;
		jsonData["Project"] =
		{
			{ "Name", config.Name },
			{ "StartScene", config.StartScene },
			{ "AssetDirectory", config.AssetDirectory },
			{ "ScriptModulePath", config.ScriptModulePath },
		};

		std::ofstream jsonOut(filepath);
		jsonOut << std::setw(4) << jsonData;

		return true;
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		std::ifstream file(filepath);
		nlohmann::json deserializedJson;
		file >> deserializedJson;

		auto& config = m_Project->GetConfig();

		if (deserializedJson.contains("Project"))
		{
			config.Name = deserializedJson["Project"]["Name"].get<std::string>();
			config.StartScene = deserializedJson["Project"]["StartScene"].get<std::string>();
			config.AssetDirectory = deserializedJson["Project"]["AssetDirectory"].get<std::string>();
			config.ScriptModulePath = deserializedJson["Project"]["ScriptModulePath"].get<std::string>();

			return true;
		}

		return false;
	}

}
