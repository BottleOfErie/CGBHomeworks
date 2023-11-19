#include "Light.hlsli"
[maxvertexcount(6)]
void GS(
	triangle VertexIn input[3]: POSITION,
	inout TriangleStream< VertexOut > output
)
{
	matrix viewProj = mul(g_View, g_Proj);
	VertexOut element = (VertexOut)0;
	VertexOut element2 = (VertexOut)0;
	[unroll]
	for (uint i = 0; i < 3; i++)
	{
		VertexIn vIn = input[i];
		float4 posW = mul(float4(vIn.PosL, 1.0f), g_World);
		element.PosH = mul(posW, viewProj);
		element.PosW = posW.xyz;
		element.NormalW = mul(vIn.NormalL, (float3x3) g_WorldInvTranspose);
		element.Color = vIn.Color;
		output.Append(element);
	}
	output.RestartStrip();
	[unroll]
	uint id[3] = { 1,0,2 };
	for (uint i = 0; i < 3; i++)
	{
		VertexIn vIn = input[id[i]];
		vIn.PosL.x = 50 - vIn.PosL.x;
		float4 posW = mul(float4(vIn.PosL, 1.0f), g_World);
		element2.PosH = mul(posW, viewProj);
		element2.PosW = posW.xyz;
		element2.NormalW = mul(vIn.NormalL, (float3x3) g_WorldInvTranspose);
		element2.Color = float4(0.5, 0.5, 0.5, 1.0);
		output.Append(element2);
	}
}