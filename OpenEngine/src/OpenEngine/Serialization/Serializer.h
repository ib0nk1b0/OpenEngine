#pragma once

#include <iostream>
#include <fstream>

#include "OpenEngine/Core/Core.h"
#include "OpenEngine/Scene/Components.h"

#include <json.hpp>

#include <glm/gtx/string_cast.hpp>

namespace OpenEngine {

	class Serializer
	{
	public:
		Serializer(const Ref<Scene>& scene)
		{
			SetContext(scene);
		}

		void SetContext(const Ref<Scene>& scene)
		{
			m_Scene = scene;
		}

		static std::string GetJSONString(const std::string& label)
		{
			return "\t\"" + label + "\": {\n";
		}

		static std::string GetJSONString(const std::string& label, const std::string& value, bool lastLine = false)
		{
			if (lastLine)
				return "\t\"" + label + "\": \"" + value + "\"\n\t";

			return "\t\"" + label + "\": \"" + value + "\",\n\t";
		}

		static std::string ProjectionTypeToString(SceneCamera::ProjectionType type)
		{
			switch (type)
			{
				case SceneCamera::ProjectionType::Orthographic: return "Orthographic";
				case SceneCamera::ProjectionType::Perspective: return "Perspective";
			}
		}

		static SceneCamera::ProjectionType ProjectionTypeFromString(const std::string& type)
		{
			if (type == "Orthographic") return SceneCamera::ProjectionType::Orthographic;
			if (type == "Perspective") return SceneCamera::ProjectionType::Perspective;
		}

		void Serialize(const std::string& filepath)
		{
			std::ofstream file;
			file.open(filepath);
			
			size_t numberOfEntities = m_Scene->m_Registry.size();
			int count = 0;

			file << "{\n\t";
			file << "\"Entities\": [\n";
			m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
			
				file << "\t\t{\n\t\t";

				if (entity.HasComponent<TagComponent>())
				{
					std::string tag = entity.GetComponent<TagComponent>().Tag.c_str();
					file << GetJSONString("Tag", tag);
				}

				if (entity.HasComponent<TransformComponent>())
				{
					auto& tc = entity.GetComponent<TransformComponent>();
					file << "\t\t\"TransformComponent\": {\n\t\t\t";
					file << GetJSONString("Translation", glm::to_string(tc.Translation)) + "\t\t";
					file << GetJSONString("Rotation", glm::to_string(tc.Rotation)) + "\t\t";
					file << GetJSONString("Scale", glm::to_string(tc.Scale), true);
					file << "\t\t},\n";
				}

				if (entity.HasComponent<SpriteRendererComponent>())
				{
					auto& src = entity.GetComponent<SpriteRendererComponent>();
					file << "\t\t\t\"SpriteRendererComponent\": {\n\t\t\t";
					file << GetJSONString("Color", glm::to_string(src.Color), true);
					file << "\t\t}\n";
				}

				if (entity.HasComponent<CameraComponent>())
				{
					auto& cc = entity.GetComponent<CameraComponent>();
					auto& camera = cc.Camera;
					SceneCamera::ProjectionType projType = camera.GetProjectionType();
					std::string primary = cc.Primary == true ? "true" : "false";
					std::string fixedApectRatio = cc.FixedAspectRatio == true ? "true" : "false";

					file << "\t\t\t\"CameraComponent\": {\n\t\t\t";
					file << GetJSONString("ProjectionType", ProjectionTypeToString(projType)) + "\t\t";
					if (projType == SceneCamera::ProjectionType::Orthographic)
					{
						file << GetJSONString("Size", std::to_string(camera.GetOrthographicSize())) + "\t\t";
						file << GetJSONString("Near", std::to_string(camera.GetOrthographicNearClip())) + "\t\t";
						file << GetJSONString("Far", std::to_string(camera.GetOrthographicFarClip())) + "\t\t";
					}
					else
					{
						file << GetJSONString("VerticalFOV", std::to_string(camera.GetPerspectiveVertivalFOV())) + "\t\t";
						file << GetJSONString("Near", std::to_string(camera.GetPerspectiveNearClip())) + "\t\t";
						file << GetJSONString("Far", std::to_string(camera.GetPerspectiveFarClip())) + "\t\t";
					}
					file << GetJSONString("Primary", primary) + "\t\t";
					file << GetJSONString("FixedAspectRatio", fixedApectRatio, true);
					file << "\t\t}\n";
				}
				if (count == numberOfEntities - 1)
					file << "\t\t}\n";
				else
					file << "\t\t},\n";

				//file << "\t\t}";
				count++;
			});

