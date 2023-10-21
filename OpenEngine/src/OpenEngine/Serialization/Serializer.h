#pragma once

#include "OpenEngine/Core/Core.h"

#include <iostream>
#include <fstream>

#include <json.hpp>

#include <glm/gtx/string_cast.hpp>

namespace OpenEngine {

	/*class Serializer
	{
	public:
		static std::string BeginFile(const std::string& filepath);
		static std::string EndFile();

		static std::string BeginObject(const std::string& key);
		static std::string EndObject();

		static std::string AddString(const std::string& key, const std::string& value);
		static std::string AddInt(const std::string& key, int value);
		static std::string AddFloat(const std::string& key, float value);
		static std::string AddBool(const std::string& key, bool value);
		static std::string AddVec2(const std::string& key, const glm::vec2& value);
		static std::string AddVec3(const std::string& key, const glm::vec3& value);
		static std::string AddVec4(const std::string& key, const glm::vec4& value);
	};*/

}

namespace OpenEngine::Serializer
{

	static std::string Tab(uint32_t count = 1)
	{
		std::string result;
		for (size_t i = 0; i < count; i++)
			result += "\t";

		return result;
	}

	static std::string NewLine()
	{
		return "\n";
	}

	static std::string StartJSONObject(const std::string& label)
	{
		return Tab(3) + "\"" + label + "\": {" + NewLine() + Tab();
	}

	static std::string EndJSONObject(bool last = false)
	{
		if (last)
			return Tab(3) + "}," + NewLine() + Tab();

		return Tab(3) + "}" + NewLine() + Tab();
	}

	static std::string GetJSONString(const std::string& label, const std::string& value, bool lastLine = false)
	{
		std::string result;

		result += Tab(3) + "\"" + label + "\": ";

		if (value[0] == '[') result += value;
		else result += "\"" + value + "\"";

		if (!lastLine)
			result += ",";

		result += NewLine() + Tab();

		return result;
	}

	static std::string GetJSONString(const std::string& label, int value, bool lastLine = false)
	{
		std::string result;

		result += Tab(3) + "\"" + label + "\": ";

		result += std::to_string(value);

		if (!lastLine)
			result += ",";

		result += NewLine() + Tab();

		return result;
	}

	static std::string GetJSONString(const std::string& label, float value, bool lastLine = false)
	{
		std::string result;

		result += Tab(3) + "\"" + label + "\": ";

		result += std::to_string(value);

		if (!lastLine)
			result += ",";

		result += NewLine() + Tab();

		return result;
	}

	static std::string GetJSONString(const std::string& label, bool value, bool lastLine = false)
	{
		std::string result;

		result += Tab(3) + "\"" + label + "\": ";

		if (value) result += "true";
		else result += "false";

		if (!lastLine)
			result += ",";

		result += NewLine() + Tab();

		return result;
	}

	static std::string Encode(glm::vec3& values)
	{
		std::string result;

		result += "[" + std::to_string(values[0]) + ", ";
		result += std::to_string(values[1]) + ", ";
		result += std::to_string(values[2]) + "]";

		return result;
	}

	static void Decode(std::string values, glm::vec3& result)
	{
		std::string delimiter = ", ";
		std::vector<std::string> convertedValues;

		size_t pos = 0;
		std::string token;
		values.erase(0, 1);
		values.erase(values.length() - 1, 1);
		for (int i = 0; i < 3; i++)
		{
			pos = values.find(delimiter);
			token = values.substr(0, pos);
			convertedValues.push_back(token);
			values.erase(0, pos + delimiter.length());
		}

		result.x = std::stof(convertedValues[0]);
		result.y = std::stof(convertedValues[1]);
		result.z = std::stof(convertedValues[2]);
	}

	static std::string Encode(glm::vec4& values)
	{
		std::string result;

		result += "[" + std::to_string(values[0]) + ", ";
		result += std::to_string(values[1]) + ", ";
		result += std::to_string(values[2]) + ", ";
		result += std::to_string(values[3]) + "]";

		return result;
	}

	static void Decode(std::string values, glm::vec4& result)
	{
		std::string delimiter = ", ";
		std::vector<std::string> convertedValues;

		size_t pos = 0;
		std::string token;
		values.erase(0, 1);
		values.erase(values.length() - 1, 1);
		for (int i = 0; i < 4; i++)
		{
			pos = values.find(delimiter);
			token = values.substr(0, pos);
			convertedValues.push_back(token);
			values.erase(0, pos + delimiter.length());
		}

		result.r = std::stof(convertedValues[0]);
		result.g = std::stof(convertedValues[1]);
		result.b = std::stof(convertedValues[2]);
		result.a = std::stof(convertedValues[3]);
	}

	static std::string ConvertFloat3(const nlohmann::json_abi_v3_11_2::json& data)
	{
		std::string result;
		int count = 0;

		result += "[";
		for (auto it = data.begin(); it != data.end(); ++it)
		{
			result += std::to_string((float)it.value());
			if (count < 2) result += ", ";
			count++;
		}
		result += "]";

		return result;
	}

	static std::string ConvertFloat4(const nlohmann::json_abi_v3_11_2::json& data)
	{
		std::string result;
		int count = 0;

		result += "[";
		for (auto it = data.begin(); it != data.end(); ++it)
		{
			result += std::to_string((float)it.value());
			if (count < 3) result += ", ";
			count++;
		}
		result += "]";

		return result;
	}

}
