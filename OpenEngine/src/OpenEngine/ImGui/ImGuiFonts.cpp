#include "oepch.h"
#include "ImGuiFonts.h"

#include <imgui.h>

namespace OpenEngine::UI {

	static std::unordered_map<std::string, ImFont*> s_Fonts;

	void Fonts::Add(const FontConfiguration& config, bool default)
	{
		if (s_Fonts.find(config.Name) != s_Fonts.end())
		{
			OE_CORE_WARN("Tried to load a font with name '{0}' but that name is already taken!", config.Name);
			return;
		}

		auto& io = ImGui::GetIO();
		ImFont* font = io.Fonts->AddFontFromFileTTF(config.Filepath.string().c_str(), config.Size);
		s_Fonts[config.Name] = font;

		if (default)
			io.FontDefault = font;
	}

	void Fonts::PushFont(const std::string& fontName)
	{
		auto& io = ImGui::GetIO();

		if (s_Fonts.find(fontName) == s_Fonts.end())
		{
			ImGui::PushFont(io.FontDefault);
			return;
		}

		ImGui::PushFont(s_Fonts[fontName]);
	}

	void Fonts::PopFont()
	{
		ImGui::PopFont();
	}

	ImFont* Fonts::Get(const std::string& fontName)
	{
		auto& io = ImGui::GetIO();

		if (s_Fonts.find(fontName) == s_Fonts.end())
		{
			OE_CORE_WARN("Font with name '{0}', doesn't exist!");
			return io.FontDefault;
		}

		return s_Fonts[fontName];
	}

	ScopedFont::ScopedFont(ImFont* font)
	{
		ImGui::PushFont(font);
	}

	ScopedFont::~ScopedFont()
	{
		ImGui::PopFont();
	}

}