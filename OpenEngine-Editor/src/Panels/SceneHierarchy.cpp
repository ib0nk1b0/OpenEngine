#include "oepch.h"
#include "SceneHierarchy.h"

#include "OpenEngine/Scene/Components.h"
#include "ContentBrowserPanel.h"
#include "OpenEngine/Utils/PlatformUtils.h"
#include "OpenEngine/ImGui/ImGuiExtended.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace OpenEngine {

	extern const std::filesystem::path g_AssetPath;

	static float GetImGuiLineHeight()
	{
		return GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
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
					ImGui::Separator();
					if (ImGui::MenuItem("Cube"))
					{
						Entity cube = m_Context->CreateEntity("Cube");
						cube.AddComponent<MeshComponent>();
					}
					ImGui::Separator();
					if (ImGui::BeginMenu("Camera"))
					{
						if (ImGui::MenuItem("Perspective"))
						{
							auto camera = m_Context->CreateEntity("Perspective Camera");
							camera.AddComponent<CameraComponent>().Camera.SetProjectionType(SceneCamera::ProjectionType::Perspective);
							camera.AddComponent<EditorRendererComponent>().Texture = Texture2D::Create("assets/textures/camera.png");
						}
						if (ImGui::MenuItem("Orthographic"))
						{
							auto camera = m_Context->CreateEntity("Orthographic Camera");
							camera.AddComponent<CameraComponent>().Camera.SetProjectionType(SceneCamera::ProjectionType::Orthographic);
							camera.AddComponent<EditorRendererComponent>().Texture = Texture2D::Create("assets/textures/camera.png");
						}

						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}

				ImGui::EndPopup();
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
						if (entity.GetName() != child.GetComponent<ParentComponent>().ParentName)
							child.GetComponent<ParentComponent>().ParentName = entity.GetName();

						bool childOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)child, flags, child.GetComponent<TagComponent>().Tag.c_str());
					
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

		float lineHeight = ImGui::GetLineHeight();
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

		if (ImGui::Button("+"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			AddComponentItem<TransformComponent>("Transform", entity);
			AddComponentItem<SpriteRendererComponent>("Sprite Renderer", entity);
			AddComponentItem<CircleRendererComponent>("Circle Renderer", entity);
			AddComponentItem<EditorRendererComponent>("Editor Renderer", entity);
			AddComponentItem<MeshComponent>("Mesh", entity);
			AddComponentItem<CameraComponent>("Camera", entity);
			AddComponentItem<RigidBody2DComponent>("RigidBody2D", entity);
			AddComponentItem<BoxColider2DComponent>("BoxColider2D", entity);

			ImGui::EndPopup();
		}

		DrawComponent<ParentComponent>("Parent", entity, [](auto& component)
		{

			std::vector<Entity> entites;
			//Add change parent
			if (ImGui::BeginCombo("Parent", "None"))
			{
				for (int i = 0; i < entites.size(); i++)
				{
					bool isSelected = false;
					if (ImGui::Selectable(entites[i].GetName().c_str(), isSelected))
					{
						
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
			ImGui::OEVec3Controls("Offset", component.Offset);
		});

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			ImGui::OEVec3Controls("Translation", component.Translation);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			ImGui::OEVec3Controls("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			ImGui::OEVec3Controls("Scale", component.Scale, 1.0f);
		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [=](auto& component)
		{
			ImGui::OEColorEdit4("Color", glm::value_ptr(component.Color));

			Ref<Texture2D> texturePreview;

			if (!component.Texture)
				texturePreview = Texture2D::Create("assets/icons/BlankTexture.png");
			else
			{
				texturePreview = component.Texture;
				ImGui::Text("%s", component.Texture->GetFilePath().c_str());
				ImGui::SameLine();
			}

			ImGuiID id = 1;
			ImGui::ImageButtonEx(id, (ImTextureID)texturePreview->GetRendererID(), { 32, 32 }, { 0, 1 }, { 1, 0 }, { 0, 0, 0, 0 }, { 1, 1, 1, 1 });
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path fullPath = std::filesystem::path(g_AssetPath) / path;
					if (FileDialogs::IsValidFile(fullPath, ".png"))
						component.Texture = Texture2D::Create(fullPath.string());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear"))
				component.Texture = nullptr;

			ImGui::OEDragFloat("Scale", &component.Scale, 0.1f, 1.0f, 50.0f, 150.0f);

		});

		DrawComponent<EditorRendererComponent>("Editor Renderer", entity, [=](auto& component)
			{
				ImGui::OEColorEdit4("Color", glm::value_ptr(component.Color));

				Ref<Texture2D> texturePreview;

				if (!component.Texture)
					texturePreview = Texture2D::Create("assets/icons/BlankTexture.png");
				else
				{
					texturePreview = component.Texture;
					ImGui::Text("%s", component.Texture->GetFilePath().c_str());
					ImGui::SameLine();
				}

				ImGuiID id = 1;
				ImGui::ImageButtonEx(id, (ImTextureID)texturePreview->GetRendererID(), { 32, 32 }, { 0, 1 }, { 1, 0 }, { 0, 0, 0, 0 }, { 1, 1, 1, 1 });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path fullPath = std::filesystem::path(g_AssetPath) / path;
						if (FileDialogs::IsValidFile(fullPath, ".png"))
							component.Texture = Texture2D::Create(fullPath.string());
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();

				if (ImGui::Button("Clear"))
					component.Texture = nullptr;
			});

		DrawComponent<MeshComponent>("Mesh", entity, [=](auto& component)
		{
			ImGui::OEColorEdit3("Color", glm::value_ptr(component.Color));
		});

		DrawComponent<CircleRendererComponent>("Circle Renderer", m_SelectionContext, [](auto& component)
		{
			ImGui::OEColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::OEDragFloat("Thickness", &component.Thickness, 0.01f, 0.0f, 1.0f);
			ImGui::OEDragFloat("Fade", &component.Fade, 0.005f, 0.0f, 1.0f, 100.0f, "%.3f");
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
				if (ImGui::OEDragFloat("VFOV", &verticalFOV))
					camera.SetPerspectiveVertivalFOV(glm::radians(verticalFOV));

				float nearClip = camera.GetPerspectiveNearClip();
				if (ImGui::OEDragFloat("Near", &nearClip, 0.1f))
					camera.SetPerspectiveNearClip(nearClip);

				float farClip = camera.GetPerspectiveFarClip();
				if (ImGui::OEDragFloat("Far", &farClip, 0.1f))
					camera.SetPerspectiveFarClip(farClip);
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if (ImGui::OEDragFloat("Size", &orthoSize, 0.1f, 0.25f, 50.0f))
					camera.SetOrthographicSize(orthoSize);

				float nearClip = camera.GetOrthographicNearClip();
				if (ImGui::OEDragFloat("Near", &nearClip, 0.1f))
					camera.SetOrthographicNearClip(nearClip);

				float farClip = camera.GetOrthographicFarClip();
				if (ImGui::OEDragFloat("Far", &farClip, 0.1f))
					camera.SetOrthographicFarClip(farClip);
			}

			ImGui::Checkbox("Camera Enabled", &component.Primary);
			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
		});
	
		DrawComponent<RigidBody2DComponent>("RigidBody2D", entity, [](auto& component)
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];

			if (ImGui::BeginCombo("BodyType", currentBodyTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						component.Type = ((BodyType)i);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
		});

		DrawComponent<BoxColider2DComponent>("BoxColider2D", entity, [](auto& component)
		{
			ImGui::OEVec3Controls("Size", component.Size);
			ImGui::OEVec3Controls("Offset", component.Offset);
		});
	}

}
