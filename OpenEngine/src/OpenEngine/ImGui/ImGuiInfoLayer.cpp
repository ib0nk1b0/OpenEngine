#include "oepch.h"
#include "ImGuiInfoLayer.h"

namespace OpenEngine {

	ImGuiInfoLayer::ImGuiInfoLayer()
		: Layer("InfoLayer")
	{
	}

	void ImGuiInfoLayer::OnImGuiRender()
	{
		ImGui::Begin("OpenGL Info");
		ImGui::Text(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		ImGui::Text(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		ImGui::Text(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
		ImGui::End();
	}

}