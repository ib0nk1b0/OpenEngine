#pragma once

#include <string>

namespace OpenEngine {

	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
		static bool IsValidFile(const std::filesystem::path& filepath, const std::string& extension);
	};

	class Utils
	{
	public:
		static std::string FormatFilepath(std::string filepathOriginal);
	};

}