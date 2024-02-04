#include "oepch.h"
#include "SceneHierarchy.h"

#include "ContentBrowserPanel.h"

#include "OpenEngine/Asset/AssetManager.h"
#include "OpenEngine/Asset/TextureImporter.h"
#include "OpenEngine/Scene/Components.h"
#include "OpenEngine/Utils/PlatformUtils.h"
#include "OpenEngine/ImGui/ImGuiExtended.h"
#include "OpenEngine/ImGui/ImGuiFonts.h"
#include "OpenEngine/Scripting/ScriptEngine.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

namespace OpenEngine {

	void SceneHierarchyPanel::Init()
	{
		m_DisplaySceneHierarchy = true;
		m_DisplayProperties = true;
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		if (m_DisplaySceneHierarchy)
		{
			ImGui::Begin("Scene Hierarchy");
			
			if (ImGui::IsMouseReleased(0) && ImGui::IsWindowHovered() && m_SelectionContext)
				m_SelectionContext = {};

			m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);
			});

			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::BeginMenu("Create"))
				{
					if (ImGui::MenuItem("Empty Entity"))
						m_Context->CreateEntity("Empty Entity");

					if (ImGui::MenuItem("Quad"))
					{
						Entity quad = m_Context->CreateEntity("Quad");
						quad.AddComponent<SpriteRendererComponent>();
					}

					if (ImGui::MenuItem("Text"))
					{
						Entity quad = m_Context->CreateEntity("Text");
						quad.AddComponent<TextComponent>().Text = "Text";
					}
					ImGui::Separator();
					
					if (ImGui::MenuItem("Mesh"))
					{
						Entity mesh = m_Context->CreateEntity("Mesh");
						mesh.AddComponent<MeshComponent>();
					}
					ImGui::Separator();
					if (ImGui::BeginMenu("Lights"))
					{
						if (ImGui::MenuItem("Point Light"))
						{
							Entity pointLight = m_Context->CreateEntity("Point Light");
							pointLight.AddComponent<PointLightComponent>();
						}
						if (ImGui::MenuItem("Directional Light"))
						{
							Entity directionalLight = m_Context->CreateEntity("Directional Light");
							directionalLight.AddComponent<DirectionalLightComponent>();
						}

						ImGui::EndMenu();
					}
					ImGui::Separator();
					
					ImGui::Separator();
					if (ImGui::BeginMenu("Camera"))
					{
						if (ImGui::MenuItem("Perspective"))
						{
							auto camera = m_Context->CreateEntity("Perspective Camera");
							camera.AddComponent<CameraComponent>().Camera.SetProjectionType(SceneCamera::ProjectionType::Perspective);
						}
						if (ImGui::MenuItem("Orthographic"))
						{
							auto camera = m_Context->CreateEntity("Orthographic Camera");
							camera.AddComponent<CameraComponent>().Camera.SetProjectionType(SceneCamera::ProjectionType::Orthographic);
						}

						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}

			ImGuiWindow* window = ImGui::GetCurrentWindow();

			if (ImGui::BeginDragDropTargetCustom(window->ContentRegionRect, window->ID))
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ITEM"))
				{
					AssetHandle handle = *(AssetHandle*)payload->Data;
					Entity child = m_Context->GetEntityByUUID(handle);
					if (child.HasParent())
						child.RemoveParent();
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::End();
		}

		if (!m_DisplayProperties) return;

		ImGui::Begin("Properties");
		if (m_SelectionContext)
			DrawComponents(m_SelectionContext);

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		if (!entity.HasParent())
		{
			ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 4.f));
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
			if (ImGui::IsItemClicked())
				m_SelectionContext = entity;

			if (ImGui::BeginDragDropSource())
			{
				AssetHandle handle = entity.GetUUID();
				ImGui::SetDragDropPayload("SCENE_HIERARCHY_ITEM", &handle, sizeof(handle));

				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ITEM"))
				{
					AssetHandle handle = *(AssetHandle*)payload->Data;
					Entity child = m_Context->GetEntityByUUID(handle);
					if (!child.HasChildren())
						child.SetParent(entity);
				}

				ImGui::EndDragDropTarget();
			}

			bool entityDeleted = false;
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Create Child"))
					m_Context->CreateEntity("ChildEntity").SetParent(entity);
				ImGui::Separator();
				if (ImGui::MenuItem("Duplicate"))
					m_Context->DuplicateEntity(entity);
				if (ImGui::MenuItem("Delete"))
					entityDeleted = true;

				ImGui::EndPopup();
			}

			if (opened)
			{
				auto entites = m_Context->GetEntitiesWithParents();
				for (auto entityID : entites)
				{
					Entity child = { entityID, m_Context.get() };
					if (child.GetComponent<ParentComponent>().ParentID == entity.GetUUID())
					{
						bool childOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)child, flags | ImGuiTreeNodeFlags_Leaf, child.GetComponent<TagComponent>().Tag.c_str());

						if (ImGui::BeginDragDropSource())
						{
							AssetHandle handle = child.GetUUID();
							ImGui::SetDragDropPayload("SCENE_HIERARCHY_ITEM", &handle, sizeof(handle));

							ImGui::EndDragDropSource();
						}

						if (ImGui::IsItemClicked())
							m_SelectionContext = child;

						if (childOpened)
							ImGui::TreePop();
					}
				}
			
				ImGui::TreePop();
			}

			ImGui::PopStyleVar();

			if (entityDeleted)
			{
				m_Context->DestroyEntity(entity);
				if (m_SelectionContext == entity) m_SelectionContext = {};
			}

		}
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& label, Entity entity, UIFunction uiFunction)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
		if (!entity.HasComponent<T>()) return;

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });

		float lineHeight = UI::GetLineHeight();
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, label.c_str());
		ImGui::PopStyleVar();

		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

		ImGui::PushFont(boldFont);
		if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
			ImGui::OpenPopup("ComponentSettings");
		ImGui::PopFont();

		bool componentDeleted = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Delete Component"))
				componentDeleted = true;

			ImGui::EndPopup();
		}

		if (open)
		{
			auto& component = entity.GetComponent<T>();

			uiFunction(component);

			ImGui::TreePop();
		}

		if (componentDeleted)
			entity.RemoveComponent<T>();
	}

	template<typename T>
	static void AddComponentItem(const std::string& label, Entity entity)
	{
		if (entity.HasComponent<T>()) return;

		if (ImGui::MenuItem(label.c_str()))
		{
			entity.AddComponent<T>();
			ImGui::CloseCurrentPopup();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{		
		if (entity.HasComponent<TagComponent>())
		{
			auto& selectedTag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), selectedTag.c_str());

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				selectedTag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		{
			UI::ScopedFont boldLarge(UI::Fonts::Get("BoldLarge"));
			if (ImGui::Button("+"))
				ImGui::OpenPopup("AddComponent");
		}

		if (ImGui::BeginPopup("AddComponent"))
		{
			AddComponentItem<TransformComponent>("Transform", entity);
			AddComponentItem<CameraComponent>("Camera", entity);
			AddComponentItem<ScriptComponent>("Script", entity);
			AddComponentItem<SpriteRendererComponent>("Sprite Renderer", entity);
			AddComponentItem<CircleRendererComponent>("Circle Renderer", entity);
			AddComponentItem<EditorRendererComponent>("Editor Renderer", entity);
			AddComponentItem<MeshComponent>("Mesh", entity);
			AddComponentItem<TextComponent>("Text", entity);
			AddComponentItem<RigidBody2DComponent>("Rigid Body 2D", entity);
			AddComponentItem<BoxColider2DComponent>("Box Colider 2D", entity);
			AddComponentItem<CircleColider2DComponent>("Circle Colider 2D", entity);

			ImGui::EndPopup();
		}

		DrawComponent<TransformComponent>("Transform", entity, [&entity](auto& component)
		{
			if (entity.HasParent())
			{
				auto& parentComponent = entity.GetComponent<ParentComponent>();
				UI::Vec3Controls("Translation", parentComponent.Offset);
			}
			else
				UI::Vec3Controls("Translation", component.Translation);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			UI::Vec3Controls("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			UI::Vec3Controls("Scale", component.Scale, 1.0f);
		});

		DrawComponent<ScriptComponent>("Script Component", entity, [entity, scene = m_Context](auto& component) mutable
		{
			bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);
			
			auto& classes = ScriptEngine::GetEntityClasses();
			const char* scriptName = scriptClassExists ? component.ClassName.c_str() : "Script";
			if (ImGui::Button(scriptName))
				ImGui::OpenPopup("ScriptPopup");

			static std::string className = "";
			if (ImGui::BeginPopup("ScriptPopup"))
			{
				ImGui::InputText("##classname", &className);

				static int item_current_idx = -1; // Here we store our selection data as an index.
				if (ImGui::BeginListBox("##listbox"))
				{
					int i = 0;
					for (auto entityClass : classes)
					{
						std::string name = entityClass.first;
						std::vector<std::string> strings = Utils::SplitSting(' ', className);
						std::vector<bool> results;
						bool contains = true;

						for (std::string str : strings)
						{
							std::string lowerCaseName = name;
							std::transform(lowerCaseName.begin(), lowerCaseName.end(), lowerCaseName.begin(), ::tolower);
							std::transform(str.begin(), str.end(), str.begin(), ::tolower);
							if (lowerCaseName.find(str) != std::string::npos)
								results.push_back(true);
							else
								results.push_back(false);
						}
						
						for (size_t i = 0; i < results.size(); i++)
						{
							if (results[i] == false)
								contains = false;
						}

						if (!contains)
							continue;
						
						const bool is_selected = (item_current_idx == i);
						if (ImGui::Selectable(name.c_str(), is_selected))
						{
							item_current_idx = i;
							component.ClassName = name;
						}

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}

						i++;
					}
					ImGui::EndListBox();
				}

				ImGui::EndPopup();
			}		

			if (true)
			{
				if (!scene->IsRunning() && scriptClassExists)
				{
					Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.ClassName);
					const auto& fields = entityClass->GetFields();

					auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
					for (const auto& [name, field] : fields)
					{
						if (entityFields.find(name) != entityFields.end())
						{
							ScriptFieldInstance& scriptField = entityFields.at(name);

							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptField.GetValue<float>();
								if (UI::DragFloat(name.c_str(), &data))
									scriptField.SetValue(data);
							}
						}
						else
						{
							if (field.Type == ScriptFieldType::Float)
							{
								float data = 0.0f;
								if (UI::DragFloat(name.c_str(), &data))
								{
									ScriptFieldInstance& fieldInstance = entityFields[name];
									fieldInstance.Field = field;
									fieldInstance.SetValue(data);
								}
							}
						}
					}

					return;
				}

				if (scriptClassExists)
				{
					Ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
					if (scriptInstance)
					{
						const auto& fields = scriptInstance->GetScriptClass()->GetFields();
						for (const auto& [name, field] : fields)
						{
							if (field.Type == ScriptFieldType::Float)
							{
								float value = scriptInstance->GetFieldValue<float>(name);
								UI::DragFloat(name, &value);
								scriptInstance->SetFieldValue(name, value);
							}
							else if (field.Type == ScriptFieldType::Int)
							{
								int value = 23;
								UI::DragInt(name, &value);
							}
						}
					}
				}
			}
		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [=](auto& component)
		{
			UI::ColorEdit4("Color", glm::value_ptr(component.Color));

			Ref<Texture2D> texturePreview;

			if (!component.Texture)
				texturePreview = TextureImporter::LoadTexture2D("Resources/Textures/BlankTexture.png");
			else
			{
				texturePreview = AssetManager::GetAsset<Texture2D>(component.Texture);
				ImGui::SameLine();
			}

			ImGuiID id = 1;
			ImGui::ImageButtonEx(id, (ImTextureID)texturePreview->GetRendererID(), { 32, 32 }, { 0, 1 }, { 1, 0 }, { 1, 1, 1, 1 }, { 1, 1, 1, 1, });
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					AssetHandle handle = *(AssetHandle*)payload->Data;
					component.Texture = handle;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear"))
				component.Texture = 0;

			UI::DragFloat("Scale", &component.Scale, 0.1f, 1.0f, 50.0f, 150.0f);

		});

		DrawComponent<EditorRendererComponent>("Editor Renderer", entity, [=](auto& component)
		{
			UI::ColorEdit4("Color", glm::value_ptr(component.Color));

			Ref<Texture2D> texturePreview;

			if (!component.Texture)
				texturePreview = TextureImporter::LoadTexture2D("Resources/Textures/BlankTexture.png");
			else
			{
				texturePreview = AssetManager::GetAsset<Texture2D>(component.Texture);
				ImGui::SameLine();
			}

			ImGuiID id = 1;
			ImGui::ImageButtonEx(id, (ImTextureID)texturePreview->GetRendererID(), { 32, 32 }, { 0, 1 }, { 1, 0 }, { 1, 1, 1, 1 }, { 1, 1, 1, 1, });
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					AssetHandle handle = *(AssetHandle*)payload->Data;
					component.Texture = handle;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear"))
				component.Texture = 0;
		});

		DrawComponent<MeshComponent>("Mesh", entity, [=](auto& component)
		{
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("Mesh");
			ImGui::NextColumn();
			
			std::string meshName = Project::GetActive()->GetAssetManager()->GetFilePath(component.MeshHandle).filename().string();
			ImGui::Button(meshName.c_str());
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					AssetHandle handle = *(AssetHandle*)payload->Data;
					component.MeshHandle = handle;
				}
				ImGui::EndDragDropTarget();
			}
			
			ImGui::Columns(1);
			UI::DragInt("Material Index", &component.MaterialIndex, 1, 0, m_Context->m_Materials.size() - 1);
		});

		DrawComponent<PointLightComponent>("Point Light", entity, [=](auto& component)
		{
			UI::ColorEdit3("Color", glm::value_ptr(component.Color));
			UI::DragFloat("Ambient Intensity", &component.AmbientIntensity, 0.05f, 0.0f, 1.0f, 150.0f);
		});

		DrawComponent<DirectionalLightComponent>("Directional Light", entity, [=](auto& component)
		{
			UI::ColorEdit3("Color", glm::value_ptr(component.Color));
			UI::DragFloat("Ambient Intensity", &component.AmbientIntensity, 0.05f, 0.0f, 1.0f, 150.0f);
		});

		DrawComponent<CircleRendererComponent>("Circle Renderer", m_SelectionContext, [](auto& component)
		{
			UI::ColorEdit4("Color", glm::value_ptr(component.Color));
			UI::DragFloat("Thickness", &component.Thickness, 0.01f, 0.0f, 1.0f);
			UI::DragFloat("Fade", &component.Fade, 0.005f, 0.0f, 1.0f, 100.0f, "%.3f");
		});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
		{
			auto& camera = component.Camera;

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float verticalFOV = glm::degrees(camera.GetPerspectiveVertivalFOV());
				if (UI::DragFloat("VFOV", &verticalFOV))
					camera.SetPerspectiveVertivalFOV(glm::radians(verticalFOV));

				float nearClip = camera.GetPerspectiveNearClip();
				if (UI::DragFloat("Near", &nearClip, 0.1f))
					camera.SetPerspectiveNearClip(nearClip);

				float farClip = camera.GetPerspectiveFarClip();
				if (UI::DragFloat("Far", &farClip, 0.1f))
					camera.SetPerspectiveFarClip(farClip);
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if (UI::DragFloat("Size", &orthoSize, 0.1f, 0.25f, 50.0f))
					camera.SetOrthographicSize(orthoSize);

				float nearClip = camera.GetOrthographicNearClip();
				if (UI::DragFloat("Near", &nearClip, 0.1f))
					camera.SetOrthographicNearClip(nearClip);

				float farClip = camera.GetOrthographicFarClip();
				if (UI::DragFloat("Far", &farClip, 0.1f))
					camera.SetOrthographicFarClip(farClip);
			}

			ImGui::Checkbox("Camera Enabled", &component.Primary);
			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
		});
	
		DrawComponent<RigidBody2DComponent>("Rigid Body 2D", entity, [](auto& component)
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];

			if (ImGui::BeginCombo("BodyType", currentBodyTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						component.Type = ((RigidBody2DComponent::BodyType)i);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
			ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
		});

		DrawComponent<BoxColider2DComponent>("Box Colider 2D", entity, [](auto& component)
		{
			UI::Vec2Controls("Size", component.Size);
			UI::Vec2Controls("Offset", component.Offset);
			UI::DragFloat("Density", &component.Density, 0.1f);
			UI::DragFloat("Friction", &component.Friction, 0.1f);
			UI::DragFloat("Restitution", &component.Restitution, 0.01f);
			UI::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.05f, 0.0f, 100.0f, 150.0f);
		});

		DrawComponent<CircleColider2DComponent>("Circle Colider 2D", entity, [](auto& component)
		{
			UI::Vec2Controls("Offset", component.Offset);
			UI::DragFloat("Radius", &component.Radius, 0.1f);
			UI::DragFloat("Density", &component.Density, 0.1f);
			UI::DragFloat("Friction", &component.Friction, 0.1f);
			UI::DragFloat("Restitution", &component.Restitution, 0.01f);
			UI::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.05f, 0.0f, 100.0f, 150.0f);
		});

		DrawComponent<TextComponent>("Text Component", entity, [](auto& component)
		{
			ImGui::InputTextMultiline("Text", &component.Text, ImVec2(0, 0));
			UI::ColorEdit4("Color", glm::value_ptr(component.Color));
			UI::DragFloat("Kerning", &component.Kerning, 0.01f);
			UI::DragFloat("Line Spacing", &component.LineSpacing, 0.05f);
			UI::DragFloat("Max Width", &component.MaxWidth, 1.0f, 0.0f, 1000.0f);

			ImGui::Checkbox("Screen Space", &component.ScreenSpace);
		});
	}

}
