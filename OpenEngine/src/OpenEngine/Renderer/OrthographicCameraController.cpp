#include "oepch.h"
#include "OrthographicCameraController.h"

#include "OpenEngine/Core/Input.h"
#include "OpenEngine/Core/KeyCodes.h"

namespace OpenEngine
{

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation, bool controlsEnabled)
		: m_AspectRatio(aspectRatio),
		  m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
		  m_Rotation(rotation),
		  m_ControlsEnabled(controlsEnabled)
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{

		OE_PROFILE_FUNCTION();

		if (m_ControlsEnabled)
		{
			if (Input::IsKeyPressed(OE_KEY_W))
				m_CameraPosition.y += m_CameraTranslationSpeed * ts;
			if (Input::IsKeyPressed(OE_KEY_A))
				m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
			if (Input::IsKeyPressed(OE_KEY_S))
				m_CameraPosition.y -= m_CameraTranslationSpeed * ts;
			if (Input::IsKeyPressed(OE_KEY_D))
				m_CameraPosition.x += m_CameraTranslationSpeed * ts;

			if (m_Rotation)
			{
				if (Input::IsKeyPressed(OE_KEY_Q))
					m_CameraRotation += m_CameraRotationSpeed * ts;
				if (Input::IsKeyPressed(OE_KEY_E))
					m_CameraRotation -= m_CameraRotationSpeed * ts;

				m_Camera.SetRotation(m_CameraRotation);
			}
		}

		m_Camera.SetPosition(m_CameraPosition);
		m_CameraTranslationSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		OE_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(OE_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(OE_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		OE_PROFILE_FUNCTION();

		if (m_ControlsEnabled)
		{
			m_ZoomLevel -= e.GetOffsetY() * 0.25f;
			m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
			m_ZoomLevel = std::min(m_ZoomLevel, 20.0f);
			m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		}
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		OE_PROFILE_FUNCTION();

		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

}