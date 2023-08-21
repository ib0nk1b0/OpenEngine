#include "oepch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "ScriptableEntity.h"
#include "OpenEngine/Renderer/Renderer.h"
#include "OpenEngine/Physics/Physics.h"	
#include "OpenEngine/Core/KeyCodes.h"
#include "OpenEngine/Core/Input.h"

#include <glm/glm.hpp>

namespace OpenEngine {

#if Dungeon
	struct Keybinds
	{
		int UP = Key::W;
		int DOWN = Key::S;
		int LEFT = Key::A;
		int RIGHT = Key::D;
	};

	class PlayerScript : public ScriptableEntity
	{
	public:
		void OnCreate()
		{

		}

		void OnUpdate(Timestep ts)
		{
			UpdateMovement(ts);
		}

		void UpdateMovement(Timestep ts)
		{
			auto& translation = GetComponent<TransformComponent>().Translation;
			if (Input::IsKeyPressed(m_Keybinds.UP))
				translation.y += m_Speed * ts;
			if (Input::IsKeyPressed(m_Keybinds.DOWN))
				translation.y -= m_Speed * ts;
			if (Input::IsKeyPressed(m_Keybinds.LEFT))
				translation.x -= m_Speed * ts;
			if (Input::IsKeyPressed(m_Keybinds.RIGHT))
				translation.x += m_Speed * ts;
		}

		void OnDestroy()
		{

		}
	private:
		float m_Speed = 2.0f;

		Keybinds m_Keybinds;
	};

	class CameraScript : public ScriptableEntity
	{
	public:
		void OnUpdate(Timestep ts)
		{
			auto& translation = GetComponent<TransformComponent>().Translation;
			if (Input::IsKeyPressed(m_Keybinds.UP))
				translation.y += m_Speed * ts;
			if (Input::IsKeyPressed(m_Keybinds.DOWN))
				translation.y -= m_Speed * ts;
			if (Input::IsKeyPressed(m_Keybinds.LEFT))
				translation.x -= m_Speed * ts;
			if (Input::IsKeyPressed(m_Keybinds.RIGHT))
				translation.x += m_Speed * ts;
		}

	private:
		float m_Speed = 2.0f;

		Keybinds m_Keybinds;
	};
#endif
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<ParentComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	Entity Scene::DuplicateEntity(Entity other)
	{
		Entity newEntity = CreateEntity(other.GetComponent<TagComponent>().Tag);
		newEntity.GetComponent<TransformComponent>().Translation = other.GetComponent<TransformComponent>().Translation;
		newEntity.GetComponent<TransformComponent>().Rotation = other.GetComponent<TransformComponent>().Rotation;
		newEntity.GetComponent<TransformComponent>().Scale = other.GetComponent<TransformComponent>().Scale;

		if (other.HasComponent<SpriteRendererComponent>())
		{
			auto& src = newEntity.AddComponent<SpriteRendererComponent>();
			src.Color = other.GetComponent<SpriteRendererComponent>().Color;
			src.Texture = other.GetComponent<SpriteRendererComponent>().Texture;
			src.Scale = other.GetComponent<SpriteRendererComponent>().Scale;
		}

		if (other.HasComponent<CircleRendererComponent>())
		{
			auto& crc = newEntity.AddComponent<CircleRendererComponent>();
			crc.Color = other.GetComponent<CircleRendererComponent>().Color;
			crc.Thickness = other.GetComponent<CircleRendererComponent>().Thickness;
			crc.Fade = other.GetComponent<CircleRendererComponent>().Fade;
		}

		if (other.HasComponent<CameraComponent>())
		{
			auto& cc = newEntity.AddComponent<CameraComponent>();
			cc.Camera = other.GetComponent<CameraComponent>().Camera;
			cc.FixedAspectRatio = other.GetComponent<CameraComponent>().FixedAspectRatio;
			cc.Primary = other.GetComponent<CameraComponent>().Primary;
		}

		return newEntity;
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		Entity entity;

		auto view = m_Registry.view<IDComponent>();
		for (auto e : view)
		{
			auto idComponent = m_Registry.get<IDComponent>(e);
			if (idComponent.ID == uuid)
				entity = { e, this };
		}

		return entity;
	}

	Entity Scene::GetEntityByName(const std::string& name)
	{
		Entity entity;

		auto view = m_Registry.view<TagComponent>();
		for (auto e : view)
		{
			if (m_Registry.get<TagComponent>(e).Tag == name)
				entity = { e, this };
		}

		return entity;
	}

