using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Solution_test
{
  internal class Program
  {
    public static void Main(string[] args)
    {
      Console.WriteLine("please input a num:");
      int num = Convert.ToInt32(Console.ReadLine());
      Console.WriteLine("Hello World! {0}", num);
      Console.ReadKey();
    }
  }
}