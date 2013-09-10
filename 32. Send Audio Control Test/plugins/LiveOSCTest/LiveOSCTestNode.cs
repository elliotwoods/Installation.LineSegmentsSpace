#region usings
using System;
using System.ComponentModel.Composition;

using VVVV.PluginInterfaces.V1;
using VVVV.PluginInterfaces.V2;
using VVVV.Utils.VColor;
using VVVV.Utils.VMath;

using VVVV.Core.Logging;
using VVVV.Utils.OSC;

#endregion usings

namespace VVVV.Nodes
{
	#region PluginInfo
	[PluginInfo(Name = "Test", Category = "LiveOSC", Help = "Basic template with one value in/out", Tags = "")]
	#endregion PluginInfo
	public class LiveOSCSendNode : IPluginEvaluate
	{
		#region fields & pins
		[Input("Track")]
		public ISpread<int> FInTrack;

		[Input("Send")]
		public ISpread<int> FInSend;
		
		[Input("Level")]
		public ISpread<float> FInLevel;
		
		[Input("Do Send", IsBang=true)]
		public ISpread<bool> FInDo;
		
		[Output("Status")]
		public ISpread<string> FOutStatus;

		[Import()]
		public ILogger FLogger;
		#endregion fields & pins

		OSCTransmitter client;
		
		LiveOSCSendNode()
		{
			client = new OSCTransmitter("192.168.100.2", 9000);
		}
		//called when data for any output pin is requested
		public void Evaluate(int SpreadMax)
		{
			try
			{
				for(int i=0; i<SpreadMax; i++)
				{
					if (FInDo[i])
					{
						var msg = new OSCMessage("/live/send");
						msg.Append((int) FInTrack[i]);

						msg.Append((int) FInSend[i]);

						msg.Append((float) FInLevel[i]);
						client.Send(msg);
					}
				}
				FOutStatus[0] = "OK";
			}
			catch (Exception e)
			{
				FOutStatus[0] = e.Message;
			}
			//FLogger.Log(LogType.Debug, "hi tty!");
		}
	}
}
