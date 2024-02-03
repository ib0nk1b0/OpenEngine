using OpenEngine;

/*
    Example script can be used as a template for scripts
    Duplicate this file and rename as appropriate
 */

namespace Sandbox
{
    public class Test : Entity
    {
        public float JumpSpeed;
        public float Gravity;

        private RigidBody2DComponent m_RigidBody;
        private float m_LastYPos;

        // OnCreate is called when the entity is instantiated
        void OnCreate()
        {
            m_RigidBody = GetComponent<RigidBody2DComponent>();
        }

        // OnUpdate is called every frame with TimeStep as a parameter
        void OnUpdate(float ts)
        {
            Vector2 force = new Vector2(0.0f, -Gravity) * ts;
            m_RigidBody.ApplyLinearImpulse(force, true);

            if (m_LastYPos == Translation.Y && Input.IsKeyDown(KeyCode.Space))
            {
                m_RigidBody.ApplyLinearImpulse(new Vector2(0, JumpSpeed), true);
            }
            Log.Info($"Last: ({m_LastYPos}), Current: ({Translation.Y})");

            m_LastYPos = Translation.Y;
        }
    }
}
