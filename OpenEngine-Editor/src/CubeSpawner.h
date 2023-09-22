#pragma once
#include "OpenEngine.h"

namespace OpenEngine {

	class CubeSpawner : public ScriptableEntity
	{
		virtual void OnCreate() override;
		virtual void OnUpdate(Timestep ts) override;

		void SpawnCube();
	};

}