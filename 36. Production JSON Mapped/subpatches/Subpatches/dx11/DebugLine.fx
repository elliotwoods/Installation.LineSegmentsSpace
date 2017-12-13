struct vsInput
{
	float4 PosO : POSITION;
};

struct psInput
{
    float4 PosWVP: SV_POSITION;
};

cbuffer cbPerDraw : register(b0)
{
	float4x4 tVP : LAYERVIEWPROJECTION;
	int ViewportIndex : VIEWPORTINDEX;
};

cbuffer cbPerObj : register( b1 )
{
	float4x4 tW : WORLD;
	int ProjectorIndex;
	float2 Start;
	float2 End;
	float4 Color;
};

psInput VS(vsInput input)
{
	psInput output;
	
	output.PosWVP  = mul(input.PosO,mul(tW,tVP));
	//output.posScreen *= ProjectorIndex[input.ii] == ViewportIndex;
	return output;
}

float4 PS(psInput input): SV_Target
{
    float4 col = Color;
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
