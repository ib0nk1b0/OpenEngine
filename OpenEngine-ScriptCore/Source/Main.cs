using System;

public class Main
{
    public Main()
    {
        Console.WriteLine("Main Constructor!");
    }

    public void PrintMessege()
    {
        Console.WriteLine("Hello World! From C#");
    }

    public void PrintInt(int value)
    {
        Console.WriteLine($"C# Says: {value}");
    }

    public void PrintCustomMessage(string message)
    {
        Console.WriteLine($"C# Says: {message}");
    }
}