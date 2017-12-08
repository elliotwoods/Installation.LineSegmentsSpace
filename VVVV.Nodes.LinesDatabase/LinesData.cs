using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VVVV.Nodes.LinesDatabase
{
    public class LinesData
    {
        public List<Line> Lines = new List<Line>();
        public Line NewLine()
        {
            var line = new Line();
            line.LineIndex = this.NextFreeIndex();
            return line;
        }

        public int NextFreeIndex()
        {
            if (this.Lines.Count == 0)
            {
                return 0;
            }
            else
            {
                var existingIDs = new HashSet<int>();
                foreach (var existingLine in this.Lines)
                {
                    existingIDs.Add(existingLine.LineIndex);
                }
                var lastIndex = existingIDs.Last<int>();
                return lastIndex + 1;
            }
        }

        public Line GetLine(int lineIndex)
        {
            foreach (var line in this.Lines)
            {
                if (line.LineIndex == lineIndex)
                {
                    return line;
                }
            }
            throw (new Exception("Line not found"));
        }

        public void AddLine(Line line)
        {
            line.LineIndex = this.NextFreeIndex();
            this.Lines.Add(line);
        }

        public void Save(string Filename)
        {
            System.IO.File.WriteAllText(Filename, JsonConvert.SerializeObject(this.Lines));
        }

        public void Load(string Filename)
        {
            this.Lines = JsonConvert.DeserializeObject<List<Line>>(System.IO.File.ReadAllText(Filename));
        }

        public static LinesData Instance = new LinesData();
    }
}
