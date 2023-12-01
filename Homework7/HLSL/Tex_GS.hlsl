#include "Light.hlsli"
[maxvertexcount(6)]
void GS(
	triangle VertexInTex input[3],
	inout TriangleStream< VertexOutTex > output
)
{
	matrix viewProj = mul(g_View, g_Proj);
	VertexOutTex element = (VertexOutTex)0;
	VertexOutTex element2 = (VertexOutTex)0;
	[unroll];
	for (uint i = 0; i < 3; i++)
	{
		VertexInTex vIn = input[i];
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
		//element.Color = vIn.Color;
		//贴图
		element.Tex = vIn.Tex;
		output.Append(element);
	}
	 output.RestartStrip();
	[unroll];
	uint id[3] = { 1,0,2 };
	for (uint i = 0; i < 3; i++)
	{
		VertexInTex vIn = input[id[i]];
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
		element2.Tex = vIn.Tex;
		output.Append(element2);
	}
}