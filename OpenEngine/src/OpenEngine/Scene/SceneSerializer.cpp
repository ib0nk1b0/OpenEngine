#include "oepch.h"
#include "SceneSerializer.h"

#include "OpenEngine/Project/Project.h"
#include "OpenEngine/Scene/Entity.h"
#include "OpenEngine/Serialization/Serializer.h"
#include "OpenEngine/Asset/Asset.h"

namespace OpenEngine {

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void SceneSerializer::SetContext(const Ref<Scene>& scene)
	{
		m_Scene = scene;
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

	static std::string BodyTypeToString(RigidBody2DComponent::BodyType type)
	{
		switch (type)
		{
			case RigidBody2DComponent::BodyType::Static: return "Static";
			case RigidBody2DComponent::BodyType::Dynamic: return "Dynamic";
			case RigidBody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		return "Static";
	}

	static RigidBody2DComponent::BodyType BodyTypeFromString(const std::string& type)
	{
		if (type == "Static") return RigidBody2DComponent::BodyType::Static;
		if (type == "Dynamic") return RigidBody2DComponent::BodyType::Dynamic;
		if (type == "Kinematic") return RigidBody2DComponent::BodyType::Kinematic;
		return RigidBody2DComponent::BodyType::Static;
	}

	void SceneSerializer::Serialize(const std::filesystem::path& filepath)
	{
		using namespace Serializer;
		std::string text;

		nlohmann::ordered_json jsonData;
		jsonData["Entities"] = {};
		std::vector<nlohmann::ordered_json> jsonEntities;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				auto& tag = entity.GetComponent<TagComponent>().Tag;

			nlohmann::ordered_json e;
			e["ID"] = (uint64_t)entity.GetUUID();
			e["Tag"] = tag;

			if (entity.HasComponent<ParentComponent>())
			{
				auto& pc = entity.GetComponent<ParentComponent>();
				e["ParentComponent"] = {
					{ "ParentName", pc.ParentName },
					{ "ParentID", pc.ParentID },
					{ "Offset", { pc.Offset.x, pc.Offset.y, pc.Offset.z } }
				};
			}

			if (entity.HasComponent<TransformComponent>())
			{
				auto& tc = entity.GetComponent<TransformComponent>();
				e["TransformComponent"] = {
					{ "Translation", { tc.Translation.x, tc.Translation.y, tc.Translation.z } },
					{ "Rotation", { tc.Rotation.x, tc.Rotation.y, tc.Rotation.z } },
					{ "Scale", { tc.Scale.x, tc.Scale.y, tc.Scale.z } },
				};
			}

			if (entity.HasComponent<ScriptComponent>())
			{
				auto& sc = entity.GetComponent<ScriptComponent>();
				e["ScriptComponent"] = {
					{ "ClassName", sc.ClassName },
				};
			}

			if (entity.HasComponent<SpriteRendererComponent>())
			{
				auto& src = entity.GetComponent<SpriteRendererComponent>();
				if (src.Texture)
				{
					e["SpriteRendererComponent"] = {
						{ "Color", Encode(src.Color) },
						{ "TextureHandle", (uint64_t)src.Texture },
						{ "Scale", src.Scale }
					};
				}
				else
				{
					e["SpriteRendererComponent"] = {
						{ "Color", { src.Color.r, src.Color.g, src.Color.b, src.Color.a } }
					};
				}
			}

			if (entity.HasComponent<EditorRendererComponent>())
			{
				auto& erc = entity.GetComponent<EditorRendererComponent>();
				if (erc.Texture)
				{
					e["EditorRendererComponent"] = {
						{ "Color", { erc.Color.r, erc.Color.g, erc.Color.b, erc.Color.a } },
						{ "TextureHandle", (uint64_t)erc.Texture },
					};
				}
				else
				{
					e["EditorRendererComponent"] = {
						{ "Color", { erc.Color.r, erc.Color.g, erc.Color.b, erc.Color.a } }
					};
				}
			}

			if (entity.HasComponent<CircleRendererComponent>())
			{
				auto& crc = entity.GetComponent<CircleRendererComponent>();
				e["CircleRendererComponent"] = {
					{ "Color", { crc.Color.r, crc.Color.g, crc.Color.b, crc.Color.a } },
					{ "Thickness", crc.Thickness },
					{ "Fade", crc.Fade }
				};
			}

			if (entity.HasComponent<MeshComponent>())
			{
				auto& mc = entity.GetComponent<MeshComponent>();
				e["MeshComponent"] = {
					{ "MeshHandle", (uint64_t)mc.MeshHandle },
					{ "MaterialIndex", mc.MaterialIndex }
				};
			}

			if (entity.HasComponent<PointLightComponent>())
			{
				auto& plc = entity.GetComponent<PointLightComponent>();
				e["PointLightComponent"] = {
					{ "Color", { plc.Color.r, plc.Color.g, plc.Color.b } },
					{ "AmbientIntensity", plc.AmbientIntensity }
				};
			}

			if (entity.HasComponent<DirectionalLightComponent>())
			{
				auto& dlc = entity.GetComponent<DirectionalLightComponent>();
				e["DirectionalLightComponent"] = {
					{ "Color", { dlc.Color.r, dlc.Color.g, dlc.Color.b } },
					{ "AmbientIntensity", dlc.AmbientIntensity }
				};
			}

			if (entity.HasComponent<CameraComponent>())
			{
				auto& cc = entity.GetComponent<CameraComponent>();
				auto& camera = cc.Camera;
				SceneCamera::ProjectionType projType = camera.GetProjectionType();
				if (projType == SceneCamera::ProjectionType::Orthographic)
				{
					e["CameraComponent"] = {
						{ "ProjectionType", ProjectionTypeToString(projType) },
						{ "Size", camera.GetOrthographicSize() },
						{ "Near", camera.GetOrthographicNearClip() },
						{ "Far", camera.GetOrthographicFarClip() },
						{ "Primary", cc.Primary },
						{ "FixedAspectRatio", cc.FixedAspectRatio }
					};
				}
				else
				{
					e["CameraComponent"] = {
						{ "ProjectionType", ProjectionTypeToString(projType) },
						{ "VerticalFOV", camera.GetPerspectiveVertivalFOV() },
						{ "Near", camera.GetPerspectiveNearClip() },
						{ "Far", camera.GetPerspectiveFarClip() },
						{ "Primary", cc.Primary },
						{ "FixedAspectRatio", cc.FixedAspectRatio }
					};
				}
			}

			if (entity.HasComponent<RigidBody2DComponent>())
			{
				auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
				e["RigidBody2DComponent"] = {
					{ "Type", BodyTypeToString(rb2d.Type) },
					{ "FixedRotation", rb2d.FixedRotation }
				};
			}

			if (entity.HasComponent<BoxColider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxColider2DComponent>();
				e["BoxColider2DComponent"] = {
					{ "Offset", { bc2d.Offset.x, bc2d.Offset.y } },
					{ "Size", { bc2d.Size.x, bc2d.Size.y } },
					{ "Density", bc2d.Density },
					{ "Friction", bc2d.Friction },
					{ "Restitution", bc2d.Restitution },
					{ "RestitutionThreshold", bc2d.RestitutionThreshold }
				};
			}

			if (entity.HasComponent<CircleColider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleColider2DComponent>();
				e["CircleColider2DComponent"] = {
					{ "Offset", { cc2d.Offset.x, cc2d.Offset.y } },
					{ "Radius",cc2d.Radius },
					{ "Density", cc2d.Density },
					{ "Friction", cc2d.Friction },
					{ "Restitution", cc2d.Restitution },
					{ "RestitutionThreshold", cc2d.RestitutionThreshold }
				};
			}

			if (entity.HasComponent<TextComponent>())
			{
				auto& tc = entity.GetComponent<TextComponent>();
				e["TextComponent"] = {
					{ "Text", tc.Text },
					{ "Color", { tc.Color.r, tc.Color.g, tc.Color.b, tc.Color.a } },
					{ "Kerning",tc.Kerning },
					{ "LineSpacing", tc.LineSpacing }
				};
			}

			jsonEntities.push_back(e);
		});

		for (int i = 0; i < jsonEntities.size(); i++)
			jsonData["Entities"] += jsonEntities[i];

		std::ofstream jsonOut(filepath);
		jsonOut << std::setw(4) << jsonData;
	}

