#include "oepch.h"
#include "FPSCameraController.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace OpenEngine {

	void FPSCameraController::OnCreate()
	{
	}

	void FPSCameraController::OnUpdate(Timestep ts)
	{
		auto& transform = GetComponent<TransformComponent>();
		m_Pitch = transform.Rotation.x;
		m_Yaw = transform.Rotation.y;

		glm::vec2 mouse = { Input::GetMouseX(), Input::GetMouseY() };
		glm::vec2 delta = (mouse - m_InitialMousePosition) * (float)ts;
		m_InitialMousePosition = mouse;
		MouseRotate(delta);
		float pitchInDegrees = glm::degrees(m_Pitch);
		float rotationX = glm::clamp(pitchInDegrees, -60.0f, 60.0f);
		
		transform.Rotation.x = glm::radians(rotationX);
		transform.Rotation.y = m_Yaw;

		auto Parent = FindEntityByName("Player");
		auto& parentTransform = Parent.GetComponent<TransformComponent>();

		parentTransform.Rotation.y = transform.Rotation.y;
	}

	void FPSCameraController::MouseRotate(const glm::vec2& delta)
	{
		auto upDirection = GetUpDirection();
		float yawSign = upDirection.y < 0 ? -1.0f : 1.0f;
		m_Yaw -= yawSign * delta.x * (m_Sensitivity * 0.025);
		m_Pitch -= delta.y * (m_Sensitivity * 0.025);
	}

	glm::vec3 FPSCameraController::GetUpDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 FPSCameraController::GetForwardDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::quat FPSCameraController::GetOrientation()
	{
		return glm::quat(glm::vec3(m_Pitch, -m_Yaw, 0.0f));
	}

}