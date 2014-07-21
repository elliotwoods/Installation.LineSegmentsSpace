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

namespace VVVV.Nodes.LiveOSC
{
	#region PluginInfo
	[PluginInfo(Name = "SetSend", Category = "LiveOSC", Tags = "", AutoEvaluate=true)]
	#endregion PluginInfo
	public class SetSendNode : IPluginEvaluate
	{
		#region fields & pins
		[Input("Connection")]
		public ISpread<OSCTransmitter> FInConnection;
		
		[Input("Track", MinValue=1)]
		public ISpread<int> FInTrack;
		
		[Input("Send")]
		public ISpread<int> FInSend;
		
		[Input("Level", MinValue=0.0, MaxValue=1.0)]
		public IDiffSpread<float> FInLevel;
		
		[Input("Force Update", IsSingle=true, IsBang=true)]
		ISpread<bool> FInForceUpdate;
		
		[Import()]
		public ILogger FLogger;
		#endregion fields & pins

		OSCTransmitter client = new OSCTransmitter("192.168.1.143", 9000);
		
		SetSendNode()
		{
		}
		//called when data for any output pin is requested
		public void Evaluate(int SpreadMax)
		{
			if (FInConnection[0] == null) {
				//return;
			} else {
				client = FInConnection[0];
			}
			//var client = FInConnection[0];
			if (FInLevel.IsChanged || FInForceUpdate[0])
			{
				for(int i=0; i<SpreadMax; i++)
				{
					var msg = new OSCMessage("/live/send");
					msg.Append((int) FInTrack[i]);
					msg.Append((int) FInSend[i]);
					msg.Append((float) FInLevel[i]);
					client.Send(msg);
				}
			}
		}
	}
}
