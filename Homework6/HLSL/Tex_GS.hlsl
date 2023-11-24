#include "Light.hlsli"
[maxvertexcount(3)]
void GS(
	triangle VertexInTex input[3]: POSITION,
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
		element.PosH = mul(posW, viewProj);
		element.PosW = posW.xyz;
		element.NormalW = mul(vIn.NormalL, (float3x3) g_WorldInvTranspose);
		//element.Color = vIn.Color;
		//贴图
		element.Tex = vIn.Tex;
		output.Append(element);
	}
	// output.RestartStrip();
	// [unroll];
	// uint id[3] = { 1,0,2 };
	// for (uint i = 0; i < 3; i++)
	// {
	// 	VertexInTex vIn = input[id[i]];
	// 	float4 posW = mul(float4(vIn.PosL, 1.0f), g_World);
	// 	posW.x = 500 - posW.x;
	// 	element2.PosH = mul(posW, viewProj);
	// 	element2.PosW = posW.xyz;
	// 	vIn.NormalL.z = -vIn.NormalL.z;
	// 	element2.NormalW = mul(vIn.NormalL, (float3x3) g_WorldInvTranspose);
	// 	//element2.Color = float4(0.5, 0.5, 0.5, 1.0);
	// 	//贴图
	// 	element2.Tex = vIn.Tex;
	// 	output.Append(element2);
	// }
}