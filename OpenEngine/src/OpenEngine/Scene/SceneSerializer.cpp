#include "oepch.h"
#include "SceneSerializer.h"

#include "OpenEngine/Serialization/Serializer.h"
#include "OpenEngine/Scene/Entity.h"

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
		case RigidBody2DComponent::BodyType::Static:
			return "Static";
			break;
		case RigidBody2DComponent::BodyType::Dynamic:
			return "Dynamic";
			break;
		}

		return "Static";
	}

	static RigidBody2DComponent::BodyType BodyTypeFromString(const std::string& type)
	{
		if (type == "Static") return RigidBody2DComponent::BodyType::Static;
		if (type == "Dynamic") return RigidBody2DComponent::BodyType::Dynamic;
		return RigidBody2DComponent::BodyType::Static;
	}

	void SceneSerializer::Serialize(const std::string& filepath)
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

			if (entity.HasComponent<SpriteRendererComponent>())
			{
				auto& src = entity.GetComponent<SpriteRendererComponent>();
				if (src.Texture)
				{
					e["SpriteRendererComponent"] = {
						{ "Color", Encode(src.Color) },
						{ "Texture", Utils::FormatFilepath(src.Texture->GetFilePath()) },
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
						{ "Texture", Utils::FormatFilepath(erc.Texture->GetFilePath()) },
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
					{ "Filepath", Utils::FormatFilepath(mc.Filepath) },
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
					{ "Type", BodyTypeToString(rb2d.Type) }
				};
			}

			if (entity.HasComponent<BoxColider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxColider2DComponent>();
				e["BoxColider2DComponent"] = {
					{ "Offset", Encode(bc2d.Offset) },
					{ "Size", Encode(bc2d.Size) }
				};
			}

			jsonEntities.push_back(e);
		});

		for (int i = 0; i < jsonEntities.size(); i++)
			jsonData["Entities"] += jsonEntities[i];

		std::ofstream jsonOut(filepath);
#ifdef OE_DIST
		jsonOut << jsonData;
#else
		jsonOut << std::setw(4) << jsonData;
#endif
	}

	void SceneSerializer::Deserialize(const std::string& filepath)
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

					mc.Filepath = jsonMeshComponent["Filepath"].get<std::string>();
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
				}

				if (value.contains("BoxColider2DComponent"))
				{
					auto& jsonBC2D = value["BoxColider2DComponent"];
					auto& bc2dComponent = entity.AddComponent<BoxColider2DComponent>();

					Decode(ConvertFloat2(jsonBC2D["Size"]), bc2dComponent.Size);
					Decode(ConvertFloat2(jsonBC2D["Offset"]), bc2dComponent.Offset);
				}

			}
		}
	}

}