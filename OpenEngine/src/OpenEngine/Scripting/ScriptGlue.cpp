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

	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << "C++ says for C#: " << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		OE_CORE_WARN("Value: {}", *parameter);
		*outResult = glm::normalize(*parameter);
	}

	static void Entity_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void Entity_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		entity.GetComponent<TransformComponent>().Translation = *translation;
	}
	static bool Input_IsKeyDown(KeyCode keyCode)
	{
		return Input::IsKeyPressed(keyCode);
	}

	void ScriptGlue::RegisterFunctions()
	{
		OE_ADD_INTERNAL_CALL(NativeLog);
		OE_ADD_INTERNAL_CALL(NativeLog_Vector);

		OE_ADD_INTERNAL_CALL(Entity_GetTranslation);
		OE_ADD_INTERNAL_CALL(Entity_SetTranslation);
		OE_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}

}