using OpenEngine;

namespace Sandbox
{
    public class FpsController : Entity
    {
        public float MouseSensitivity = 5.0f;
        public float MovementSpeed = 2.0f;

        private Vector2 m_InitialMousePosition;
        private float m_Pitch = 0.0f, m_Yaw = 0.0f;
        private Entity m_Camera;

        void OnCreate()
        {
            m_InitialMousePosition = new Vector2();
            m_Camera = GetEntityByName("Camera");
        }

        void OnUpdate(float ts)
        {
            Movement(ts);
            CameraRotation(ts);
        }

        private void Movement(float ts)
        {
            Vector3 translation = Translation;
            Vector3 rotation = Rotation;

            Vector3 direction = new Vector3(rotation.X, -rotation.Y, 0.0f);

            if (Input.IsKeyDown(KeyCode.W))
            {
                translation.X -= Mathf.GetForwardDirection(direction).X * MovementSpeed * ts;
                translation.Z += Mathf.GetForwardDirection(direction).Z * MovementSpeed * ts;
            }
            if (Input.IsKeyDown(KeyCode.S))
            {
                translation.X += Mathf.GetForwardDirection(direction).X * MovementSpeed * ts;
                translation.Z -= Mathf.GetForwardDirection(direction).Z * MovementSpeed * ts;
            }
            if (Input.IsKeyDown(KeyCode.A))
            {
                translation.X -= Mathf.GetRightDirection(direction).X * MovementSpeed * ts;
                translation.Z += Mathf.GetRightDirection(direction).Z * MovementSpeed * ts;
            }
            if (Input.IsKeyDown(KeyCode.D))
            {
                translation.X += Mathf.GetRightDirection(direction).X * MovementSpeed * ts;
                translation.Z -= Mathf.GetRightDirection(direction).Z * MovementSpeed * ts;
            }

            Translation = translation;
            m_Camera.Translation = translation;
        }

        private void CameraRotation(float ts)
        {
            Vector3 rotation = m_Camera.Rotation;

            m_Pitch = rotation.X;
            m_Yaw = rotation.Y;

            Vector2 mouse = new Vector2(Input.GetMousePosition());
            Vector2 delta = (mouse - m_InitialMousePosition) * ts;
            m_InitialMousePosition = mouse;
            MouseRotate(delta);

            float rotationInDegrees = Mathf.Degrees(m_Pitch);
            float clamped = Mathf.Clamp(rotationInDegrees, -45.0f, 45.0f);
            rotation.X = Mathf.Radians(clamped);
            rotation.Y = m_Yaw;

            m_Camera.Rotation = rotation;
            Rotation = rotation;
        }

        private void MouseRotate(Vector2 delta)
        {
            Vector3 upDirection = Mathf.GetUpDirection(new Vector3(m_Pitch, -m_Yaw, 0.0f));
            float yawSign = upDirection.Y < 0 ? -1.0f : 1.0f;
            m_Yaw -= yawSign * delta.X * (MouseSensitivity * 0.025f);
            m_Pitch -= delta.Y * (MouseSensitivity * 0.025f);
        }
    }
}
