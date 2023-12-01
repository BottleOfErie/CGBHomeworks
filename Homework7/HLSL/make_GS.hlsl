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
		float3 normalW = mul(vIn.NormalL, (float3x3) g_WorldInvTranspose);
		if (g_IsReflection)
    	{
			posW = mul(posW, g_Reflection);
			normalW = mul(normalW, (float3x3) g_Reflection);
    	}

		element.PosH = mul(posW, viewProj);
		element.PosW = posW.xyz;
		element.NormalW = normalW;
		element.Color = vIn.Color;
		output.Append(element);
	}
	output.RestartStrip();
	uint id[3] = { 1,0,2 };
	[unroll]
	for (uint i = 0; i < 3; i++)
	{
		VertexIn vIn = input[id[i]];
		vIn.NormalL.x = -vIn.NormalL.x;
		float4 posW = mul(float4(vIn.PosL, 1.0f), g_World);
		float3 normalW = mul(vIn.NormalL, (float3x3) g_WorldInvTranspose);
		
		
		if (g_IsReflection)
    	{
			posW = mul(posW, g_Reflection);
			normalW = mul(normalW, (float3x3) g_Reflection);
    	}
		posW.x = 100 - posW.x;

		element2.PosH = mul(posW, viewProj);
		element2.PosW = posW.xyz;
		element2.NormalW = normalW;
		element2.Color = float4(0.5, 0.5, 0.5, 1.0);
		output.Append(element2);
	}
}