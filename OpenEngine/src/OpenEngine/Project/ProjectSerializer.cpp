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
			{ "StartScene", (uint64_t)config.StartScene },
			{ "AssetDirectory", config.AssetDirectory },
			{ "AssetRegistryPath", config.AssetRegistryPath },
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
			config.StartScene = deserializedJson["Project"]["StartScene"].get<uint64_t>();
			config.AssetDirectory = deserializedJson["Project"]["AssetDirectory"].get<std::string>();
			if (deserializedJson["Project"].contains("AssetRegistryPath"))
				config.AssetRegistryPath = deserializedJson["Project"]["AssetRegistryPath"].get<std::string>();
			config.ScriptModulePath = deserializedJson["Project"]["ScriptModulePath"].get<std::string>();

			return true;
		}

		return false;
	}

}
