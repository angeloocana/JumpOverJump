using System.Collections.Generic;
using System.Linq;

namespace JumpOverJump.Core
{
    public class GamePieces : List<GamePositon>
    {
        public static GamePieces GetStartPositons()
        {
            return new GamePieces
            {
                //Black Pieces
                new GamePositon(0,0,true),new GamePositon(1,0,true),new GamePositon(2,0,true),new GamePositon(3,0,true),
                new GamePositon(4,0,true),new GamePositon(5,0,true),new GamePositon(6,0,true),new GamePositon(7,0,true),

                //White Pieces
                new GamePositon(0,7,false),new GamePositon(1,7,false),new GamePositon(2,7,false),new GamePositon(3,7,false),
                new GamePositon(4,7,false),new GamePositon(5,7,false),new GamePositon(6,7,false),new GamePositon(7,7,false)
            };
        }

        public void Move(GameMove move)
        {
            var piece = this.First(x => x.X == move.StartPosition.X && x.Y == move.StartPosition.Y);

            piece.X = move.NextPosition.X;
            piece.Y = move.NextPosition.Y;
        }

        public override string ToString()
        {
            var orderedPieces = from x in this
                orderby x.IsBlack, x.X, x.Y
                select x.X + "" + x.Y;
            return string.Join("", orderedPieces);
        }
    }
}