#include "oepch.h"
#include "ScriptGlue.h"

#include "OpenEngine/Core/UUID.h"
#include "OpenEngine/Core/KeyCodes.h"
#include "OpenEngine/Core/Input.h"

#include "OpenEngine/Scene/Scene.h"
#include "OpenEngine/Scripting/ScriptEngine.h"

#include <mono/metadata/object.h>

#include <glm/glm.hpp>

namespace OpenEngine {

#define OE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("OpenEngine.InternalCalls::"#Name, Name)

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static void TransformComponent_GetRotation(UUID entityID, glm::vec3* outRotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		*outRotation = entity.GetComponent<TransformComponent>().Rotation;
	}

	static void TransformComponent_SetRotation(UUID entityID, glm::vec3* rotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		entity.GetComponent<TransformComponent>().Rotation = *rotation;
	}

	static void TransformComponent_GetScale(UUID entityID, glm::vec3* outScale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		*outScale = entity.GetComponent<TransformComponent>().Scale;
	}

	static void TransformComponent_SetScale(UUID entityID, glm::vec3* scale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		entity.GetComponent<TransformComponent>().Scale = *scale;
	}

	static bool Input_IsKeyDown(KeyCode keyCode)
	{
		return Input::IsKeyPressed(keyCode);
	}

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		return false;
	}

	void ScriptGlue::RegisterFunctions()
	{
		OE_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		OE_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		OE_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		OE_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		OE_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		OE_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		OE_ADD_INTERNAL_CALL(Input_IsKeyDown);
		OE_ADD_INTERNAL_CALL(Entity_HasComponent);
	}

}