#pragma once

#include "OpenEngine.h"

struct ImFont;

namespace OpenEngine::UI {

	struct FontConfiguration
	{
		std::string Name;
		std::filesystem::path Filepath;
		float Size = 16.0f;
	};

	class Fonts
	{
	public:
		static void Add(const FontConfiguration& config, bool default = false);
		static void PushFont(const std::string& fontName);
		static void PopFont();
		static ImFont* Get(const std::string& fontName);
	};

	class ScopedFont
	{
	public:
		ScopedFont(ImFont* font);
		~ScopedFont();
	};

}