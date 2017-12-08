#region usings
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;

using VVVV.PluginInterfaces.V1;
using VVVV.PluginInterfaces.V2;
using VVVV.Utils.VColor;
using VVVV.Utils.VMath;

using VVVV.Core.Logging;
#endregion usings

namespace VVVV.Nodes
{
	#region PluginInfo
	[PluginInfo(Name = "Filter", Category = "Value", Help = "Basic template with one value in/out", Tags = "c#")]
	#endregion PluginInfo
	public class ValueFilterNode : IPluginEvaluate
	{
		#region fields & pins
		[Input("Input")]
		public ISpread<int> FInput;
		
		[Input("Filter")]
		public ISpread<int> FInFilter;

		[Output("Output")]
		public ISpread<bool> FOutput;

		[Import()]
		public ILogger FLogger;
		#endregion fields & pins

		//called when data for any output pin is requested
		public void Evaluate(int SpreadMax)
		{
			FOutput.SliceCount = FInput.SliceCount;

			HashSet<double> filter = new HashSet<double>();
			foreach(var value in FInFilter) {
				filter.Add(value);
			}
			for (int i = 0; i < SpreadMax; i++) {
				
				FOutput[i] = filter.Contains(FInput[i]);
			}

			//FLogger.Log(LogType.Debug, "hi tty!");
		}
	}
}
