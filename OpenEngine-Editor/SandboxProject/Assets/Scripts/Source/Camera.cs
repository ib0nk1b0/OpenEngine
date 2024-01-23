using OpenEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Camera : Entity
    {
        public Entity Player;

        public void OnCreate()
        {
            Player = GetEntityByName("Player");
        }

        public void OnUpdate(float ts)
        {
            Translation = Player.Translation;
        }
    }
}
