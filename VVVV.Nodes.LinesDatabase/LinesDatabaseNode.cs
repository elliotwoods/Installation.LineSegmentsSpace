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
    [PluginInfo(Name = "Database", Category = "Lines", Help = "Basic template with one value in/out", Tags = "", AutoEvaluate = true)]
	#endregion PluginInfo
	public class LinesDatabaseNode : IPluginEvaluate, IDisposable
	{
        #region fields & pins
        [Input("Load", IsBang =true, IsSingle =true)]
        public ISpread<bool> FInLoad;

        [Input("Save", IsBang = true, IsSingle = true)]
        public ISpread<bool> FInSave;

        [Input("Clear", IsBang = true, IsSingle = true)]
        public ISpread<bool> FInClear;

        [Input("Filename", IsSingle = true, StringType =StringType.Filename)]
        public ISpread<string> FInFilename;

        [Output("LinesData")]
        public ISpread<LinesData> FOutLinesData;

        [Output("Lines")]
		public ISpread<Line> FOutLines;

        [Output("Error")]
        public ISpread<string> FOutError;

		[Import()]
		public ILogger FLogger;

        WebServer FWebServer = null;
        #endregion fields & pins

        //called when data for any output pin is requested
        public void Evaluate(int SpreadMax)
		{
            if(FWebServer == null)
            {
                FWebServer = new WebServer("http://*:8000/", RoutingStrategy.Regex);
                FWebServer.RegisterModule(new WebApiModule());
                FWebServer.Module<WebApiModule>().RegisterController<APIController>();
                FWebServer.RunAsync();

                try
                {
                    LinesData.Instance.Load(FInFilename[0]);
                    FOutError[0] = "OK";
                }
                catch (Exception e)
                {
                    FOutError[0] = e.Message;
                }
                FOutLinesData[0] = LinesData.Instance;
            }

            if(FInLoad[0])
            {
                try
                {
                    LinesData.Instance.Load(FInFilename[0]);
                    FOutError[0] = "OK";
                }
                catch (Exception e)
                {
                    FOutError[0] = e.Message;
                }
            }

            if (FInSave[0])
            {
                try
                {
                    LinesData.Instance.Save(FInFilename[0]);
                    FOutError[0] = "OK";
                }
                catch (Exception e)
                {
                    FOutError[0] = e.Message;
                }
            }

            if (FInClear[0])
            {
                try
                {
                    LinesData.Instance.Lines.Clear();
                    FOutError[0] = "OK";
                }
                catch (Exception e)
                {
                    FOutError[0] = e.Message;
                }
            }

            FOutLines.AssignFrom(LinesData.Instance.Lines);
        }

        public void Dispose()
        {
            if(FWebServer != null)
            {
                FWebServer.Dispose();
                FWebServer = null;
            }
        }
    }
}
