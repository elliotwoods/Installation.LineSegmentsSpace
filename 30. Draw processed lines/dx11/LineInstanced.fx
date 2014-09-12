//@author: elliotwoods
//@help: for line mapping
//@credits: 

StructuredBuffer<float3> LineStart;
StructuredBuffer<float3> LineEnd;
StructuredBuffer<float> TextureCoord;
StructuredBuffer<uint> ViewPortSelection;

Texture2D Texture;

SamplerState g_samLinear : IMMUTABLE
{
    AddressU = Clamp;
    AddressV = Clamp;
};

 
cbuffer cbPerDraw : register( b0 )
{
	float4x4 tVP : VIEWPROJECTION;
	int ViewPortCurrent : VIEWPORTINDEX;
	float4x4 tWVP : WORLDVIEWPROJECTION;
};


cbuffer cbPerObj : register( b1 )
{
	float4x4 tW : WORLD;
	float Alpha <float uimin=0.0; float uimax=1.0;> = 1; 
};

bool ViewPortSelected(int InstanceIndex)
{
	int ViewPortSelect = ViewPortSelection[InstanceIndex];
	return ViewPortSelect == -1 || ViewPortSelect == ViewPortCurrent;
}

struct VS_Input
{
	uint VertexIndex : SV_VertexID;
};

struct GS_Input
{
	bool Allow : TEXCOORD0;
    int VertexIndex: TEXCOORD1;
};

struct PS_Input
{
	float4 PosWVP : SV_POSITION;
	float2 TexCd : TEXCOORD0;
};

GS_Input VS(VS_Input input)
{
	GS_Input output;
	
	uint instanceIndex = input.VertexIndex;
    GS_Input Out = (GS_Input)0;
	
	Out.Allow = ViewPortSelected(instanceIndex);
	Out.VertexIndex = input.VertexIndex;
	
    return Out;
}

[maxvertexcount(2)]
void GS(point GS_Input inputs[1], inout LineStream<PS_Input> lines)
{
    PS_Input output;
	
	//Extrude point to line using our data fetched in GS
	GS_Input input = inputs[0];
	if (input.Allow) {
		float4 pos;
		pos.xyz = LineStart[input.VertexIndex];
		pos.w = 1.0f;
		output.PosWVP = mul(pos, tWVP);
		output.TexCd = float2(0.0f, TextureCoord[input.VertexIndex]);
		lines.Append(output);
		
		pos.xyz = LineEnd[input.VertexIndex];
		output.PosWVP = mul(pos, tWVP);
		output.TexCd.x = 1.0f;
		lines.Append(output);
		lines.RestartStrip();
	}
}

float4 PS(PS_Input In): SV_Target
{
    return Texture.Sample(g_samLinear, In.TexCd);
}



technique10 Constant
{
	pass P0
	{
		SetHullShader( 0 );
		SetDomainShader( 0 );
		SetGeometryShader( CompileShader( gs_5_0, GS() ) );
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetPixelShader( CompileShader( ps_5_0, PS() ) );
	}
}




