#include "oepch.h"
#include "Camera.h"

namespace OpenEngine {

	void CameraScript::OnCreate()
	{
		//m_Player = GetEntityByName("Player");
	}

	void CameraScript::OnUpdate(Timestep ts)
	{
		//GetComponent<TransformComponent>().Translation.x += 2.0f * ts;
	}

	void CameraScript::OnDestroy()
	{

	}

}