			file << "\t]\n";
			file << "}";
			file.close();
		}

		void Deserialize(const std::string& filepath)
		{
			std::string result = ReadFile(filepath);

			OE_INFO("{0}", result.c_str());

			OE_CORE_INFO("Deserialization is in development!");
		}

		std::string ReadFile(const std::string& filepath)
		{
			std::ifstream file(filepath);
			nlohmann::json j;
			file >> j;
			if (j.contains("Entities"))
			{
				auto& entities = j["Entities"];
				for (auto& [key, value] : entities.items())
				{
					auto& jsonTag = value["Tag"];
					std::string tag = jsonTag.get<std::string>();

					Entity entity = m_Scene->CreateEntity(tag);
					auto& jsonTransform = value["TransformComponent"];

					std::string translationUnTrimmed = jsonTransform["Translation"].get<std::string>();
					std::string rotationUnTrimmed = jsonTransform["Rotation"].get<std::string>();
					std::string scaleUnTrimmed = jsonTransform["Scale"].get<std::string>();

					size_t size = translationUnTrimmed.size();

					std::string translation = translationUnTrimmed.substr(5, size - 6);
					std::string rotaion = rotationUnTrimmed.substr(5, size - 6);
					std::string scale = scaleUnTrimmed.substr(5, size - 6);

					entity.GetComponent<TransformComponent>().Translation = ConvertFloat3(translation);
					entity.GetComponent<TransformComponent>().Rotation = ConvertFloat3(rotaion);
					entity.GetComponent<TransformComponent>().Scale = ConvertFloat3(scale);

					if (value.contains("SpriteRendererComponent"))
					{
						auto& jsonSpriteRenderer = value["SpriteRendererComponent"];
						std::string colorUnTrimmed = jsonSpriteRenderer["Color"].get<std::string>();
						size_t s = colorUnTrimmed.size();

						std::string color = colorUnTrimmed.substr(5, s - 6);
						entity.AddComponent<SpriteRendererComponent>();
						entity.GetComponent<SpriteRendererComponent>().Color = ConvertFloat4(color);
					}

					if (value.contains("CameraComponent"))
					{
						auto& cameraComponent = entity.AddComponent<CameraComponent>();
						auto& camera = cameraComponent.Camera;
						auto& jsonCameraComponent = value["CameraComponent"];
						std::string projType = jsonCameraComponent["ProjectionType"].get<std::string>();
						camera.SetProjectionType(ProjectionTypeFromString(projType));
						if (projType == "Orthographic")
						{
							camera.SetOrthographicSize(std::stof(jsonCameraComponent["Size"].get<std::string>()));
							camera.SetOrthographicNearClip(std::stof(jsonCameraComponent["Near"].get<std::string>()));
							camera.SetOrthographicFarClip(std::stof(jsonCameraComponent["Far"].get<std::string>()));
						}
						else
						{
							camera.SetPerspectiveVertivalFOV(std::stof(jsonCameraComponent["VerticalFOV"].get<std::string>()));
							camera.SetPerspectiveNearClip(std::stof(jsonCameraComponent["Near"].get<std::string>()));
							camera.SetPerspectiveFarClip(std::stof(jsonCameraComponent["Far"].get<std::string>()));
						}

						std::string primary = jsonCameraComponent["Primary"].get<std::string>();
						if (primary == "true")
							cameraComponent.Primary = true;
						else
							cameraComponent.Primary = false;

						std::string fixedAspectRatio = jsonCameraComponent["FixedAspectRatio"].get<std::string>();
						if (fixedAspectRatio == "true")
							cameraComponent.FixedAspectRatio = true;
						else
							cameraComponent.FixedAspectRatio = false;

					}
				}
			}
			//auto result = nlohmann::json::parse(file);
			return "Hello";
		}

		glm::vec3& ConvertFloat3(std::string values)
		{
			std::string delimiter = ", ";

			std::vector<std::string> convertedValues;

			size_t pos = 0;
			std::string token;

			for (int i = 0; i < 3; i++)
			{
				pos = values.find(delimiter);
				token = values.substr(0, pos);
				convertedValues.push_back(token);
				values.erase(0, pos + delimiter.length());
			}
			
			glm::vec3 float3 = { std::stof(convertedValues[0]), std::stof(convertedValues[1]), std::stof(convertedValues[2]) };

			return float3;
		}

		glm::vec4& ConvertFloat4(std::string values)
		{
			std::string delimiter = ", ";

			std::vector<std::string> convertedValues;

			size_t pos = 0;
			std::string token;

			for (int i = 0; i < 4; i++)
			{
				pos = values.find(delimiter);
				token = values.substr(0, pos);
				convertedValues.push_back(token);
				values.erase(0, pos + delimiter.length());
			}

			glm::vec4 float4 = { std::stof(convertedValues[0]), std::stof(convertedValues[1]), std::stof(convertedValues[2]), std::stof(convertedValues[3]) };

			return float4;
		}

	private:
		Ref<Scene> m_Scene;
	};

}