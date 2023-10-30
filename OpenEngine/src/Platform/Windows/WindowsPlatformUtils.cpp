#include "oepch.h"
#include "OpenEngine/Utils/PlatformUtils.h"
#include "OpenEngine/Core/Application.h"

#include <filesystem>

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace OpenEngine {

	std::string FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		return std::string();
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		return std::string();
	}

	bool FileDialogs::IsValidFile(const std::filesystem::path& filepath, const std::string& extension)
	{
		if (filepath.string().substr(filepath.string().length() - extension.length(), extension.length()) != extension)
			return false;

		return true;
	}

	std::string Utils::FormatFilepath(std::string filepathOriginal)
	{
		std::string filepath;
		char delimiter = '\\';
		size_t pos = 0;
		std::string token;

		int no_of_backslash = (int)std::count(filepathOriginal.begin(), filepathOriginal.end(), '\\');
		if (no_of_backslash > 0)
		{
			for (int i = 0; i < no_of_backslash + 1; i++)
			{
				pos = filepathOriginal.find(delimiter);
				token = filepathOriginal.substr(0, pos);
				filepath += token + "/";
				filepathOriginal.erase(0, pos + 1);
			}
			filepath.erase(filepath.length() - 1, filepath.length());
		}
		else
			filepath = filepathOriginal;

		return filepath;
	}

	std::string Utils::GetFileNameFromPath(std::string filepath)
	{
		if (filepath.find(".") != std::string::npos && filepath.find("\\") != std::string::npos || filepath.find("/") != std::string::npos)
		{
			size_t extensionPos = filepath.find_last_of(".");
			size_t slashPos = filepath.find_last_of("\\/");
			std::string ext = filepath.substr(extensionPos);
			std::string result = filepath.substr(slashPos + 1, filepath.size() - slashPos - ext.size() - 1);
			return result;
		}

		return filepath;
	}

}