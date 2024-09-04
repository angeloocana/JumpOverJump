using System.Linq;

namespace JumpOverJump.Core
{
    public class GameInProgress
    {
        public GamePieces Pieces { get; set; }
        public bool IsBlackTurn { get; set; }
        public bool AlreadyInverted { get; private set; }

        public void InvertIfIsBlack()
        {
            if (!IsBlackTurn)
                return;

            if(AlreadyInverted)
                return;

            AlreadyInverted = true;
            IsBlackTurn = false;
            foreach (var piece in Pieces)
                piece.Invert();
        }
        
        public BestMovesToWin GetBestMovesToWin()
        {
            InvertIfIsBlack();

            var pieces = Pieces.ToString();
            var whitePieces = pieces.Substring(0, 16);
            var blackPieces = pieces.Substring(16);

            var bestMovesToWin = new BestMovesToWin
            {
                LookingWhite = MovesToWin.GetFileMovesToWin(Game.MovesToWinPath + whitePieces + "\\a.txt", AlreadyInverted).ToList(),
                LookingBlack =
                    MovesToWin.GetFileMovesToWin(Game.MovesToWinPath + whitePieces + "\\" + blackPieces + ".txt", AlreadyInverted)
                        .ToList()
            };
            return bestMovesToWin;
        }
    }
}