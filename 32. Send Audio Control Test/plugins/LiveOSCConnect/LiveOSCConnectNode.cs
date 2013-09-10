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
	[PluginInfo(Name = "Connect", Category = "LiveOSC", Tags = "")]
	#endregion PluginInfo
	public class ConnectNode : IPluginEvaluate
	{
		#region fields & pins
		[Input("IP Address", IsSingle=true)]
		public IDiffSpread<string> FInAddress;

		[Input("Connect", IsSingle=true)]
		public IDiffSpread<bool> FInConnect;
		
		[Output("Connection")]
		public ISpread<OSCTransmitter> FOutConnection;
		
		[Output("Status")]
		public ISpread<string> FOutStatus;

		[Import()]
		public ILogger FLogger;
		#endregion fields & pins

		OSCTransmitter FConnection;
		
		ConnectNode()
		{
		}
		//called when data for any output pin is requested
		public void Evaluate(int SpreadMax)
		{
			if (FInAddress.IsChanged || FInConnect.IsChanged)
			{
				try
				{
					if (FInConnect[0] != (FConnection != null)) {
						if (!FInConnect[0]) {
							Disconnect();
						} else {
							Connect();
						}
					}
					FOutStatus[0] = "OK";
				}
				catch(Exception e)
				{
					Disconnect();
					FOutStatus[0] = e.Message;
				}
				FOutConnection[0] = FConnection;
			}
		}
		
		void Connect()
		{
			Disconnect();
			FConnection = new OSCTransmitter(FInAddress[0], 9000);
			FConnection.Connect();
		}
		
		void Disconnect()
		{
			if (FConnection != null) 
			{
				FConnection.Close();
			}
			FConnection = null;
		}
	}
}
