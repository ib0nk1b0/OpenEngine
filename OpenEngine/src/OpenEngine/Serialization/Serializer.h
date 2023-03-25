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
			std::string result;

			result += "\t\"" + label + "\": ";

			if (value[0] == '[') result += value;
			else result += "\"" + value + "\"";

			if (lastLine) result += "\n\t";
			else result += ",\n\t";

			return result;
		}

		static std::string GetJSONString(const std::string& label, int value, bool lastLine = false)
		{
			std::string result;

			result += "\t\"" + label + "\": ";

			result += std::to_string(value);

			if (lastLine) result += "\n\t";
			else result += ",\n\t";

			return result;
		}

		static std::string GetJSONString(const std::string& label, float value, bool lastLine = false)
		{
			std::string result;

			result += "\t\"" + label + "\": ";

			result += std::to_string(value);

			if (lastLine) result += "\n\t";
			else result += ",\n\t";

			return result;
		}

		static std::string GetJSONString(const std::string& label, bool value, bool lastLine = false)
		{
			std::string result;

			result += "\t\"" + label + "\": ";

			if (value) result += "true";
			else result += "false";

			if (lastLine) result += "\n\t";
			else result += ",\n\t";

			return result;
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

		static std::string Encode(glm::vec3& values)
		{
			std::string result;

			result += "[" + std::to_string(values[0]) + ", ";
			result += std::to_string(values[1]) + ", ";
			result += std::to_string(values[2]) + "]";

			return result;
		}

		static void Decode(std::string values, glm::vec3& result)
		{
			std::string delimiter = ", ";
			std::vector<std::string> convertedValues;

			size_t pos = 0;
			std::string token;
			values.erase(0, 1);
			values.erase(values.length() - 1, 1);
			for (int i = 0; i < 3; i++)
			{
				pos = values.find(delimiter);
				token = values.substr(0, pos);
				convertedValues.push_back(token);
				values.erase(0, pos + delimiter.length());
			}

			result.x = std::stof(convertedValues[0]);
			result.y = std::stof(convertedValues[1]);
			result.z = std::stof(convertedValues[2]);
		}

		static std::string Encode(glm::vec4& values)
		{
			std::string result;

			result += "[" + std::to_string(values[0]) + ", ";
			result += std::to_string(values[1]) + ", ";
			result += std::to_string(values[2]) + ", ";
			result += std::to_string(values[3]) + "]";

			return result;
		}

		static void Decode(std::string values, glm::vec4& result)
		{
			std::string delimiter = ", ";
			std::vector<std::string> convertedValues;

			size_t pos = 0;
			std::string token;
			values.erase(0, 1);
			values.erase(values.length() - 1, 1);
			for (int i = 0; i < 4; i++)
			{
				pos = values.find(delimiter);
				token = values.substr(0, pos);
				convertedValues.push_back(token);
				values.erase(0, pos + delimiter.length());
			}

			result.r = std::stof(convertedValues[0]);
			result.g = std::stof(convertedValues[1]);
			result.b = std::stof(convertedValues[2]);
			result.a = std::stof(convertedValues[3]);
		}

		void Serialize(const std::string& filepath)
		{
			//using namespace nlohmann;
			std::string text;
			
			size_t numberOfEntities = m_Scene->m_Registry.size();
			int count = 0;

			text += "{\n\t";
			text += "\"Entities\": [\n";
			m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
			
				text += "\t\t{\n\t\t";

				if (entity.HasComponent<TagComponent>())
				{
					std::string tag = entity.GetComponent<TagComponent>().Tag.c_str();
					text += GetJSONString("ID", 85493002);
					text += "\t\t\"TagComponent\": {\n\t\t\t";
					text += GetJSONString("Tag", tag, true);
					text += "\t\t},\n";
				}

				if (entity.HasComponent<TransformComponent>())
				{
					auto& tc = entity.GetComponent<TransformComponent>();

					text += "\t\t\t\"TransformComponent\": {\n\t\t\t";
					text += GetJSONString("Translation", Encode(tc.Translation)) + "\t\t";
					text += GetJSONString("Rotation", Encode(tc.Rotation)) + "\t\t";
					text += GetJSONString("Scale", Encode(tc.Scale), true);
					text += "\t\t},\n";
				}

				if (entity.HasComponent<SpriteRendererComponent>())
				{
					auto& src = entity.GetComponent<SpriteRendererComponent>();
					text += "\t\t\t\"SpriteRendererComponent\": {\n\t\t\t";
					text += GetJSONString("Color", Encode(src.Color), true);
					text += "\t\t}\n";
				}

				if (entity.HasComponent<CameraComponent>())
				{
					auto& cc = entity.GetComponent<CameraComponent>();
					auto& camera = cc.Camera;
					SceneCamera::ProjectionType projType = camera.GetProjectionType();

					text += "\t\t\t\"CameraComponent\": {\n\t\t\t";
					text += GetJSONString("ProjectionType", ProjectionTypeToString(projType)) + "\t\t";
					if (projType == SceneCamera::ProjectionType::Orthographic)
					{
						text += GetJSONString("Size", camera.GetOrthographicSize()) + "\t\t";
						text += GetJSONString("Near", camera.GetOrthographicNearClip()) + "\t\t";
						text += GetJSONString("Far", camera.GetOrthographicFarClip()) + "\t\t";
					}
					else
					{
						text += GetJSONString("VerticalFOV", camera.GetPerspectiveVertivalFOV()) + "\t\t";
						text += GetJSONString("Near", camera.GetPerspectiveNearClip()) + "\t\t";
						text += GetJSONString("Far", camera.GetPerspectiveFarClip()) + "\t\t";
					}
					
					text += GetJSONString("Primary", cc.Primary) + "\t\t";
					text += GetJSONString("FixedAspectRatio", cc.FixedAspectRatio, true);
					text += "\t\t}\n";
				}
				if (count == numberOfEntities - 1)
					text += "\t\t}\n";
				else
					text += "\t\t},\n";

				count++;
			});

			text += "\t]\n";
			text += "}";

			std::ofstream file;
			file.open(filepath);
			file << text;
			file.close();
		}

		void Deserialize(const std::string& filepath)
		{
			ReadFile(filepath);

			OE_CORE_INFO("Deserialization is in development!");
		}

		void ReadFile(const std::string& filepath)
		{
			std::ifstream file(filepath);
			nlohmann::json j;
			file >> j;
			if (j.contains("Entities"))
			{
				auto& entities = j["Entities"];
				for (auto& [key, value] : entities.items())
				{
					auto& jsonTag = value["TagComponent"]["Tag"];
					std::string tag = jsonTag.get<std::string>();

					Entity entity = m_Scene->CreateEntity(tag);
					auto& jsonTransform = value["TransformComponent"];

					Decode(ConvertFloat3(jsonTransform["Translation"]), entity.GetComponent<TransformComponent>().Translation);
					Decode(ConvertFloat3(jsonTransform["Rotation"]), entity.GetComponent<TransformComponent>().Rotation);
					Decode(ConvertFloat3(jsonTransform["Scale"]), entity.GetComponent<TransformComponent>().Scale);

					if (value.contains("SpriteRendererComponent"))
					{
						auto& jsonSpriteRenderer = value["SpriteRendererComponent"];
						entity.AddComponent<SpriteRendererComponent>();
						
						Decode(ConvertFloat4(jsonSpriteRenderer["Color"]), entity.GetComponent<SpriteRendererComponent>().Color);
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
							camera.SetOrthographicSize(jsonCameraComponent["Size"].get<float>());
							camera.SetOrthographicNearClip(jsonCameraComponent["Near"].get<float>());
							camera.SetOrthographicFarClip(jsonCameraComponent["Far"].get<float>());
						}
						else
						{
							camera.SetPerspectiveVertivalFOV(jsonCameraComponent["VertivalFOV"].get<float>());
							camera.SetPerspectiveNearClip(jsonCameraComponent["Near"].get<float>());
							camera.SetPerspectiveFarClip(jsonCameraComponent["Far"].get<float>());
						}

						cameraComponent.Primary = jsonCameraComponent["Primary"].get<bool>();
						cameraComponent.FixedAspectRatio = jsonCameraComponent["FixedAspectRatio"].get<bool>();
					}
				}
			}
		}

		std::string ConvertFloat3(nlohmann::json_abi_v3_11_2::json data)
		{
			std::string result;
			int count = 0;

			result += "[";
			for (auto it = data.begin(); it != data.end(); ++it)
			{
				result += std::to_string((float)it.value());
				if (count < 2) result += ", ";
				count++;
			}
			result += "]";

			return result;
		}

		std::string ConvertFloat4(nlohmann::json_abi_v3_11_2::json data)
		{
			std::string result;
			int count = 0;

			result += "[";
			for (auto it = data.begin(); it != data.end(); ++it)
			{
				result += std::to_string((float)it.value());
				if (count < 3) result += ", ";
				count++;
			}
			result += "]";

			return result;
		}

	private:
		Ref<Scene> m_Scene;
	};

}
