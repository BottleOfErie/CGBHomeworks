#include "Light.hlsli"
[maxvertexcount(3)]
void GS(
	triangle VertexInTex input[3]: POSITION,
	inout TriangleStream< VertexOutTex > output
)
{
	matrix viewProj = mul(g_View, g_Proj);
	VertexOutTex element = (VertexOutTex)0;
	uint id[3] = { 1,0,2 };
	[unroll]
	for (uint i = 0; i < 3; i++)
	{
		VertexInTex vIn = input[id[i]];
		float4 posW = mul(float4(vIn.PosL, 1.0f), g_World);
		float3 normalW = mul(-vIn.NormalL, (float3x3) g_WorldInvTranspose);
		element.PosH = mul(posW, viewProj);
		element.PosW = posW.xyz;
		element.NormalW = normalW;
		element.Tex = vIn.Tex;
		output.Append(element);
	}
	output.RestartStrip();
}