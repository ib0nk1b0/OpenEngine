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
		/*if ((Input::IsKeyPressed(Key::Space) || Input::IsMouseButtonPressed(Mouse::ButtonLeft)) && m_Velocity.y <= 0.0f)
			m_Velocity.y = m_Power;
		else
			m_Velocity.y -= m_Gravity;

		auto translation = GetComponent<TransformComponent>().Translation;
		translation.x += m_Velocity.x * ts;
		translation.y += m_Velocity.y * ts;
		GetComponent<TransformComponent>().Translation = translation;

		Entity ground = FindEntityByName("Ground");
		if (CollisionQuads(FindEntityByName("Player"), ground))
			OE_CORE_INFO("COLLISION");*/
		float speed = 2.0f;
		auto& transform = GetComponent<TransformComponent>();
		//auto& translation = GetComponent<TransformComponent>().Translation;
		//auto& translation = GetTranslation();
		auto& translation = transform.Translation;
		if (Input::IsKeyPressed(Key::W))
			translation.y += speed * ts;
		if (Input::IsKeyPressed(Key::A))
			translation.x -= speed * ts;
		if (Input::IsKeyPressed(Key::S))
			translation.y -= speed * ts;
		if (Input::IsKeyPressed(Key::D))
			translation.x += speed * ts;
	}

	void Player::OnDestroy()
	{

	}

}