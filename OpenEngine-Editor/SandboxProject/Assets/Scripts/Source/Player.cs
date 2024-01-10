using OpenEngine;
using System;

namespace Sandbox
{
    public class Player : Entity
    {
        public float Speed = 0.2f;
        public string Name;
        public int Number;

        private RigidBody2DComponent m_RigidBody2D;

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");
            m_RigidBody2D = GetComponent<RigidBody2DComponent>();
        }

        void OnUpdate(float ts)
        {
            Vector3 velocity = Vector3.Zero;
            
            if (Input.IsKeyDown(KeyCode.W))
                velocity.Y += Speed;
            if (Input.IsKeyDown(KeyCode.A))
                velocity.X -= Speed;
            if (Input.IsKeyDown(KeyCode.S))
                velocity.Y -= Speed;
            if (Input.IsKeyDown(KeyCode.D))
                velocity.X += Speed;

            m_RigidBody2D.ApplyLinearImpulse(velocity.XY, true);
        }
    }
}
