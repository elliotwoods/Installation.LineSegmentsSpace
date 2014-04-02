#region usings
using System;
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
	[PluginInfo(Name = "SiftIndex", Category = "Value", Help = "Basic template with one value in/out", Tags = "")]
	#endregion PluginInfo
	public class ValueSiftIndexNode : IPluginEvaluate
	{
		#region fields & pins
		[Input("Input")]
		public ISpread<int> FInput;

		[Input("Search")]
		public ISpread<int> FSearch;
		
		[Input("Default", DefaultValue = -1, IsSingle=true)]
		public ISpread<int> FDefault;
		
		[Output("Index")]
		public ISpread<double> FOutIndex;

		[Output("Found")]
		public ISpread<bool> FOutFound;

		[Import()]
		public ILogger FLogger;
		#endregion fields & pins

		//called when data for any output pin is requested
		public void Evaluate(int SpreadMax)
		{
			FOutIndex.SliceCount = FInput.SliceCount;
			FOutFound.SliceCount = FInput.SliceCount;

			for (int i = 0; i < FInput.SliceCount; i++)
			{
				bool found = false;
				for(int j=0; j<FSearch.SliceCount; j++)
				{
					if (FInput[i] == FSearch[j])
					{
						FOutIndex[i] = j;
						found = true;
						break;
					}
				}
				if (!found)
				{
					FOutIndex[i] = FDefault[0];
				}
				FOutFound[i] = found;
			}
		}
	}
}
