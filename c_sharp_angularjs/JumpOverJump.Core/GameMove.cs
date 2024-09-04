namespace JumpOverJump.Core
{
    public class GameMove
    {
        public GameMove()
        {
            
        }

        public GameMove(string line, bool isBlack, bool isBlackPerspective = false)
        {
            StartPosition = new GamePositon(line.Substring(0,1), line.Substring(1, 1), isBlack, isBlackPerspective);
            NextPosition = new GamePositon(line.Substring(2, 1), line.Substring(3, 1), isBlack, isBlackPerspective);
        }

        public GamePositon StartPosition { get; set; }
        public GamePositon NextPosition { get; set; }

        public override string ToString()
        {
            return StartPosition.ToString() + NextPosition;
        }
    }
}