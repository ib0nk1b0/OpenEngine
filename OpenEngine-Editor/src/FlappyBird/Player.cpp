#include "oepch.h"
#include "Player.h"

namespace OpenEngine {

	static bool CollisionQuads(Entity quad1, Entity quad2)
	{
		// Get position
		auto translation1 = quad1.GetTranslation();
		auto translation2 = quad2.GetTranslation();

		// Get size
		auto scale1 = quad1.GetScale();
		auto scale2 = quad2.GetScale();

		if (translation1.x < translation2.x + scale2.x / 2 &&
			translation1.x + scale1.x / 2 > translation2.x &&
			translation1.y < translation2.y + scale2.y / 2 &&
			translation1.y + scale1.y / 2 > translation2.y)
		{
			return true;
		}

		return false;
	}

	void Player::OnCreate()
	{
	}

	void Player::OnUpdate(Timestep ts)
	{
		Move(ts);
	}

	void Player::Move(Timestep ts)
	{
		auto& translation = GetComponent<TransformComponent>().Translation;

		if (Input::IsKeyPressed(Key::W))
		{
			translation.x -= GetForwardDirection().x * m_Speed * (float)ts;
			translation.z += GetForwardDirection().z * m_Speed * (float)ts;
		}
		if (Input::IsKeyPressed(Key::S))
		{
			translation.x += GetForwardDirection().x * m_Speed * (float)ts;
			translation.z -= GetForwardDirection().z * m_Speed * (float)ts;
		}
		if (Input::IsKeyPressed(Key::A))
		{
			translation.x -= GetRightDirection().x * m_Speed * (float)ts;
			translation.z += GetRightDirection().z * m_Speed * (float)ts;
		}
		if (Input::IsKeyPressed(Key::D))
		{
			translation.x += GetRightDirection().x * m_Speed * (float)ts;
			translation.z -= GetRightDirection().z * m_Speed * (float)ts;
		}
	}

	void Player::OnDestroy()
	{

	}

	glm::vec3 Player::GetForwardDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 Player::GetRightDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::quat Player::GetOrientation()
	{
		return glm::quat(glm::vec3(GetComponent<TransformComponent>().Rotation.x, -GetComponent<TransformComponent>().Rotation.y, 0.0f));
	}

}