	void SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		using namespace Serializer;

		std::ifstream file(filepath);
		nlohmann::json j;
		file >> j;
		if (j.contains("Entities"))
		{
			auto& entities = j["Entities"];
			for (auto& [key, value] : entities.items())
			{
				uint64_t uuid = value["ID"].get<uint64_t>();
				std::string tag;
				if (value.contains("TagComponent"))
					tag = value["TagComponent"]["Tag"].get<std::string>();
				else if (value.contains("Tag"))
					tag = value["Tag"].get<std::string>();

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

				if (value.contains("ScriptComponent"))
				{
					auto& jsonScript = value["ScriptComponent"];
					auto& scriptComponent = entity.AddComponent<ScriptComponent>();

					scriptComponent.ClassName = jsonScript["ClassName"].get<std::string>();
				}

				if (value.contains("SpriteRendererComponent"))
				{
					auto& jsonSpriteRenderer = value["SpriteRendererComponent"];
					auto& src = entity.AddComponent<SpriteRendererComponent>();

					Decode(ConvertFloat4(jsonSpriteRenderer["Color"]), src.Color);
					if (jsonSpriteRenderer.contains("TextureHandle"))
					{
						src.Texture = jsonSpriteRenderer["TextureHandle"].get<uint64_t>();
						src.Scale = jsonSpriteRenderer["Scale"].get<float>();
					}
				}

				if (value.contains("EditorRendererComponent"))
				{
					auto& jsonEditotRenderer = value["EditorRendererComponent"];
					auto& erc = entity.AddComponent<EditorRendererComponent>();

					Decode(ConvertFloat4(jsonEditotRenderer["Color"]), erc.Color);
					if (jsonEditotRenderer.contains("TextureHandle"))
					{
						erc.Texture = jsonEditotRenderer["TextureHandle"].get<uint64_t>();
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

					mc.MeshHandle = jsonMeshComponent["MeshHandle"].get<uint64_t>();
					mc.MaterialIndex = jsonMeshComponent["MaterialIndex"].get<int>();
				}

				if (value.contains("PointLightComponent"))
				{
					auto& jsonPointlightComponent = value["PointLightComponent"];
					auto& pl = entity.AddComponent<PointLightComponent>();

					Decode(ConvertFloat3(jsonPointlightComponent["Color"]), pl.Color);
					pl.AmbientIntensity = jsonPointlightComponent["AmbientIntensity"].get<float>();
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
					rgbd2DComponent.FixedRotation = jsonRGBD2D["FixedRotation"].get<bool>();
				}

				if (value.contains("BoxColider2DComponent"))
				{
					auto& jsonBC2D = value["BoxColider2DComponent"];
					auto& bc2dComponent = entity.AddComponent<BoxColider2DComponent>();

					Decode(ConvertFloat2(jsonBC2D["Size"]), bc2dComponent.Size);
					Decode(ConvertFloat2(jsonBC2D["Offset"]), bc2dComponent.Offset);
					bc2dComponent.Density = jsonBC2D["Density"].get<float>();
					bc2dComponent.Friction = jsonBC2D["Friction"].get<float>();
					bc2dComponent.Restitution = jsonBC2D["Restitution"].get<float>();
					bc2dComponent.RestitutionThreshold = jsonBC2D["RestitutionThreshold"].get<float>();
				}

				if (value.contains("CircleColider2DComponent"))
				{
					auto& jsonCC2D = value["CircleColider2DComponent"];
					auto& cc2dComponent = entity.AddComponent<CircleColider2DComponent>();

					Decode(ConvertFloat2(jsonCC2D["Offset"]), cc2dComponent.Offset);
					cc2dComponent.Radius = jsonCC2D["Radius"].get<float>();
					cc2dComponent.Density = jsonCC2D["Density"].get<float>();
					cc2dComponent.Friction = jsonCC2D["Friction"].get<float>();
					cc2dComponent.Restitution = jsonCC2D["Restitution"].get<float>();
					cc2dComponent.RestitutionThreshold = jsonCC2D["RestitutionThreshold"].get<float>();
				}

				if (value.contains("TextComponent"))
				{
					auto& jsonTc = value["TextComponent"];
					auto& tc = entity.AddComponent<TextComponent>();

					tc.Text = jsonTc["Text"].get<std::string>();
					Decode(ConvertFloat4(jsonTc["Color"]), tc.Color);
					tc.Kerning = jsonTc["Kerning"].get<float>();
					tc.LineSpacing = jsonTc["LineSpacing"].get<float>();
				}

			}
		}
	}

}