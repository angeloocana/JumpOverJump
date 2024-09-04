namespace JumpOverJump.Core
{
    public class GameMovesToWin : MovesToWin
    {
        public GameMovesToWin(GameMove move, string currentPieces, int nMovesToFinish, bool win)
        {
            Move = move;
            CurrentPieces = currentPieces;
            SetNMovesToFinish(nMovesToFinish, win);
        }

        public string CurrentPieces { get; private set; }
       

        public string GetCurrentWhitePieces()
        {
            return CurrentPieces.Substring(0,16);
        }

        public string GetCurrentBlackPieces()
        {
            return CurrentPieces.Substring(16);
        }
    }
}