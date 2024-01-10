#include "oepch.h"
#include "ScriptGlue.h"

#include "OpenEngine/Core/UUID.h"
#include "OpenEngine/Core/KeyCodes.h"
#include "OpenEngine/Core/Input.h"

#include "OpenEngine/Scene/Scene.h"
#include "OpenEngine/Scripting/ScriptEngine.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include <glm/glm.hpp>

namespace OpenEngine {

	namespace Utils {

		std::string MonoStringToString(MonoString* string)
		{
			char* cStr = mono_string_to_utf8(string);
			std::string str(cStr);
			mono_free(cStr);
			return str;
		}

	}

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define OE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("OpenEngine.InternalCalls::"#Name, Name)

#pragma region TransformComponent

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

#pragma endregion

#pragma region Entity

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		OE_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(), "");
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static UUID Entity_FindEntityByName(MonoString* monoName)
	{
		char* cStr = mono_string_to_utf8(monoName);
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByName(cStr);

		if (!entity)
			return 0;

		return entity.GetUUID();
	}

#pragma endregion

#pragma Input

	static bool Input_IsKeyDown(KeyCode keyCode)
	{
		return Input::IsKeyPressed(keyCode);
	}

#pragma endregion

	template<typename Component>
	static void RegisterComponent()
	{
		std::string_view typeName = typeid(Component).name();
		size_t pos = typeName.find_last_of(':');
		std::string_view structName = typeName.substr(pos + 1);
		std::string managedTypeName = fmt::format("OpenEngine.{}", structName);

		MonoType* managedType = mono_reflection_type_from_name(managedTypeName.data(), ScriptEngine::GetCoreAssemblyImage());
		if (!managedType)
		{
			OE_CORE_ERROR("Could not find component type {}", managedTypeName);
			return;
		}
		s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
	}

	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponent<TransformComponent>();
	}

	void ScriptGlue::RegisterFunctions()
	{
		OE_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		OE_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		OE_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		OE_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		OE_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		OE_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		OE_ADD_INTERNAL_CALL(Entity_HasComponent);
		OE_ADD_INTERNAL_CALL(Entity_FindEntityByName);

		OE_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}

}