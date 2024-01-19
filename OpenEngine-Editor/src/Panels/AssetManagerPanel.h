#pragma once

#include "Panel.h"

namespace OpenEngine {

	class AssetManagerPanel : public Panel
	{
	public:
		AssetManagerPanel();

		static std::string GetName() { return "AssetManagerPanel"; }
		void OnImGuiRender() override;
	};

}