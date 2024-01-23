using OpenEngine;

namespace Sandbox
{
    public class FpsController : Entity
    {
        public float Sensitivity = 5.0f;
        private Vector2 m_InitialMousePosition;
        private float m_Pitch = 0.0f, m_Yaw = 0.0f;

        void OnCreate()
        {
            m_InitialMousePosition = new Vector2();
        }

        void OnUpdate(float ts)
        {
            TransformComponent transformComponent = GetComponent<TransformComponent>();
            Vector3 rotation = transformComponent.Rotation;

            m_Pitch = rotation.X;
            m_Yaw = rotation.Y;

            Vector2 mouse = new Vector2(Input.GetMousePosition());
            Vector2 delta = (mouse - m_InitialMousePosition) * ts;
            m_InitialMousePosition = mouse;
        }

        void MouseRotate(Vector2 delta)
        {
            Vector3 upDirection = Mathf.GetUpDirection(new Vector3(m_Pitch, -m_Yaw, 0.0f));
            float yawSign = upDirection.Y < 0 ? -1.0f : 1.0f;
            m_Yaw -= yawSign * delta.X * (Sensitivity * 0.025f);
            m_Pitch -= delta.Y * (Sensitivity * 0.025f);
        }
    }
}
