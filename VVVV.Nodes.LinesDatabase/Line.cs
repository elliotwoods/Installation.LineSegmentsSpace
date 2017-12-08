using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using VVVV.Utils.VMath;

namespace VVVV.Nodes.LinesDatabase
{
    public class Line
    {
        public int LineIndex;
        public int ProjectorIndex;
        public Vector2D Start = new Vector2D();
        public Vector2D End = new Vector2D();

        public Vector3D StartWorld = new Vector3D();
        public Vector3D EndWorld = new Vector3D();

        public DateTime LastUpdate = DateTime.Now;

        public string LastEditBy = "";
        public double Age = 100.0f;
    }
}
