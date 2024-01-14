using OpenEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class ExampleController : Entity
    {
        public float Speed;

        void OnCreate()
        {
        }

        void OnUpdate(float ts)
        {
            Vector3 velocity = new Vector3();

            if (Input.IsKeyDown(KeyCode.W))
                velocity.Y += -Speed * ts;
            if (Input.IsKeyDown(KeyCode.A))
                velocity.X -= -Speed * ts;
            if (Input.IsKeyDown(KeyCode.S))
                velocity.Y -= -Speed * ts;
            if (Input.IsKeyDown(KeyCode.D))
                velocity.X += -Speed * ts;

            Vector3 translation = Translation;
            translation += velocity;
            Translation = translation;
        }
    }
}
