#include "oepch.h"
#include "CubeSpawner.h"
#include <OpenEngine/Core/Random.h>

namespace OpenEngine {

	void CubeSpawner::OnCreate()
	{
		SpawnCube();
	}

	void CubeSpawner::OnUpdate(Timestep ts)
	{
		/*if (Random::UInt(0, 1000) == 0)
			SpawnCube();*/
	}

	void CubeSpawner::SpawnCube()
	{
		/*auto cube = CreateEntity("Cube");
		auto& meshComponent = cube.AddComponent<MeshComponent>();
		meshComponent.Filepath = "assets/Models/Cube.obj";
		auto& tranform = cube.GetComponent<TransformComponent>();
		tranform.Translation = { Random::Float(-5.0f, 5.0f), Random::Float(0.0f, 5.0f), Random::Float(-5.0f, 5.0f) };*/
	}

}