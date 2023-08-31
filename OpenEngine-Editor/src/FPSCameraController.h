#include "OpenEngine.h"

#include <glm/glm.hpp>

namespace OpenEngine {

	class FPSCameraController : public ScriptableEntity
	{
	public:
		virtual void OnCreate() override;
		virtual void OnUpdate(Timestep ts) override;

		void MouseRotate(const glm::vec2& delta);
		glm::vec3 GetUpDirection();
		glm::quat GetOrientation();
		glm::vec3 GetForwardDirection();
	private:
		float m_Sensitivity = 5.0f;
		glm::vec2 m_InitialMousePosition = { 0.0f ,0.0f };

		float m_Pitch = 0.0f, m_Yaw = 0.0f;
	};

}