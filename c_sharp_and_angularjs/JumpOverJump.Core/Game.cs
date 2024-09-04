using System.Collections.Generic;
using System.Linq;

namespace JumpOverJump.Core
{
    public class Game
    {
        public Game()
        {

        }

        public static int InvertNumber(int n)
        {
            switch (n)
            {
                case 7:
                    return 0;
                case 6:
                    return 1;
                case 5:
                    return 2;
                case 4:
                    return 3;
                case 3:
                    return 4;
                case 2:
                    return 5;
                case 1:
                    return 6;
                case 0:
                    return 7;
                default:
                    return 9;
            }
        }

        public static char InvertNumber(char n)
        {
            switch (n)
            {
                case '7':
                    return '0';
                case '6':
                    return '1';
                case '5':
                    return '2';
                case '4':
                    return '3';
                case '3':
                    return '4';
                case '2':
                    return '5';
                case '1':
                    return '6';
                case '0':
                    return '7';
                default:
                    return '_';
            }
        }

        public string GetInvertedLine(string line)
        {
            return string.Join("",line.Select(InvertNumber));
        }

        public Game(string line, bool isWhitePerpective)
        {
            Ended = true;
            BlackWin = line.Substring(0, 1) == "1";
            IsWhitePerpective = isWhitePerpective;

            //RemoveBlackWin
            line = line.Substring(1);

            if (!isWhitePerpective)
            {
                BlackWin = !BlackWin;
                line = GetInvertedLine(line);
            }

            Movements = new List<GameMove>();
            var isBlack = false;
            do
            {
                var move = line.Substring(0, 4);
                Movements.Add(new GameMove(move, isBlack));

                isBlack = !isBlack;
                line = line.Substring(4);
            } while (!string.IsNullOrEmpty(line));
        }

        public const string SavePath = "a:\\jojGames\\";
        public const string MovesToWinPath = "a:\\jojMovesToWin\\";

        public bool Ended { get; set; }
        public List<GameMove> Movements { get; set; }
        public bool BlackWin { get; set; }
        public bool IsWhitePerpective { get; set; }

        public string GetLine()
        {
            if (!Ended)
                return "";

            var line = BlackWin ? "1" : "0";
            return line + string.Join("", Movements.Select(x => x.ToString()));
        }

        public IEnumerable<GameMovesToWin> GetMovesToWin()
        {
            var pieces = GamePieces.GetStartPositons();

            for (var i = 0; i < Movements.Count; i++)
            {
                var currentPieces = pieces.ToString();
                var move = Movements[i];

                pieces.Move(move);

                var whiteTurn = i % 2 == 0;

                var nMovesToFinish = Movements.Count - i;

                if (whiteTurn && IsWhitePerpective)
                    yield return new GameMovesToWin(move, currentPieces, nMovesToFinish, !BlackWin);
                else if (!whiteTurn && !IsWhitePerpective)
                    yield return new GameMovesToWin(move, currentPieces, nMovesToFinish, BlackWin);
            }
        }
    }
}