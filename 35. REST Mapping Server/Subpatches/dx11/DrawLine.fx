Texture2D texture2d <string uiname="Texture";>;

SamplerState g_samLinear <string uiname="Sampler State";>
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

 
cbuffer cbPerDraw : register( b0 )
{
	float4x4 tVP : VIEWPROJECTION;
	int ViewportIndex : VIEWPORTINDEX;
};


cbuffer cbPerObj : register( b1 )
{
	float4x4 tW : WORLD;
	float Alpha <float uimin=0.0; float uimax=1.0;> = 1; 
	float4 cAmb <bool color=true;String uiname="Color";> = { 1.0f,1.0f,1.0f,1.0f };
	
	int ProjectorIndex;
	float2 Start;
	float2 End;
};

struct VS_IN
{
	float4 PosO : POSITION;
	float4 TexCd : TEXCOORD0;

};

struct vs2ps
{
    float4 PosWVP: SV_POSITION;
};

vs2ps VS(VS_IN input)
{
    vs2ps Out = (vs2ps)0;
	
	float4 position;
	position.xy = lerp(Start, End, input.PosO.x);
	position.zw = float2(0, 1);
	
    Out.PosWVP  = mul(position,mul(tW,tVP));
	Out.PosWVP *= ProjectorIndex == ViewportIndex;
    return Out;
}




float4 PS(vs2ps In): SV_Target
{
    float4 col = cAmb;
	col.a *= Alpha;
    return col;
}





technique10 DrawLine
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}




