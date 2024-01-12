using OpenEngine;
using System;

namespace Sandbox
{
    public class Player : Entity
    {
        public float Speed;
        public float JumpSpeed;

        private float m_LastFrameY = 0.0f;
        private RigidBody2DComponent m_RigidBody2D;

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");
            m_RigidBody2D = GetComponent<RigidBody2DComponent>();
        }

        void OnUpdate(float ts)
        {
            Vector3 velocity = Vector3.Zero;
            
            if (Input.IsKeyDown(KeyCode.A))
                velocity.X -= 1 * Speed * ts;
            if (Input.IsKeyDown(KeyCode.D))
                velocity.X += 1 * Speed * ts;

            if (Input.IsKeyDown(KeyCode.Space) && m_LastFrameY == Translation.Y)
                velocity.Y += 10 * JumpSpeed * ts;

            m_RigidBody2D.ApplyLinearImpulse(velocity.XY, true);

            m_LastFrameY = Translation.Y;
        }
    }
}
