#include "oepch.h"
#include "Scene.h"

#include "OpenEngine/Core/KeyCodes.h"
#include "OpenEngine/Core/Input.h"
#include "OpenEngine/Core/Application.h"
#include "OpenEngine/Scene/Entity.h"
#include "OpenEngine/Scene/Components.h"
#include "OpenEngine/Scene/ScriptableEntity.h"
#include "OpenEngine/Scripting/ScriptEngine.h"
#include "OpenEngine/Renderer/Renderer.h"
#include "OpenEngine/Renderer/Mesh.h"
#include "OpenEngine/Asset/AssetManager.h"
#include "OpenEngine/Project/Project.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

#include <glm/glm.hpp>

namespace OpenEngine {

	static b2BodyType RigidBody2DTypeToBox2DBody(RigidBody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case RigidBody2DComponent::BodyType::Static: return b2BodyType::b2_staticBody;
			case RigidBody2DComponent::BodyType::Dynamic: return b2BodyType::b2_dynamicBody;
			case RigidBody2DComponent::BodyType::Kinematic:	return b2BodyType::b2_kinematicBody;
		}

		return b2BodyType::b2_staticBody;
	}

	Scene::Scene()
	{
		Material& defaultMaterial = m_Materials.emplace_back();
		m_Filepath = "UntitledScene.openengine";
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
		auto view = m_Registry.view<ParentComponent>();
		for (auto e : view)
		{
			Entity child = { e, this };
			if (child.GetComponent<ParentComponent>().ParentID == entity.GetUUID())
				m_Registry.destroy(child);
		}
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

		if (other.HasComponent<EditorRendererComponent>())
		{
			auto& src = newEntity.AddComponent<EditorRendererComponent>();
			src.Color = other.GetComponent<EditorRendererComponent>().Color;
			src.Texture = other.GetComponent<EditorRendererComponent>().Texture;
		}

		if (other.HasComponent<CircleRendererComponent>())
		{
			auto& crc = newEntity.AddComponent<CircleRendererComponent>();
			crc.Color = other.GetComponent<CircleRendererComponent>().Color;
			crc.Thickness = other.GetComponent<CircleRendererComponent>().Thickness;
			crc.Fade = other.GetComponent<CircleRendererComponent>().Fade;
		}

		if (other.HasComponent<MeshComponent>())
		{
			auto& mc = newEntity.AddComponent<MeshComponent>();
			mc.MeshHandle = other.GetComponent<MeshComponent>().MeshHandle;
			mc.MaterialIndex = other.GetComponent<MeshComponent>().MaterialIndex;
		}

		if (other.HasComponent<CameraComponent>())
		{
			auto& cc = newEntity.AddComponent<CameraComponent>();
			cc.Camera = other.GetComponent<CameraComponent>().Camera;
			cc.FixedAspectRatio = other.GetComponent<CameraComponent>().FixedAspectRatio;
			cc.Primary = other.GetComponent<CameraComponent>().Primary;
		}

		if (other.HasComponent<RigidBody2DComponent>())
		{
			auto& rb2d = newEntity.AddComponent<RigidBody2DComponent>();
			rb2d.Type = other.GetComponent<RigidBody2DComponent>().Type;
			rb2d.FixedRotation = other.GetComponent<RigidBody2DComponent>().FixedRotation;
		}

		if (other.HasComponent<BoxColider2DComponent>())
		{
			auto& bc2d = newEntity.AddComponent<BoxColider2DComponent>();
			bc2d.Offset = other.GetComponent<BoxColider2DComponent>().Offset;
			bc2d.Size = other.GetComponent<BoxColider2DComponent>().Size;
			bc2d.Density = other.GetComponent<BoxColider2DComponent>().Density;
			bc2d.Friction = other.GetComponent<BoxColider2DComponent>().Friction;
			bc2d.Restitution = other.GetComponent<BoxColider2DComponent>().Restitution;
			bc2d.RestitutionThreshold = other.GetComponent<BoxColider2DComponent>().RestitutionThreshold;
		}

		if (other.HasComponent<CircleColider2DComponent>())
		{
			auto& cc2d = newEntity.AddComponent<CircleColider2DComponent>();
			cc2d.Offset = other.GetComponent<CircleColider2DComponent>().Offset;
			cc2d.Radius = other.GetComponent<CircleColider2DComponent>().Radius;
			cc2d.Density = other.GetComponent<CircleColider2DComponent>().Density;
			cc2d.Friction = other.GetComponent<CircleColider2DComponent>().Friction;
			cc2d.Restitution = other.GetComponent<CircleColider2DComponent>().Restitution;
			cc2d.RestitutionThreshold = other.GetComponent<CircleColider2DComponent>().RestitutionThreshold;
		}

		if (other.HasComponent<TextComponent>())
		{
			auto& otherTc = other.GetComponent<TextComponent>();
			auto& tc = newEntity.AddComponent<TextComponent>();
			tc = otherTc;
		}

		if (other.HasComponent<ScriptComponent>())
		{
			auto& otherSc = other.GetComponent<ScriptComponent>();
			auto& sc = newEntity.AddComponent<ScriptComponent>();
			sc = otherSc;
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
				return { e, this };
		}

		return{};
	}

	Entity Scene::PrimaryCamera()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto e : view)
		{
			Entity camera = { e, this };
			if (camera.GetComponent<CameraComponent>().Primary)
				return camera;
		}

		OE_CORE_ERROR("No primary camera found. One has been added by OpenEngine to prevent a program crash! Ensure to add a primary camera to your scene.");
		Entity camera = CreateEntity("PrimaryCamera");
		camera.AddComponent<CameraComponent>();
		return camera;
	}

	void Scene::OnEditorStart()
	{
		
	}

	void Scene::OnRuntimeStart()
	{
		OnPhysics2DStart();
		ScriptEngine::OnRuntimeStart(this);
		
		// Scripting
		{
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnCreateEntity(entity);
			}
		}

		m_IsRunning = true;			
	}

	void Scene::OnRuntimeStop()
	{
		OnPhysics2DStop();
		ScriptEngine::OnRuntimeStop();

		m_IsRunning = false;
	}

	void Scene::OnPhysics2DStart()
	{
		b2Vec2 gravity = { 0.0f, -9.8f };
		m_PhysicsWorld = new b2World(gravity);
		auto view = m_Registry.view<RigidBody2DComponent>();

		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = RigidBody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position = { transform.Translation.x, transform.Translation.y };
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxColider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxColider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleColider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleColider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = transform.Scale.x * cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
		/*delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;*/
	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Update parent entities

		OE_PROFILE_FUNCTION();

		auto children = GetEntitiesWithParents();
		for (auto childID : children)
		{
			Entity child = { childID, this };
			Entity parent = GetEntityByUUID(child.GetComponent<ParentComponent>().ParentID);
			glm::vec3 forwardDirection = glm::rotate(glm::quat(glm::vec3(parent.GetComponent<TransformComponent>().Rotation.x, parent.GetComponent<TransformComponent>().Rotation.y, 0.0f)), glm::vec3(0.0f, 0.0f, -1.0f));
			if (child.GetTranslation() != parent.GetTranslation() + child.GetComponent<ParentComponent>().Offset)
			{
				glm::vec3 parentPosition = parent.GetTranslation();
				glm::vec3 offset = child.GetComponent<ParentComponent>().Offset; 
				glm::vec3 position = parentPosition + forwardDirection;
				child.GetComponent<TransformComponent>().Translation = position;
				OE_CORE_INFO("{0}, {1}, {2}", forwardDirection.x, forwardDirection.y, forwardDirection.z);
			}
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		OE_PROFILE_FUNCTION();
		OE_PERF_FUNC();

		OnUpdate(ts);

		{
			auto directionalLightView = m_Registry.view<DirectionalLightComponent>();
			auto pointLightView = m_Registry.view<PointLightComponent>();
			std::vector<Entity> lights;
			for (auto entity : pointLightView)
			{
				lights.push_back({ entity, this });
			}

			Renderer::BeginScene(camera, lights);

			/*auto view = m_Registry.view<TransformComponent, MeshComponent>();
			for (auto entity : view)
			{
				auto [transform, meshComponent] = view.get<TransformComponent, MeshComponent>(entity);
				int index = meshComponent.MaterialIndex > m_Materials.size() - 1 ? 0 : meshComponent.MaterialIndex;
				Material material = m_Materials[index];
				
				if (meshComponent.Filepath != "null")
				{
					bool exists = false;
					int index = 0;
					
					for (int i = 0; i < m_Meshes.size(); i++)
					{
						if (m_Meshes[i].GetFilePath() == meshComponent.Filepath)
						{
							index = i;
							exists = true;
						}
					}

					if (!exists)
					{
						m_Meshes.emplace_back(meshComponent.Filepath);
						index = (int)m_Meshes.size() - 1;
					}

					Renderer::Submit(m_Meshes[index], transform.GetTransform(), material, (int)entity);
				}
			}*/

			Renderer::EndScene(m_Meshes);
		}

		{
			Renderer2D::BeginScene(camera);

			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

					if (Project::GetActive()->GetAssetManager()->IsAssetHandleValid(sprite.Texture))
					{
						Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(sprite.Texture);
						Renderer2D::DrawQuad(transform.GetTransform(), texture, sprite.Color, sprite.Scale, (int)entity);
					}
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
				auto view = m_Registry.view<TransformComponent, TextComponent>();
				for (auto entity : view)
				{
					auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);
					if (!text.ScreenSpace)
					{
						Renderer2D::TextParams textParams;
						textParams.Color = text.Color;
						textParams.Kerning = text.Kerning;
						textParams.LineSpacing = text.LineSpacing;
						Renderer2D::DrawString(text.Text, text.TextFont, transform.GetTransform(), textParams, (int)entity);
					}
				}
			}

			{
				auto view = m_Registry.view<TransformComponent, EditorRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, sprite] = view.get<TransformComponent, EditorRendererComponent>(entity);

					if (Project::GetActive()->GetAssetManager()->IsAssetHandleValid(sprite.Texture))
					{
						Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(sprite.Texture);
						Renderer2D::DrawQuad(transform.GetTransform(), texture, sprite.Color, 1.0f, (int)entity);
					}
					else
						Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color, (int)entity);
				}
			}

			// Grid
			{
				if (m_GridEnabled)
				{
					for (int i = 0; i <= m_GridSize * 2; i++)
					{
						Renderer2D::DrawLine({ -m_GridSize, 0, i - m_GridSize }, { m_GridSize, 0, i - m_GridSize }, { 0.8f, 0.8f, 0.9f, 1.0f });
						Renderer2D::DrawLine({ i - m_GridSize, 0, -m_GridSize }, { i - m_GridSize, 0, m_GridSize }, { 0.8f, 0.8f, 0.9f, 1.0f });
					}
				}
			}

			Renderer2D::EndScene();
		}

		// Screen space rendering
		{
			OrthographicCamera camera(-16.0f, 16.0f, -9.0f, 9.0f);

			Renderer2D::BeginScene(camera);

			{
				auto view = m_Registry.view<TransformComponent, TextComponent>();
				for (auto entity : view)
				{
					auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);
					if (text.ScreenSpace)
					{
						Renderer2D::TextParams textParams;
						textParams.Color = text.Color;
						textParams.Kerning = text.Kerning;
						textParams.LineSpacing = text.LineSpacing;
						Renderer2D::DrawString(text.Text, text.TextFont, transform.GetTransform(), textParams);
					}
				}
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		OnUpdate(ts);

		// Update Scripts
		{
			// C# Scripts
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnUpdateEntity(entity, ts);
			}

			// C++ Scripts
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
			const int32 velocityInterations = 6;
			const int32 positionInterations = 2;

			m_PhysicsWorld->Step(ts, velocityInterations, positionInterations);
			auto view = m_Registry.view<RigidBody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = body->GetAngle();
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
			auto pointLightView = m_Registry.view<PointLightComponent>();
			std::vector<Entity> lights;
			for (auto entity : pointLightView)
			{
				lights.push_back({ entity, this });
			}

			Renderer::BeginScene(*mainCamera, cameraTransform, lights);

			/*{
				auto view = m_Registry.view<TransformComponent, MeshComponent>();
				for (auto entity : view)
				{
					auto [transform, meshComponent] = view.get<TransformComponent, MeshComponent>(entity);
					int index = meshComponent.MaterialIndex > m_Materials.size() - 1 ? 0 : meshComponent.MaterialIndex;
					Material material = m_Materials[index];

					if (meshComponent.Filepath != "null")
					{
						bool exists = false;
						int index = 0;

						for (int i = 0; i < m_Meshes.size(); i++)
						{
							if (m_Meshes[i].GetFilePath() == meshComponent.Filepath)
							{
								index = i;
								exists = true;
							}
						}

						if (!exists)
						{
							m_Meshes.emplace_back(meshComponent.Filepath);
							index = (int)m_Meshes.size() - 1;
						}

						Renderer::Submit(m_Meshes[index], transform.GetTransform(), material, (int)entity);
					}
				}
			}*/

			Renderer::EndScene(m_Meshes);

			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

			{
				auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

					if (Project::GetActive()->GetAssetManager()->IsAssetHandleValid(sprite.Texture))
					{
						Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(sprite.Texture);
						Renderer2D::DrawQuad(transform.GetTransform(), texture, sprite.Color, sprite.Scale, (int)entity);
					}
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
				auto view = m_Registry.view<TransformComponent, TextComponent>();
				for (auto entity : view)
				{
					auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);
					if (!text.ScreenSpace)
					{
						Renderer2D::TextParams textParams;
						textParams.Color = text.Color;
						textParams.Kerning = text.Kerning;
						textParams.LineSpacing = text.LineSpacing;
						Renderer2D::DrawString(text.Text, text.TextFont, transform.GetTransform(), textParams, (int)entity);
					}
				}
			}

			Renderer2D::EndScene();
		}

		// Screen space rendering
		{
			OrthographicCamera camera(-16.0f, 16.0f, -9.0f, 9.0f);

			Renderer2D::BeginScene(camera);

			{
				auto view = m_Registry.view<TransformComponent, TextComponent>();
				for (auto entity : view)
				{
					auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);
					if (text.ScreenSpace)
					{
						Renderer2D::TextParams textParams;
						textParams.Color = text.Color;
						textParams.Kerning = text.Kerning;
						textParams.LineSpacing = text.LineSpacing;
						Renderer2D::DrawString(text.Text, text.TextFont, transform.GetTransform(), textParams);
					}
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
		other->m_Materials = m_Materials;

		m_Registry.each([&](auto entityID)
		{
				// TODO: Somehow link Duplicate to this
				//       The problem with Duplicate is that it only duplicates the entity in the current scene
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

				if (entity.HasComponent<MeshComponent>())
				{
					auto& mesh = newEntity.AddComponent<MeshComponent>();
					mesh.MeshHandle = entity.GetComponent<MeshComponent>().MeshHandle;
					mesh.MaterialIndex = entity.GetComponent<MeshComponent>().MaterialIndex;
				}

				if (entity.HasComponent<PointLightComponent>())
				{
					auto& pl = newEntity.AddComponent<PointLightComponent>();
					pl.Color = entity.GetComponent<PointLightComponent>().Color;
					pl.AmbientIntensity = entity.GetComponent<PointLightComponent>().AmbientIntensity;
				}
				
				if (entity.HasComponent<DirectionalLightComponent>())
				{
					auto& drl = newEntity.AddComponent<DirectionalLightComponent>();
					drl.Color = entity.GetComponent<DirectionalLightComponent>().Color;
					drl.AmbientIntensity = entity.GetComponent<DirectionalLightComponent>().AmbientIntensity;
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

				if (entity.HasComponent<RigidBody2DComponent>())
				{
					auto oldRb2d = entity.GetComponent<RigidBody2DComponent>();
					auto& rb2d = newEntity.AddComponent<RigidBody2DComponent>();
					rb2d = oldRb2d;
				}

				if (entity.HasComponent<BoxColider2DComponent>())
				{
					auto oldBc2d = entity.GetComponent<BoxColider2DComponent>();
					auto& bc2d = newEntity.AddComponent<BoxColider2DComponent>();
					bc2d = oldBc2d;
				}

				if (entity.HasComponent<CircleColider2DComponent>())
				{
					auto oldCc2d = entity.GetComponent<CircleColider2DComponent>();
					auto& cc2d = newEntity.AddComponent<CircleColider2DComponent>();
					cc2d = oldCc2d;
				}

				if (entity.HasComponent<TextComponent>())
				{
					auto oldTc = entity.GetComponent<TextComponent>();
					auto& tc = newEntity.AddComponent<TextComponent>();
					tc = oldTc;
				}

				if (entity.HasComponent<ScriptComponent>())
				{
					auto oldSc = entity.GetComponent<ScriptComponent>();
					auto& sc = newEntity.AddComponent<ScriptComponent>();
					sc = oldSc;
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