#pragma once
#include <imgui.h>
#include "ImGuiLayer.h"
#include "glad/glad.h"

namespace OpenEngine {

	class ImGuiInfoLayer : public Layer
	{
	public:
		ImGuiInfoLayer();

		virtual void OnImGuiRender() override;
	};

}