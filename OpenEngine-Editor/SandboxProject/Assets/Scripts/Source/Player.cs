using OpenEngine;
using System;

namespace Sandbox
{
    public class Player : Entity
    {
        public float Speed = 1.0f;
        public float JumpSpeed = 2.0f;
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
                velocity.X -= Speed * ts;
            if (Input.IsKeyDown(KeyCode.D))
                velocity.X += Speed * ts;

            if (Input.IsKeyDown(KeyCode.Space))
                velocity.Y += JumpSpeed * ts;

            m_RigidBody2D.ApplyLinearImpulse(velocity.XY, true);
        }
    }
}
