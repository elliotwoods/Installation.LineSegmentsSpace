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
	[PluginInfo(Name = "FindCorners", Category = "2D", Help = "Basic template with one value in/out", Tags = "c#")]
	#endregion PluginInfo
	public class C2DFindCornersNode : IPluginEvaluate
	{
		#region fields & pins
		[Input("Projector Index", IsSingle=true)]
		public IDiffSpread<int> FInProjectorIndex;
		
		[Input("Lines")]
		public IDiffSpread<ISpread<Vector2D>> FInLines;

		[Input("iProjector")]
		public IDiffSpread<int> FInIProjector;
		
		[Input("Targets")]
		public IDiffSpread<Vector2D> FInTargets;
		
		[Output("Output")]
		public ISpread<Vector2D> FOutput;

		[Import()]
		public ILogger FLogger;
		#endregion fields & pins

		//called when data for any output pin is requested
		public void Evaluate(int SpreadMax)
		{
			if(FInLines.IsChanged
			|| FInIProjector.IsChanged
			|| FInProjectorIndex.IsChanged
			|| FInTargets.IsChanged) {
				//setup distances and output
				var distanceToTargets = new List<double>();
				foreach(var target in FInTargets) {
					distanceToTargets.Add(double.MaxValue);
				}
				FOutput.SliceCount = FInTargets.SliceCount;
				
				//check all corners
				for(int i=0; i<FInLines.SliceCount; i++) {
					if(FInIProjector[i] != FInProjectorIndex[i]) {
						//not a matching line
						continue;
					}
					
					var line = FInLines[i];
					foreach(var corner in line) {
						for(int targetIndex = 0; targetIndex < FInTargets.SliceCount; targetIndex++) {
							var target = FInTargets[targetIndex];
							var distanceToTarget = (target - corner).LengthSquared;
							if(distanceToTarget < distanceToTargets[targetIndex]) {
								distanceToTargets[targetIndex] = distanceToTarget;
								FOutput[targetIndex] = corner;
							}
						}
					}
				}
			}
			//FLogger.Log(LogType.Debug, "hi tty!");
		}
	}
}
