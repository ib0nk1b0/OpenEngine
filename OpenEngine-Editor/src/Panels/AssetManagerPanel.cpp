#include "oepch.h"
#include "AssetManagerPanel.h"

#include "OpenEngine/Project/Project.h"
#include "OpenEngine/ImGui/ImGuiFonts.h"
#include "OpenEngine/Utils/PlatformUtils.h"

#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace OpenEngine {

	void AssetManagerPanel::OnImGuiRender()
	{
		auto assetHandles = Project::GetActive()->GetAssetManager()->GetAllAssetHandles();
		
		ImGui::Begin("Asset Manager");

		static std::string search = "";
		ImGui::InputText("##search", &search);

		int i = 0;

		for (auto handle : assetHandles)
		{
			auto metadata = Project::GetActive()->GetAssetManager()->GetMetadata(handle);
			std::string id = std::to_string(handle);
			std::string filepath = metadata.FilePath.string();
			std::string type = (std::string)AssetTypeToString(metadata.Type);

			std::vector<std::string> strings = Utils::SplitSting(' ', search);
			std::vector<bool> results;
			bool contains = true;

			for (std::string str : strings)
			{
				std::string lowerCaseFilepath = filepath;
				std::string lowerCaseType = type;
				std::transform(lowerCaseFilepath.begin(), lowerCaseFilepath.end(), lowerCaseFilepath.begin(), ::tolower);
				std::transform(lowerCaseType.begin(), lowerCaseType.end(), lowerCaseType.begin(), ::tolower);
				std::transform(str.begin(), str.end(), str.begin(), ::tolower);
				if (id.find(str) != std::string::npos ||
					lowerCaseFilepath.find(str) != std::string::npos ||
					lowerCaseType.find(str) != std::string::npos)
					results.push_back(true);
				else
					results.push_back(false);
			}

			for (size_t i = 0; i < results.size(); i++)
			{
				if (results[i] == false)
					contains = false;
			}

			if (!contains)
				continue;

			ImGui::PushID(i++);
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 100.0f);
			
			ImGui::Text("Handle: ");
			ImGui::NextColumn();
			ImGui::Text("%s", id.c_str());

			ImGui::NextColumn();

			ImGui::Text("Filepath: ");
			ImGui::NextColumn();
			ImGui::Text("%s", filepath.c_str());

			ImGui::NextColumn();

			ImGui::Text("Type: ");
			ImGui::NextColumn();
			ImGui::Text("%s", type.c_str());

			ImGui::Columns(1);

			ImGui::Separator();

			ImGui::PopID();
		}

		ImGui::End();
	}

}
