using System;
using System.IO;
using System.Threading.Tasks;
using System.Web.Http;
using JumpOverJump.Core;

namespace JumpOverJump.Web.Controllers
{
    public class GameController : ApiController
    {
        [HttpPost]
        public async Task<string> Save(Game game)
        {
            try
            {
                var line = game.GetLine();
                if (string.IsNullOrEmpty(line))
                    return "Not Ended";

                await FileHelper.WriteFile(Game.SavePath, DateTime.Now.Ticks + "_" + Guid.NewGuid() + ".txt", line);

                return "OK";
            }
            catch (Exception ex)
            {
                return ex.Message;
            }
        }

        [HttpPost]
        public BestMovesToWin BestMove(GameInProgress game)
        {
            var bestMove = game.GetBestMovesToWin();
            return bestMove;
        }
    }
}