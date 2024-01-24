namespace OpenEngine
{
    public enum CursorMode
    {
        Normal,
        Locked,
        Hidden
    }

    public class Input
    {
        public static bool IsKeyDown(KeyCode keyCode)
        {
            return InternalCalls.Input_IsKeyDown(keyCode);
        }

        public static Vector2 GetMousePosition()
        {
            InternalCalls.Input_GetMousePosition(out Vector2 mousePosition);
            return mousePosition;
        }

        public static void SetCursorMode(CursorMode cursorMode)
        {
            // TODO
        }
    }
}
