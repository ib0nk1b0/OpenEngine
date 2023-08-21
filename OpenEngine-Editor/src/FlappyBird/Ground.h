#pragma once

#include "OpenEngine.h"

namespace OpenEngine {
	
	class Ground : public ScriptableEntity
	{
	public:
		virtual void OnCreate() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnDestroy() override;
	};

}
