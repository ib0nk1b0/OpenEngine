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
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <box2d/box2d.h>

namespace OpenEngine {

	namespace Utils {

		std::string MonoStringToString(MonoString* string)
		{
			char* cStr = mono_string_to_utf8(string);
			OE_CORE_WARN("{}", cStr);
			std::string str(cStr);
			mono_free(cStr);
			return str;
		}

	}

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define OE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("OpenEngine.InternalCalls::"#Name, Name)

#pragma region Entity

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OE_CORE_ASSERT(scene, "");
		Entity entity = scene->GetEntityByUUID(entityID);
		OE_CORE_ASSERT(entity, "");

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		OE_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(), "");
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static uint64_t Entity_GetEntityByName(MonoString* monoName)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OE_CORE_ASSERT(scene, "");
		std::string name = Utils::MonoStringToString(monoName);
		Entity entity = scene->GetEntityByName(name);
		OE_CORE_ASSERT(entity, "");

		if (!entity)
			return 0;

		return entity.GetUUID();
	}

#pragma endregion

#pragma region TransformComponent

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OE_CORE_ASSERT(scene, "");
		Entity entity = scene->GetEntityByUUID(entityID);
		OE_CORE_ASSERT(entity, "");

		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OE_CORE_ASSERT(scene, "");
		Entity entity = scene->GetEntityByUUID(entityID);
		OE_CORE_ASSERT(entity, "");

		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static void TransformComponent_GetRotation(UUID entityID, glm::vec3* outRotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OE_CORE_ASSERT(scene, "");
		Entity entity = scene->GetEntityByUUID(entityID);
		OE_CORE_ASSERT(entity, "");

		*outRotation = entity.GetComponent<TransformComponent>().Rotation;
	}

	static void TransformComponent_SetRotation(UUID entityID, glm::vec3* rotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OE_CORE_ASSERT(scene, "");
		Entity entity = scene->GetEntityByUUID(entityID);
		OE_CORE_ASSERT(entity, "");

		entity.GetComponent<TransformComponent>().Rotation = *rotation;
	}

	static void TransformComponent_GetScale(UUID entityID, glm::vec3* outScale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OE_CORE_ASSERT(scene, "");
		Entity entity = scene->GetEntityByUUID(entityID);
		OE_CORE_ASSERT(entity, "");

		*outScale = entity.GetComponent<TransformComponent>().Scale;
	}

	static void TransformComponent_SetScale(UUID entityID, glm::vec3* scale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OE_CORE_ASSERT(scene, "");
		Entity entity = scene->GetEntityByUUID(entityID);
		OE_CORE_ASSERT(entity, "");

		entity.GetComponent<TransformComponent>().Scale = *scale;
	}

#pragma endregion

#pragma region RigidBody2DComponent

	static void RigidBody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* worldPosition, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OE_CORE_ASSERT(scene, "");
		Entity entity = scene->GetEntityByUUID(entityID);
		OE_CORE_ASSERT(entity, "");

		auto& rigidbody2D = entity.GetComponent<RigidBody2DComponent>();
		b2Body* body = (b2Body*)rigidbody2D.RuntimeBody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(worldPosition->x, worldPosition->y), wake);
	}

	static void RigidBody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		OE_CORE_ASSERT(scene, "");
		Entity entity = scene->GetEntityByUUID(entityID);
		OE_CORE_ASSERT(entity, "");

		auto& rigidbody2D = entity.GetComponent<RigidBody2DComponent>();
		b2Body* body = (b2Body*)rigidbody2D.RuntimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

#pragma endregion

#pragma region Input

	static bool Input_IsKeyDown(KeyCode keyCode)
	{
		return Input::IsKeyPressed(keyCode);
	}

	static void Input_GetMousePosition(glm::vec2* outPosition)
	{
		*outPosition = glm::vec2(Input::GetMouseX(), Input::GetMouseY());
	}

#pragma endregion

#pragma region Maths

	static glm::quat GetOrientation(const glm::vec3& input)
	{
		return glm::quat(input);
	}

	static void Math_GetUpDirection(glm::vec3* inDirection, glm::vec3* outDirection)
	{
		*outDirection = glm::rotate(GetOrientation(*inDirection), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	static void Math_GetForwardDirection(glm::vec3* inDirection, glm::vec3* outDirection)
	{
		*outDirection = glm::rotate(GetOrientation(*inDirection), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	static void Math_GetRightDirection(glm::vec3* inDirection, glm::vec3* outDirection)
	{
		*outDirection = glm::rotate(GetOrientation(*inDirection), glm::vec3(1.0f, 0.0f, 0.0f));
	}

#pragma endregion

#pragma region Log

	static void Log_Trace(MonoString* message)
	{
		OE_TRACE(Utils::MonoStringToString(message));
	}

	static void Log_Info(MonoString* message)
	{
		OE_INFO(Utils::MonoStringToString(message));
	}

	static void Log_Warn(MonoString* message)
	{
		OE_WARN(Utils::MonoStringToString(message));
	}

	static void Log_Error(MonoString* message)
	{
		OE_ERROR(Utils::MonoStringToString(message));
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
		RegisterComponent<RigidBody2DComponent>();
	}

	void ScriptGlue::RegisterFunctions()
	{
		OE_ADD_INTERNAL_CALL(Entity_HasComponent);
		OE_ADD_INTERNAL_CALL(Entity_GetEntityByName);

		OE_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		OE_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		OE_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		OE_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		OE_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		OE_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		OE_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulse);
		OE_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulseToCenter);

		OE_ADD_INTERNAL_CALL(Input_IsKeyDown);
		OE_ADD_INTERNAL_CALL(Input_GetMousePosition);

		OE_ADD_INTERNAL_CALL(Math_GetUpDirection);
		OE_ADD_INTERNAL_CALL(Math_GetForwardDirection);
		OE_ADD_INTERNAL_CALL(Math_GetRightDirection);

		OE_ADD_INTERNAL_CALL(Log_Trace);
		OE_ADD_INTERNAL_CALL(Log_Info);
		OE_ADD_INTERNAL_CALL(Log_Warn);
		OE_ADD_INTERNAL_CALL(Log_Error);
	}

}