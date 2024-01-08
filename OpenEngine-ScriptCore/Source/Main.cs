using System;
using System.Runtime.CompilerServices;

namespace OpenEngine
{

    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }
    }

    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog(string text, int parameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog_Vector(ref Vector3 parameter, out Vector3 reuslt);
    }

    public class Entity
    {
        public float Number { get; set; }

        public Entity()
        {
            Console.WriteLine("Main Constructor!");
            Log("Message from the C# constructor calling NativeLog", 2024);

            Vector3 pos = new Vector3(1, 2, 3);
            Vector3 result = Log(pos);
            Console.WriteLine($"{result.X}, {result.Y}, {result.Z}");
        }

        public void PrintMessege()
        {
            Console.WriteLine("Hello World! From C#");
        }

        public void PrintInt(int value)
        {
            Console.WriteLine($"C# Says: {value}");
        }

        public void PrintInts(int value1, int value2)
        {
            Console.WriteLine($"C# Says: {value1} and {value2}");
        }

        public void PrintCustomMessage(string message)
        {
            Console.WriteLine($"C# Says: {message}");
        }

        private void Log(string text, int parameter)
        {
            InternalCalls.NativeLog(text, parameter);
        }

        private Vector3 Log(Vector3 parameter)
        {
            InternalCalls.NativeLog_Vector(ref parameter, out Vector3 result);
            return result;
        }
        
    }

}