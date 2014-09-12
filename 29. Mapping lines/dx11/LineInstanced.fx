//@author: elliotwoods
//@help: for line mapping
//@credits: 

StructuredBuffer<float3> LineStart;
StructuredBuffer<float3> LineEnd;
StructuredBuffer<float> Highlight;
StructuredBuffer<uint> ViewPortSelection;

SamplerState g_samLinear : IMMUTABLE
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

 
cbuffer cbPerDraw : register( b0 )
{
	float4x4 tVP : VIEWPROJECTION;
	int ViewPortCurrent : VIEWPORTINDEX;
};


cbuffer cbPerObj : register( b1 )
{
	float4x4 tW : WORLD;
	float Alpha <float uimin=0.0; float uimax=1.0;> = 1; 
	float4 cOrdinary <bool color=true;String uiname="Color Ordinary";> = { 1.0f,1.0f,1.0f,1.0f };
	float4 cHighlight <bool color=true;String uiname="Color Highligh";> = { 1.0f,1.0f,1.0f,1.0f };
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
	float4 col : COLOR;
	bool Allow : TEXCOORD0;
    int VertexIndex: TEXCOORD1;
};

struct PS_Input
{
	float4 PosWVP : SV_POSITION;
	float4 col : COLOR;
};

GS_Input VS(VS_Input input)
{
	GS_Input output;
	
	uint instanceIndex = input.VertexIndex;
    GS_Input Out = (GS_Input)0;
	
	float4 col = cOrdinary;
	Out.col = lerp(col, cHighlight, Highlight[instanceIndex]);
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
		output.PosWVP.xyz = LineStart[input.VertexIndex];
		output.PosWVP.w = 1.0f;
		output.col = input.col;
		lines.Append(output);
		output.PosWVP.xyz = LineEnd[input.VertexIndex];
		lines.Append(output);
		lines.RestartStrip();
	}
}

float4 PS(PS_Input In): SV_Target
{
    return In.col;
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




