using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace JumpOverJump.Core
{
    public class FileHelper
    {
        public static async Task WriteFile(string folder, string fileName, List<string> lines)
        {
            if (!Directory.Exists(folder))
                Directory.CreateDirectory(folder);

            using (var file = new StreamWriter(folder + fileName))
            {
                foreach (var line in lines)
                {
                    await file.WriteLineAsync(line);
                }
            }
        }

        public static async Task WriteFile(string folder, string fileName, string line)
        {
            await WriteFile(folder, fileName, new List<string> { line });
        }

        public static IEnumerable<string> ReadFiles(string folder)
        {
            var files = Directory.GetFiles(folder);
            foreach (var fileName in files)
            {
                using (var file = new StreamReader(fileName))
                {
                    var line = file.ReadLine();
                    while (!string.IsNullOrEmpty(line))
                    {
                        yield return line;
                        line = file.ReadLine();
                    }
                }
            }
        }
    }
}
