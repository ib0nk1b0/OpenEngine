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

		static std::string Tab(uint32_t count = 1)
		{
			std::string result;
			for (size_t i = 0; i < count; i++)
				result += "\t";

			return result;
		}

		static std::string NewLine()
		{
			return "\n";
		}

		static std::string GetJSONString(const std::string& label)
		{
			return Tab(3) + "\"" + label + "\": {" + NewLine() + Tab();
		}

		static std::string GetJSONString(const std::string& label, const std::string& value, bool lastLine = false)
		{
			std::string result;

			result += Tab(3) + "\"" + label + "\": ";

			if (value[0] == '[') result += value;
			else result += "\"" + value + "\"";

			if (!lastLine)
				result += ",";

			result += NewLine() + Tab();

			return result;
		}

		static std::string GetJSONString(const std::string& label, int value, bool lastLine = false)
		{
			std::string result;

			result += Tab(3) + "\"" + label + "\": ";

			result += std::to_string(value);

			if (!lastLine)
				result += ",";

			result += NewLine() + Tab();

			return result;
		}

		static std::string GetJSONString(const std::string& label, float value, bool lastLine = false)
		{
			std::string result;

			result += Tab(3) + "\"" + label + "\": ";

			result += std::to_string(value);

			if (!lastLine)
				result += ",";

			result += NewLine() + Tab();

			return result;
		}

		static std::string GetJSONString(const std::string& label, bool value, bool lastLine = false)
		{
			std::string result;

			result += Tab(3) + "\"" + label + "\": ";

			if (value) result += "true";
			else result += "false";

			if (!lastLine)
				result += ",";

			result += NewLine() + Tab();

			return result;
		}

		static std::string ProjectionTypeToString(SceneCamera::ProjectionType type)
		{
			switch (type)
			{
			case SceneCamera::ProjectionType::Orthographic: return "Orthographic";
			case SceneCamera::ProjectionType::Perspective: return "Perspective";
			}

			return "";
		}

		static SceneCamera::ProjectionType ProjectionTypeFromString(const std::string& type)
		{
			if (type == "Orthographic") return SceneCamera::ProjectionType::Orthographic;
			if (type == "Perspective") return SceneCamera::ProjectionType::Perspective;

			return SceneCamera::ProjectionType::Orthographic;
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

		Serializer(const Ref<Scene>& scene)
		{
			SetContext(scene);
		}

		void SetContext(const Ref<Scene>& scene)
		{
			m_Scene = scene;
		}

		void Serialize(const std::string& filepath)
		{
			std::string text;
			
			size_t numberOfEntities = m_Scene->m_Registry.size();
			int count = 0;

			text += "{" + NewLine() + Tab();
			text += "\"Entities\": [" + NewLine();
			m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
			
				text += Tab(2) + "{" + NewLine(); // Tab(2)

				if (entity.HasComponent<TagComponent>())
				{
					std::string tag = entity.GetComponent<TagComponent>().Tag.c_str();
					text += Tab(3) + "\"ID\": " + "85493002," + NewLine();
					text += GetJSONString("TagComponent");
					text += GetJSONString("Tag", tag, true);
					text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<TransformComponent>())
				{
					auto& tc = entity.GetComponent<TransformComponent>();

					text += GetJSONString("TransformComponent");
					text += GetJSONString("Translation", Encode(tc.Translation));
					text += GetJSONString("Rotation", Encode(tc.Rotation));
					text += GetJSONString("Scale", Encode(tc.Scale), true);
					if (!entity.HasComponent<SpriteRendererComponent>() && !entity.HasComponent<CircleRendererComponent>() && !entity.HasComponent<CameraComponent>())
						text += Tab(2) + "}" + NewLine();
					else
						text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<SpriteRendererComponent>())
				{
					auto& src = entity.GetComponent<SpriteRendererComponent>();
					text += GetJSONString("SpriteRendererComponent");

					if (src.Texture)
					{
						std::string filepathOrginal = src.Texture->GetFilePath();
						std::string filepath;
						char delimiter = '\\';
						size_t pos = 0;
						std::string token;

						int no_of_backslash = (int)std::count(filepathOrginal.begin(), filepathOrginal.end(), '\\');
						if (no_of_backslash > 0)
						{
							for (int i = 0; i < no_of_backslash + 1; i++)
							{
								pos = filepathOrginal.find(delimiter);
								token = filepathOrginal.substr(0, pos);
								filepath += token + "/";
								filepathOrginal.erase(0, pos + 1);
							}
							filepath.erase(filepath.length() - 1, filepath.length());
						}
						else
							filepath = filepathOrginal;

						text += GetJSONString("Color", Encode(src.Color));
						text += GetJSONString("Texture", filepath);
						text += GetJSONString("Scale", src.Scale, true);
					}
					else
						text += GetJSONString("Color", Encode(src.Color), true);

					if (!entity.HasComponent<CircleRendererComponent>() && !entity.HasComponent<CameraComponent>())
						text += Tab(2) + "}" + NewLine();
					else
						text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<CircleRendererComponent>())
				{
					auto& crc = entity.GetComponent<CircleRendererComponent>();
					text += GetJSONString("CircleRendererComponent");
					text += GetJSONString("Color", Encode(crc.Color));
					text += GetJSONString("Thickness", crc.Thickness);
					text += GetJSONString("Fade", crc.Fade, true);
					if (!entity.HasComponent<CameraComponent>())
						text += Tab(2) + "}" + NewLine();
					else
						text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<CameraComponent>())
				{
					auto& cc = entity.GetComponent<CameraComponent>();
					auto& camera = cc.Camera;
					SceneCamera::ProjectionType projType = camera.GetProjectionType();

					text += GetJSONString("CameraComponent");
					text += GetJSONString("ProjectionType", ProjectionTypeToString(projType));
					if (projType == SceneCamera::ProjectionType::Orthographic)
					{
						text += GetJSONString("Size", camera.GetOrthographicSize());
						text += GetJSONString("Near", camera.GetOrthographicNearClip());
						text += GetJSONString("Far", camera.GetOrthographicFarClip());
					}
					else
					{
						text += GetJSONString("VerticalFOV", camera.GetPerspectiveVertivalFOV());
						text += GetJSONString("Near", camera.GetPerspectiveNearClip());
						text += GetJSONString("Far", camera.GetPerspectiveFarClip());
					}
					
					text += GetJSONString("Primary", cc.Primary);
					text += GetJSONString("FixedAspectRatio", cc.FixedAspectRatio, true);
					text += Tab(2) + "}" + NewLine();
				}

				// TODO: Add script component

				if (count == numberOfEntities - 1)
					text += Tab(2) + "}" + NewLine();
				else
					text += Tab(2) + "}," + NewLine();

				count++;
			});

			text += Tab() + "]" + NewLine();
			text += "}";

			std::ofstream file;
			file.open(filepath);
			file << text;
			file.close();
		}

		void Deserialize(const std::string& filepath)
		{
			std::ifstream file(filepath);
			nlohmann::json j;
			file >> j;
			if (j.contains("Entities"))
			{
				auto& entities = j["Entities"];
				for (auto& [key, value] : entities.items())
				{
					uint64_t uuid = value["ID"].get<uint64_t>();
					//std::string tag = value["TagComponent"]["Tag"].get<std::string>();

					Entity entity = m_Scene->CreateEntity(value["TagComponent"]["Tag"].get<std::string>());
					auto& jsonTransform = value["TransformComponent"];
					TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();

					Decode(ConvertFloat3(jsonTransform["Translation"]), transformComponent.Translation);
					Decode(ConvertFloat3(jsonTransform["Rotation"]), transformComponent.Rotation);
					Decode(ConvertFloat3(jsonTransform["Scale"]), transformComponent.Scale);

					if (value.contains("SpriteRendererComponent"))
					{
						auto& jsonSpriteRenderer = value["SpriteRendererComponent"];
						auto& src = entity.AddComponent<SpriteRendererComponent>();

						Decode(ConvertFloat4(jsonSpriteRenderer["Color"]), src.Color);
						if (jsonSpriteRenderer.contains("Texture"))
						{
							src.Texture = Texture2D::Create(jsonSpriteRenderer["Texture"].get<std::string>());
							src.Scale = jsonSpriteRenderer["Scale"].get<float>();
						}
						// TODO: Add texture deserialization
					}

					if (value.contains("CircleRendererComponent"))
					{
						auto& jsonCircleRenderer = value["CircleRendererComponent"];
						entity.AddComponent<CircleRendererComponent>();

						Decode(ConvertFloat4(jsonCircleRenderer["Color"]), entity.GetComponent<CircleRendererComponent>().Color);
						entity.GetComponent<CircleRendererComponent>().Thickness = jsonCircleRenderer["Thickness"].get<float>();
						entity.GetComponent<CircleRendererComponent>().Fade = jsonCircleRenderer["Fade"].get<float>();
					}

					if (value.contains("CameraComponent"))
					{
						auto& jsonCameraComponent = value["CameraComponent"];
						auto& cameraComponent = entity.AddComponent<CameraComponent>();
						auto& camera = cameraComponent.Camera;

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
							camera.SetPerspectiveVertivalFOV(jsonCameraComponent["VerticalFOV"].get<float>());
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
