struct vsInput
{
	uint ii : SV_InstanceID; //Object instance ID
    float4 posObject : POSITION;
    float2 uv: TEXCOORD0;
};

int ProjectorIndex;

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
	float4 cAmb <bool color=true;String uiname="Color";> = { 1.0f,1.0f,1.0f,1.0f };
};

psInput VS(vsInput input)
{
	psInput output;
	
	output.posScreen = mul(input.posObject, mul(tW,tVP));

	output.uv = input.uv;
	
	if(ProjectorIndex != ViewportIndex) {
		output.posScreen = 0;
	}
	
	return output;
}

float4 PS(psInput input): SV_Target
{
    float4 col = inputTexture.Sample(Sampler,input.uv.xy);;
    return col * cAmb;
}


technique11 DrawInViewport
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}