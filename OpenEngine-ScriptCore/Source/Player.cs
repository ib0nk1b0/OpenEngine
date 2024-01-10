using OpenEngine;
using System;

namespace Sandbox
{
    public class Player : Entity
    {
        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");
            bool hasComponent = HasComponent<TransformComponent>();
            string name = "Orthographic Camera";
            Console.WriteLine($"Attempting to find {name}");
            Entity camera = FindEntityByName(name);
            Console.WriteLine($"Attempt finished");
            if (camera != null)
            {
                Console.WriteLine($"Camera: {camera.ID}");
            }
            if (hasComponent)
            {
                Console.WriteLine($"Has a transformComponent");
            }
        }

        void OnUpdate(float ts)
        {
            Console.WriteLine($"Player.OnUpdate: {ts}");

            float speed = 2.0f;
            Vector3 translation = Translation;

            if (Input.IsKeyDown(KeyCode.W))
                translation.Y += speed * ts;
            if (Input.IsKeyDown(KeyCode.A))
                translation.X -= speed * ts;
            if (Input.IsKeyDown(KeyCode.S))
                translation.Y -= speed * ts;
            if (Input.IsKeyDown(KeyCode.D))
                translation.X += speed * ts;

            Translation = translation;
        }
    }
}
