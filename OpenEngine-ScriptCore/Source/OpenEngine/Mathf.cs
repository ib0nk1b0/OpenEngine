using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OpenEngine
{
    public static class Mathf
    {
        public static Vector3 GetUpDirection(Vector3 direction)
        {
            InternalCalls.Math_GetUpDirection(ref direction, out Vector3 upDirection);
            return upDirection;
        }
    }
}