	void Scene::OnRuntimeStart()
	{
#if Dungeon
		auto player = GetEntityByName("Player");
		player.AddComponent<NativeScriptComponent>().Bind<PlayerScript>();
		auto camera = GetEntityByName("Orthographic Camera");
		camera.AddComponent<NativeScriptComponent>().Bind<CameraScript>();
#endif
	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Update parent entities
		auto view = m_Registry.view<ParentComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& parentComponent = entity.GetComponent<ParentComponent>();
			if (parentComponent.ParentName.empty())
				continue;
			auto& translation = entity.GetComponent<TransformComponent>().Translation;
			Entity parentEntity = GetEntityByUUID(parentComponent.ParentID);
			translation = parentEntity.GetTranslation() + parentComponent.Offset;
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		OnUpdate(ts);
		Renderer2D::BeginScene(camera);

		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				if (sprite.Texture)
					Renderer2D::DrawQuad(transform.GetTransform(), sprite.Texture, sprite.Color, sprite.Scale, (int)entity);
				else
					Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color, (int)entity);
			}
		}

		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		{
			auto view = m_Registry.view<TransformComponent, EditorRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, sprite] = view.get<TransformComponent, EditorRendererComponent>(entity);

				if (sprite.Texture)
					Renderer2D::DrawQuad(transform.GetTransform(), sprite.Texture, sprite.Color, 1.0f, (int)entity);
				else
					Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color, (int)entity);
			}
		}

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		OnUpdate(ts);

		// Update Scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				// TODO: Move to Scene::OnScenePlay
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(ts);
			});
		}

		// Init Physics
		{
			auto physics = Physics2D(9.8f);
			World2D world = World2D();
			physics.AddWorld(world);

			auto view = m_Registry.view<RigidBody2DComponent>();
			for (auto entityID : view)
			{
				Entity entity = Entity{ entityID, this };

			}
		}

		// Render
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto group = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto [camera, transform] = m_Registry.get<CameraComponent, TransformComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

					if (sprite.Texture)
						Renderer2D::DrawQuad(transform.GetTransform(), sprite.Texture, sprite.Color, sprite.Scale, (int)entity);
					else
						Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color, (int)entity);
				}
			}

			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}
			}

			Renderer2D::EndScene();
		}

	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

	void Scene::CopyTo(Ref<Scene> other)
	{
		m_Registry.each([&](auto entityID)
		{
				Entity entity{ entityID, this };
				Entity newEntity = other->CreateEntityWithUUID(entity.GetUUID(), entity.GetComponent<TagComponent>().Tag);

				newEntity.GetComponent<ParentComponent>().ParentName = entity.GetComponent<ParentComponent>().ParentName;
				newEntity.GetComponent<ParentComponent>().ParentID = entity.GetComponent<ParentComponent>().ParentID;
				newEntity.GetComponent<ParentComponent>().Offset = entity.GetComponent<ParentComponent>().Offset;

				newEntity.GetComponent<TransformComponent>().Translation = entity.GetComponent<TransformComponent>().Translation;
				newEntity.GetComponent<TransformComponent>().Rotation = entity.GetComponent<TransformComponent>().Rotation;
				newEntity.GetComponent<TransformComponent>().Scale = entity.GetComponent<TransformComponent>().Scale;

				if (entity.HasComponent<SpriteRendererComponent>())
				{
					auto& src = newEntity.AddComponent<SpriteRendererComponent>();
					src.Color = entity.GetComponent<SpriteRendererComponent>().Color;
					src.Texture = entity.GetComponent<SpriteRendererComponent>().Texture;
					src.Scale = entity.GetComponent<SpriteRendererComponent>().Scale;
				}

				if (entity.HasComponent<CircleRendererComponent>())
				{
					auto& crc = newEntity.AddComponent<CircleRendererComponent>();
					crc.Color = entity.GetComponent<CircleRendererComponent>().Color;
					crc.Thickness = entity.GetComponent<CircleRendererComponent>().Thickness;
					crc.Fade = entity.GetComponent<CircleRendererComponent>().Fade;
				}

				if (entity.HasComponent<CameraComponent>())
				{
					auto& cc = newEntity.AddComponent<CameraComponent>();
					cc.Camera = entity.GetComponent<CameraComponent>().Camera;
					cc.FixedAspectRatio = entity.GetComponent<CameraComponent>().FixedAspectRatio;
					cc.Primary = entity.GetComponent<CameraComponent>().Primary;
				}

				if (entity.HasComponent<NativeScriptComponent>())
				{
					auto oldNsc = entity.GetComponent<NativeScriptComponent>();
					auto& nsc = newEntity.AddComponent<NativeScriptComponent>();
					nsc = oldNsc;
				}
		});
	}

	std::vector<entt::entity> Scene::GetEntitiesWithParents()
	{
		std::vector<entt::entity> entities;
		auto view = m_Registry.view<ParentComponent>();
		for (auto entityID : view)
		{
			Entity entity = { entityID, this };
			if (entity.HasParent())
				entities.push_back(entityID);
		}

		return entities;
	}

}