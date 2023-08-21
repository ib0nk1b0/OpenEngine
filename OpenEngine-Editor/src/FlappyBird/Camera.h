#pragma once

#include "OpenEngine.h"

namespace OpenEngine {
	
	class CameraScript : public ScriptableEntity
	{
	public:
		virtual void OnCreate() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnDestroy() override;
	private:
		Entity m_Player;
	};

}
