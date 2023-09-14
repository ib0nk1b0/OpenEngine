#pragma once

#include "OpenEngine/Core/UUID.h"
#include "OpenEngine/Renderer/Renderer2D.h"
#include "OpenEngine/Scene/SceneCamera.h"
#include "OpenEngine/Renderer/Renderer.h"

#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace OpenEngine {
	
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
				/*glm::rotate(glm::mat4(1.0f), Rotation.x, { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });*/

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct ParentComponent
	{
		std::string ParentName;
		uint64_t ParentID{ 0 };

		glm::vec3 Offset = { 0.0f, 0.0f, 0.0f };

		ParentComponent() = default;
		ParentComponent(const ParentComponent&) = default;
		ParentComponent(std::string name, UUID parentID)
			: ParentName(name), ParentID(parentID) {}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float Scale = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
		SpriteRendererComponent(const Ref<Texture2D>& texture, float scale = 1.0f)
			: Texture(texture), Scale(scale) {}
	};

	struct CircleRendererComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
		CircleRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct EditorRendererComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;

		EditorRendererComponent() = default;
		EditorRendererComponent(const EditorRendererComponent&) = default;
		EditorRendererComponent(const glm::vec4& color)
			: Color(color) {}
		EditorRendererComponent(const Ref<Texture2D>& texture)
			: Texture(texture) {}
	};

	struct MeshComponent
	{
		//std::filesystem::path Filepath;
		std::string Filepath = "null";
		int MaterialIndex = 0;
		MeshInstance* Mesh;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
	};

	struct DirectionalLightComponent
	{
		glm::vec3 Color{ 1.0f };
		float AmbientIntensity = 0.1f;
		float SpecularStrength = 0.5f;

		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
	};

	struct PointLightComponent
	{
		glm::vec3 Color{ 1.0f };
		float AmbientIntensity = 0.1f;
		float SpecularStrength = 0.5f;

		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	enum class BodyType { Static = 0, Dynamic };

	struct RigidBody2DComponent
	{
		BodyType Type = BodyType::Static;

		RigidBody2DComponent() = default;
		RigidBody2DComponent(const RigidBody2DComponent&) = default;

		RigidBody2DComponent(const BodyType& Type)
			: Type(Type)
		{
		}
	};

	struct BoxColider2DComponent
	{
		glm::vec3 Size = { 1.0f, 1.0f, 1.0f };
		glm::vec3 Offset = { 0.0f, 0.0f, 0.0f };

		BoxColider2DComponent() = default;
		BoxColider2DComponent(const BoxColider2DComponent&) = default;
		BoxColider2DComponent(const glm::vec3& size, const glm::vec3& offset)
			: Size(size), Offset(offset) {}
	};

	class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

}
