#include "oepch.h"
#include "Serializer.h"

#include "OpenEngine/Scene/Entity.h"

namespace OpenEngine {

#pragma region StaticFunctions
	
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

	static std::string StartJSONObject(const std::string& label)
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

	static std::string BodyTypeToString(BodyType type)
	{
		switch (type)
		{
		case BodyType::Static:
			return "Static";
			break;
		case BodyType::Dynamic:
			return "Dynamic";
			break;
		}

		return "Static";
	}

	static BodyType BodyTypeFromString(const std::string& type)
	{
		if (type == "Static") return BodyType::Static;
		if (type == "Dynamic") return BodyType::Dynamic;
		return BodyType::Static;
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
#pragma endregion

	Serializer::Serializer(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void Serializer::SetContext(const Ref<Scene>& scene)
	{
		m_Scene = scene;
	}

	void Serializer::Serialize(const std::string& filepath)
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
					text += Tab(3) + "\"ID\": " + std::to_string(entity.GetUUID()) + "," + NewLine();
					text += StartJSONObject("TagComponent");
					text += GetJSONString("Tag", tag, true);
					text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<ParentComponent>())
				{
					auto& pc = entity.GetComponent<ParentComponent>();

					text += StartJSONObject("ParentComponent");
					text += GetJSONString("ParentName", pc.ParentName);
					text += Tab(4) + "\"ParentID\": " + std::to_string(pc.ParentID) + "," + NewLine();
					text += GetJSONString("Offset", Encode(pc.Offset), true);

					if (!entity.HasComponent<TransformComponent>() &&
						!entity.HasComponent<SpriteRendererComponent>() &&
						!entity.HasComponent<EditorRendererComponent>() &&
						!entity.HasComponent<CircleRendererComponent>() &&
						!entity.HasComponent<MeshComponent>() &&
						!entity.HasComponent<CameraComponent>() &&
						!entity.HasComponent<RigidBody2DComponent>() &&
						!entity.HasComponent<BoxColider2DComponent>())
						text += Tab(2) + "}" + NewLine();
					else
						text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<TransformComponent>())
				{
					auto& tc = entity.GetComponent<TransformComponent>();

					text += StartJSONObject("TransformComponent");
					text += GetJSONString("Translation", Encode(tc.Translation));
					text += GetJSONString("Rotation", Encode(tc.Rotation));
					text += GetJSONString("Scale", Encode(tc.Scale), true);
					if (!entity.HasComponent<SpriteRendererComponent>() &&
						!entity.HasComponent<EditorRendererComponent>() &&
						!entity.HasComponent<CircleRendererComponent>() &&
						!entity.HasComponent<MeshComponent>() &&
						!entity.HasComponent<CameraComponent>() &&
						!entity.HasComponent<RigidBody2DComponent>() &&
						!entity.HasComponent<BoxColider2DComponent>())
						text += Tab(2) + "}" + NewLine();
					else
						text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<SpriteRendererComponent>())
				{
					auto& src = entity.GetComponent<SpriteRendererComponent>();
					text += StartJSONObject("SpriteRendererComponent");

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

					if (!entity.HasComponent<EditorRendererComponent>() &&
						!entity.HasComponent<CircleRendererComponent>() &&
						!entity.HasComponent<MeshComponent>() &&
						!entity.HasComponent<CameraComponent>() &&
						!entity.HasComponent<RigidBody2DComponent>() &&
						!entity.HasComponent<BoxColider2DComponent>())
						text += Tab(2) + "}" + NewLine();
					else
						text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<EditorRendererComponent>())
				{
					auto& erc = entity.GetComponent<EditorRendererComponent>();
					text += StartJSONObject("EditorRendererComponent");
					if (erc.Texture)
					{
						std::string filepathOrginal = erc.Texture->GetFilePath();
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

						text += GetJSONString("Color", Encode(erc.Color));
						text += GetJSONString("Texture", filepath, true);
					}
					else
						text += GetJSONString("Color", Encode(erc.Color), true);

					if (!entity.HasComponent<CircleRendererComponent>() &&
						!entity.HasComponent<MeshComponent>() &&
						!entity.HasComponent<CameraComponent>() &&
						!entity.HasComponent<RigidBody2DComponent>() &&
						!entity.HasComponent<BoxColider2DComponent>())
						text += Tab(2) + "}" + NewLine();
					else
						text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<CircleRendererComponent>())
				{
					auto& crc = entity.GetComponent<CircleRendererComponent>();
					text += StartJSONObject("CircleRendererComponent");
					text += GetJSONString("Color", Encode(crc.Color));
					text += GetJSONString("Thickness", crc.Thickness);
					text += GetJSONString("Fade", crc.Fade, true);
					if (!entity.HasComponent<MeshComponent>() &&
						!entity.HasComponent<CameraComponent>() &&
						!entity.HasComponent<RigidBody2DComponent>() &&
						!entity.HasComponent<BoxColider2DComponent>())
						text += Tab(2) + "}" + NewLine();
					else
						text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<MeshComponent>())
				{
					auto& mc = entity.GetComponent<MeshComponent>();
					text += StartJSONObject("MeshComponent");
					text += GetJSONString("MaterialIndex", mc.MaterialIndex, true);
					
					if (!entity.HasComponent<CameraComponent>() &&
						!entity.HasComponent<RigidBody2DComponent>() &&
						!entity.HasComponent<BoxColider2DComponent>())
						text += Tab(2) + "}" + NewLine();
					else
						text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<CameraComponent>())
				{
					auto& cc = entity.GetComponent<CameraComponent>();
					auto& camera = cc.Camera;
					SceneCamera::ProjectionType projType = camera.GetProjectionType();

					text += StartJSONObject("CameraComponent");
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

					if (!entity.HasComponent<RigidBody2DComponent>() &&
						!entity.HasComponent<BoxColider2DComponent>())
						text += Tab(2) + "}" + NewLine();
					else
						text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<RigidBody2DComponent>())
				{
					auto& rgb2d = entity.GetComponent<RigidBody2DComponent>();
					
					text += StartJSONObject("RigidBody2DComponent");
					text += GetJSONString("Type", BodyTypeToString(rgb2d.Type), true);

					if (!entity.HasComponent<BoxColider2DComponent>())
						text += Tab(2) + "}" + NewLine();
					else
						text += Tab(2) + "}," + NewLine();
				}

				if (entity.HasComponent<BoxColider2DComponent>())
				{
					auto& bc2d = entity.GetComponent<BoxColider2DComponent>();
					
					text += StartJSONObject("BoxColider2DComponent");
					text += GetJSONString("Size", Encode(bc2d.Size));
					text += GetJSONString("Offset", Encode(bc2d.Offset), true);

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

	void Serializer::Deserialize(const std::string& filepath)
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
				std::string tag = value["TagComponent"]["Tag"].get<std::string>();

				Entity entity = m_Scene->CreateEntityWithUUID(uuid, tag);

				if (value.contains("ParentComponent"))
				{
					auto& jsonParentComponent = value["ParentComponent"];
					auto& parentComponent = entity.GetComponent<ParentComponent>();
					parentComponent.ParentName = jsonParentComponent["ParentName"].get<std::string>();
					parentComponent.ParentID = jsonParentComponent["ParentID"].get<uint64_t>();
					Decode(ConvertFloat3(jsonParentComponent["Offset"]), parentComponent.Offset);
				}

				if (value.contains("TransformComponent"))
				{
					auto& jsonTransform = value["TransformComponent"];
					TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();

					Decode(ConvertFloat3(jsonTransform["Translation"]), transformComponent.Translation);
					Decode(ConvertFloat3(jsonTransform["Rotation"]), transformComponent.Rotation);
					Decode(ConvertFloat3(jsonTransform["Scale"]), transformComponent.Scale);
				}

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
				}

				if (value.contains("EditorRendererComponent"))
				{
					auto& jsonEditotRenderer = value["EditorRendererComponent"];
					auto& erc = entity.AddComponent<EditorRendererComponent>();

					Decode(ConvertFloat4(jsonEditotRenderer["Color"]), erc.Color);
					if (jsonEditotRenderer.contains("Texture"))
					{
						erc.Texture = Texture2D::Create(jsonEditotRenderer["Texture"].get<std::string>());
					}
				}

				if (value.contains("CircleRendererComponent"))
				{
					auto& jsonCircleRenderer = value["CircleRendererComponent"];
					entity.AddComponent<CircleRendererComponent>();

					Decode(ConvertFloat4(jsonCircleRenderer["Color"]), entity.GetComponent<CircleRendererComponent>().Color);
					entity.GetComponent<CircleRendererComponent>().Thickness = jsonCircleRenderer["Thickness"].get<float>();
					entity.GetComponent<CircleRendererComponent>().Fade = jsonCircleRenderer["Fade"].get<float>();
				}

				if (value.contains("MeshComponent"))
				{
					auto& jsonMeshComponent = value["MeshComponent"];
					auto& mc = entity.AddComponent<MeshComponent>();

					mc.MaterialIndex = jsonMeshComponent["MaterialIndex"].get<int>();


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
			
				if (value.contains("RigidBody2DComponent"))
				{
					auto& jsonRGBD2D = value["RigidBody2DComponent"];
					auto& rgbd2DComponent = entity.AddComponent<RigidBody2DComponent>();

					rgbd2DComponent.Type = BodyTypeFromString(jsonRGBD2D["Type"].get<std::string>());
				}
			
				if (value.contains("BoxColider2DComponent"))
				{
					auto& jsonBC2D = value["BoxColider2DComponent"];
					auto& bc2dComponent = entity.AddComponent<BoxColider2DComponent>();

					Decode(ConvertFloat3(jsonBC2D["Size"]), bc2dComponent.Size);
					Decode(ConvertFloat3(jsonBC2D["Offset"]), bc2dComponent.Offset);
				}

			}
		}
	}

	std::string Serializer::ConvertFloat3(nlohmann::json_abi_v3_11_2::json data)
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

	std::string Serializer::ConvertFloat4(nlohmann::json_abi_v3_11_2::json data)
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
}