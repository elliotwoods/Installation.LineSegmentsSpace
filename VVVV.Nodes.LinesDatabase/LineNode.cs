#region usings
using System;
using System.ComponentModel.Composition;

using VVVV.PluginInterfaces.V1;
using VVVV.PluginInterfaces.V2;
using VVVV.Utils.VColor;
using VVVV.Utils.VMath;

using Unosquare.Labs.EmbedIO;
using Unosquare.Labs.EmbedIO.Constants;
using Unosquare.Labs.EmbedIO.Modules;
using Unosquare.Net;
using Newtonsoft.Json;

using VVVV.Core.Logging;
using System.Collections.Generic;
using System.Linq;
#endregion usings

namespace VVVV.Nodes.LinesDatabase
{
    #region PluginInfo
    [PluginInfo(Name = "Line", Category = "Lines", Tags = "")]
    #endregion PluginInfo
    public class LineNode : IPluginEvaluate
    {
        #region fields & pins
        [Input("Line")]
        public ISpread<Line> FInLines;

        [Output("LineIndex")]
        public ISpread<int> FOutLineIndex;

        [Output("ProjectorIndex")]
        public ISpread<int> FOutProjectorIndex;

        [Output("Start")]
        public ISpread<Vector2D> FOutStart;

        [Output("End")]
        public ISpread<Vector2D> FOutEnd;

        [Output("Start World")]
        public ISpread<Vector3D> FOutStartWorld;

        [Output("End World")]
        public ISpread<Vector3D> FOutEndWorld;

        [Output("Last Update")]
        public ISpread<DateTime> FOutLastUpdate;

        [Output("Last Edit By")]
        public ISpread<string> FOutLastEditBy;

        [Output("Age")]
        public ISpread<double> FOutAge;

        [Import()]
        public ILogger FLogger;

        #endregion fields & pins

        //called when data for any output pin is requested
        public void Evaluate(int SpreadMax)
        {
            if(FInLines.SliceCount == 1 && FInLines[0] == null)
            {
                SpreadMax = 0;
            }

            FOutLineIndex.SliceCount = SpreadMax;
            FOutProjectorIndex.SliceCount = SpreadMax;
            FOutStart.SliceCount = SpreadMax;
            FOutEnd.SliceCount = SpreadMax;
            FOutStartWorld.SliceCount = SpreadMax;
            FOutEndWorld.SliceCount = SpreadMax;
            FOutLastUpdate.SliceCount = SpreadMax;
            FOutLastEditBy.SliceCount = SpreadMax;
            FOutAge.SliceCount = SpreadMax;

            for (int i=0; i<SpreadMax; i++)
            {
                var line = FInLines[i];
                if(line == null)
                {
                    continue;
                }

                FOutLineIndex[i] = line.LineIndex;
                FOutProjectorIndex[i] = line.ProjectorIndex;
                FOutStart[i] = line.Start;
                FOutEnd[i] = line.End;
                FOutStartWorld[i] = line.StartWorld;
                FOutEndWorld[i] = line.EndWorld;
                FOutLastUpdate[i] = line.LastUpdate;
                FOutLastEditBy[i] = line.LastEditBy;
                FOutAge[i] = line.Age;
            }
        }
    }
}
