struct vsInput
{
	uint ii : SV_InstanceID; //Object instance ID
    float4 posObject : POSITION;
};

StructuredBuffer<int> ProjectorIndex;
StructuredBuffer<float2> Start;
StructuredBuffer<float2> End;
StructuredBuffer<float4> Colors;

struct psInput
{
    float4 posScreen : SV_Position;
	uint ii : SV_InstanceID;
};

cbuffer cbPerDraw : register(b0)
{
	float4x4 tVP : LAYERVIEWPROJECTION;
	int ViewportIndex : VIEWPORTINDEX;
};

cbuffer cbPerObj : register( b1 )
{
	float4x4 tW : WORLD;
};

psInput VS(vsInput input)
{
	psInput output;
	float2 start = Start[input.ii];
	float2 end = End[input.ii];
	
	float4 position;
	position.xy = lerp(start, end, input.posObject.x);
	position.zw = float2(0, 1);
	
	output.posScreen = mul(position,mul(tW,tVP));
	output.ii = input.ii;
	output.posScreen *= ProjectorIndex[input.ii] == ViewportIndex;
	return output;
}

float4 PS(psInput input): SV_Target
{
    float4 col = Colors[input.ii];
    return col;
}


technique11 LineDebug
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}
