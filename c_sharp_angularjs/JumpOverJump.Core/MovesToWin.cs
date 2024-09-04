using System;
using System.Collections.Generic;
using System.IO;

namespace JumpOverJump.Core
{
    public class MovesToWin
    {
        public MovesToWin()
        {
            
        }

        public MovesToWin(string line, bool isBlackPerspective)
        {
            Move = new GameMove(line, false, isBlackPerspective);
            SetNMovesToFinish(line);
        }

        public GameMove Move { get; set; }
        public int NMovesToFinish { get; private set; }
        public bool Win { get; private set; }

        public void SetNMovesToFinish(string line)
        {
            var n = Convert.ToInt32(line.Substring(4));

            if (n < 0)
            {
                Win = false;
                n = n * -1;
            }
            else
                Win = true;

            SetNMovesToFinish(n, Win);
        }

        public void SetNMovesToFinish(int n, bool win)
        {
            NMovesToFinish = n;
            Win = win;
        }

        public override string ToString()
        {
            return Move.ToString() + (NMovesToFinish * (Win ? 1 : -1));
        }

        public static IEnumerable<MovesToWin> GetFileMovesToWin(string fileName, bool isBlackPerspective  = false)
        {
            if (!File.Exists(fileName)) yield break;

            using (var file = new StreamReader(fileName))
            {
                var line = file.ReadLine();
                while (!string.IsNullOrEmpty(line))
                {
                    yield return new MovesToWin(line, isBlackPerspective);
                    line = file.ReadLine();
                }
            }
        }
    }
}