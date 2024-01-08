using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using OpenEngine;

namespace Sandbox
{
    public class Player : Entity
    {
        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");
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
