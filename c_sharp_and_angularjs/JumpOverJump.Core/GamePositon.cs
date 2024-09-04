using System;

namespace JumpOverJump.Core
{
    public class GamePositon
    {
        public GamePositon()
        {

        }

        public GamePositon(int x, int y, bool isBlack, bool isBlackPerspective = false)
        {
            X = x;
            Y = y;
            IsBlack = isBlack;

            if (isBlackPerspective)
                Invert();
        }

        public GamePositon(string x, string y, bool isBlack, bool isBlackPerspective = false)
            : this(Convert.ToInt16(x), Convert.ToInt16(y), isBlack, isBlackPerspective)
        {
        }

        public int X { get; set; }
        public int Y { get; set; }
        public bool IsBlack { get; set; }

        public void Invert()
        {
            IsBlack = !IsBlack;
            X = Game.InvertNumber(X);
            Y = Game.InvertNumber(Y);
        }

        public override string ToString()
        {
            return X + "" + Y;
        }
    }
}