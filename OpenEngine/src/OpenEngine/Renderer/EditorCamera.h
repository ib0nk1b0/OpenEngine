#pragma once

#include "Camera.h"
#include "OpenEngine/Core/Timestep.h"
#include "OpenEngine/Events/Event.h"
#include "OpenEngine/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace OpenEngine {

	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;

		float GetPitch() const { return m_Pitch; }
		void SetPitch(float pitch) { m_Pitch = pitch; }

		float GetYaw() const { return m_Yaw; }
		void SetYaw(float yaw) { m_Yaw = yaw; }

		float GetFOV() const { return m_FOV; }
		void SetFOV(float fov) { m_FOV = fov; }

		float GetNearClip() const { return m_NearClip; }
		void SetNearClip(float nearClip) { m_NearClip = nearClip; }

		float GetFarClip() const { return m_FarClip; }
		void SetFarClip(float farClip) { m_FarClip = farClip; }
	public:
		bool m_Rotate = true;
	private:
		void UpdateProjection();
		void UpdateView();

		void ResetCamera();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;
	private:
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_InitialMousePosition;

		float m_Distance = 15.0f;
		float m_Pitch = 0.45f, m_Yaw = 0.67f;

		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};

}