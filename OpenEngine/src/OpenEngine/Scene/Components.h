#pragma once

#include "OpenEngine/Core/UUID.h"
#include "OpenEngine/Renderer/Renderer2D.h"
#include "OpenEngine/Scene/SceneCamera.h"
#include "OpenEngine/Renderer/Renderer.h"
#include "OpenEngine/Asset/Asset.h"

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
		uint64_t ParentID{ 0 };
		std::vector<uint64_t> Children;

		glm::vec3 Offset = { 0.0f, 0.0f, 0.0f };

		ParentComponent() = default;
		ParentComponent(const ParentComponent&) = default;
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		AssetHandle Texture = 0;
		float Scale = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
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
		AssetHandle Texture = 0;

		EditorRendererComponent() = default;
		EditorRendererComponent(const EditorRendererComponent&) = default;
		EditorRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct MeshComponent
	{
		AssetHandle MeshHandle = 0;
		int MaterialIndex = 0;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
	};

	struct DirectionalLightComponent
	{
		glm::vec3 Color{ 1.0f };
		float AmbientIntensity = 0.1f;

		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
	};

	struct PointLightComponent
	{
		glm::vec3 Color{ 1.0f };
		float AmbientIntensity = 0.1f;

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


	struct RigidBody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		void* RuntimeBody = nullptr;

		RigidBody2DComponent() = default;
		RigidBody2DComponent(const RigidBody2DComponent&) = default;
	};

	struct BoxColider2DComponent
	{
		glm::vec2 Size = { 0.5f, 0.5f };
		glm::vec2 Offset = { 0.0f, 0.0f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;

		BoxColider2DComponent() = default;
		BoxColider2DComponent(const BoxColider2DComponent&) = default;
	};

	struct CircleColider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;

		CircleColider2DComponent() = default;
		CircleColider2DComponent(const CircleColider2DComponent&) = default;
	};

	struct TextComponent
	{
		std::string Text;
		Ref<Font> TextFont = Font::GetDefault();
		glm::vec4 Color{ 1.0f };

		float Kerning = 0.0f;
		float LineSpacing = 0.0f;
		float MaxWidth = 100.0f;

		bool ScreenSpace = false;

		TextComponent() = default;
		TextComponent(const TextComponent&) = default;
	};

	struct ScriptComponent
	{
		std::string ClassName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
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
