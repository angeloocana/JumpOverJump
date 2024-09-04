using System.Web.Mvc;

namespace JumpOverJump.Web.Controllers
{
    public class HomeController : Controller
    {
        public ActionResult Index()
        {
            return new FilePathResult("~/index.html", "text/html");
        }
    }
}