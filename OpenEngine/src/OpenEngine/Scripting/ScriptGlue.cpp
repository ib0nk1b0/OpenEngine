#include "oepch.h"
#include "ScriptGlue.h"

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

	void ScriptGlue::RegisterFunctions()
	{
		OE_ADD_INTERNAL_CALL(NativeLog);
		OE_ADD_INTERNAL_CALL(NativeLog_Vector);
	}

}