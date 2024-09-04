using Microsoft.Owin;
using Owin;

[assembly: OwinStartupAttribute(typeof(JumpOverJump.Startup))]
namespace JumpOverJump
{
    public partial class Startup
    {
        public void Configuration(IAppBuilder app)
        {
            ConfigureAuth(app);
        }
    }
}
