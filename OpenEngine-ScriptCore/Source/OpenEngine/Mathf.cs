namespace OpenEngine
{
    public static class Mathf
    {
        public static Vector3 GetUpDirection(Vector3 direction)
        {
            InternalCalls.Math_GetUpDirection(ref direction, out Vector3 upDirection);
            return upDirection;
        }

        public static Vector3 GetForwardDirection(Vector3 direction)
        {
            InternalCalls.Math_GetForwardDirection(ref direction, out Vector3 forwardDirection);
            return forwardDirection;
        }

        public static Vector3 GetRightDirection(Vector3 direction)
        {
            InternalCalls.Math_GetRightDirection(ref direction, out Vector3 rightDirection);
            return rightDirection;
        }
    }
}
