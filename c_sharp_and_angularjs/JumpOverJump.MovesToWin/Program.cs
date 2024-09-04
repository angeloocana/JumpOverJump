using System;
using System.Linq;
using JumpOverJump.Core;

namespace JumpOverJump.MovesToWin
{
    class Program
    {
        static void Main(string[] args)
        {
            var nFile = 0;

            foreach (var line in FileHelper.ReadFiles(Game.SavePath))
            {
                nFile++;
                Console.WriteLine(nFile);
                {
                    var gameWhite = new Game(line, true);
                    MovesToFile(gameWhite);
                }

                {
                    var gameBlack = new Game(line, false);
                    MovesToFile(gameBlack);
                }
            }
        }

        public static void FillFile(string folder, string fileName, GameMovesToWin movesToWin)
        {
            var fileMovesToWin = Core.MovesToWin.GetFileMovesToWin(fileName).ToList();

            var moveToWinInFile = fileMovesToWin.FirstOrDefault(x =>
                x.Move.StartPosition.X == movesToWin.Move.StartPosition.X
                && x.Move.StartPosition.Y == movesToWin.Move.StartPosition.Y
                && x.Move.NextPosition.X == movesToWin.Move.NextPosition.X
                && x.Move.NextPosition.Y == movesToWin.Move.NextPosition.Y);

            if (moveToWinInFile == null)
                fileMovesToWin.Add(movesToWin);
            else
            {
                if (moveToWinInFile.Win == movesToWin.Win)
                {
                    if (moveToWinInFile.NMovesToFinish > movesToWin.NMovesToFinish)
                        fileMovesToWin[fileMovesToWin.IndexOf(moveToWinInFile)] = movesToWin;
                    else
                        return;
                }
                else if (!moveToWinInFile.Win && movesToWin.Win)
                    fileMovesToWin[fileMovesToWin.IndexOf(moveToWinInFile)] = movesToWin;
                else
                    return;
            }

            var lines = fileMovesToWin.Select(x => x.ToString()).ToList();
            FileHelper.WriteFile(folder, fileName, lines).Wait();
        }

        public static void MovesToFile(Game game)
        {
            foreach (var movesToWin in game.GetMovesToWin())
            {
                var folder = Game.MovesToWinPath + movesToWin.GetCurrentWhitePieces();

                FillFile(folder, "\\a.txt", movesToWin);
                FillFile(folder, "\\" + movesToWin.GetCurrentBlackPieces() + ".txt", movesToWin);
            }
        }
    }
}
