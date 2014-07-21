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
	[PluginInfo(Name = "PlayClip", Category = "LiveOSC", Tags = "", AutoEvaluate=true)]
	#endregion PluginInfo
	public class PlayClipNode : IPluginEvaluate
	{
		#region fields & pins
		[Input("Connection")]
		public ISpread<OSCTransmitter> FInConnection;
		
		[Input("Track", MinValue=1)]
		public ISpread<int> FInTrack;
		
		[Input("Clip")]
		public ISpread<int> FInClip;
		
		[Input("Play", IsBang=true)]
		public ISpread<bool> FInPlay;
		
		[Input("Stop", IsBang=true)]
		public ISpread<bool> FInStop;
		
		[Import()]
		public ILogger FLogger;
		#endregion fields & pins

		PlayClipNode()
		{
		}
		//called when data for any output pin is requested
		public void Evaluate(int SpreadMax)
		{
			if (FInConnection[0] == null) {
				return;
			}
			var client = FInConnection[0];
			for(int i=0; i<SpreadMax; i++)
			{
				if (FInPlay[i])
				{
					var msg = new OSCMessage("/live/play/clip");
					msg.Append( (int) FInTrack[i]);
					msg.Append( (int) FInClip[i] );
					client.Send(msg);
				}
				
				if (FInStop[i])
				{
					var msg = new OSCMessage("/live/stop/clip");
					msg.Append( (int) FInTrack[i]);
					msg.Append( (int) FInClip[i] );
					client.Send(msg);
				}
			}
		}
	}
}
