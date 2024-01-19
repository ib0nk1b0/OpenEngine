#include "oepch.h"
#include "PanelManager.h"

namespace OpenEngine {

	PanelManager::PanelManager()
	{
	}

	void PanelManager::OnImGuiRender()
	{
		for (auto& [name, panel] : m_Panels)
			panel->OnImGuiRender();
	}

}
