struct vsInput
{
	uint ii : SV_InstanceID; //Object instance ID
    float4 posObject : POSITION;
};

StructuredBuffer<float3> StartWorld;
StructuredBuffer<float3> EndWorld;

Texture2D inputTexture <string uiname="Texture";>;

SamplerState Sampler <string uiname="Sampler State";>
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

struct psInput
{
    float4 posScreen : SV_Position;
	uint ii : SV_InstanceID;
    float2 uv: TEXCOORD0;
};

cbuffer cbPerDraw : register(b0)
{
	float4x4 tVP : LAYERVIEWPROJECTION;
	int ViewportIndex : VIEWPORTINDEX;
};

cbuffer cbPerObj : register( b1 )
{
	float4x4 tW : WORLD;
	int CountUpPow2;
};

psInput VS(vsInput input)
{
	psInput output;
	float3 start = StartWorld[input.ii];
	float3 end = EndWorld[input.ii];
	
	float4 position;
	position.xyz = lerp(start, end, input.posObject.x);
	position.w = 1.0f;
	
	output.posScreen = mul(position,mul(tW,tVP));
	output.ii = input.ii;

	output.uv = float2(input.posObject.x
		, (float(input.ii) + 0.5f) / float(CountUpPow2));
	return output;
}

float4 PS(psInput input): SV_Target
{
    float4 col = inputTexture.Sample(Sampler,input.uv.xy);;
    return col;
}


technique11 Line
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}