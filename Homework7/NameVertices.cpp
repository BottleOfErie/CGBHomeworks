#include "NameVertices.h"

using namespace DirectX;
NameVertices::NameVertices(D3D11_PRIMITIVE_TOPOLOGY type)
{
	topology = type;

	verticesCount = 5344;
	indexCount = 24156;
	/*for (int i = 0; i < 47436; i++) {
		nameVertices[i].color = nameVertices[nameIndices[i]].color;
		nameIndices[i] = i;
	}*/
	// 处理顶点颜色
	for (UINT i = 0; i < verticesCount; i++)
	{
		nameVertices[i].color.x = (nameVertices[i].pos.x  + 6.0f) /6.0f;
		nameVertices[i].color.y = (nameVertices[i].pos.y + 5.0f) /5.0f;
		nameVertices[i].color.z = nameVertices[i].pos.z + 1.0f;
		nameVertices[i].pos.x *= 3.0f;
		nameVertices[i].pos.y *= 3.0f;
		nameVertices[i].pos.z *= 3.0f;

		
	}
	for (UINT i = 0; i < 4; i++)
	{
		
		nameTexVertices[i].pos.y *= 5.0f;
		nameTexVertices[i].pos.z *= 5.0f;
	}
	
}

NameVertices::~NameVertices()
{
	delete nameVertices;
}

GameApp::VertexPosNormalColor* NameVertices::GetNameVertices()
{
	return nameVertices;
}
GameApp::VertexPosNormalTex* NameVertices::GetNameTexVertices()
{
	return nameTexVertices;
}
GameApp::VertexPosNormalTex* NameVertices::GetMirrorVertices()
{
	return mirrorVertices;
}
GameApp::VertexPosNormalTex* NameVertices::GetWallVertices()
{
	return wallVertices;
}
WORD* NameVertices::GetNameIndices()
{
	return nameIndices;
}
WORD* NameVertices::GetTexNameIndices()
{
	return TexNameIndices;
}
WORD* NameVertices::GetWallIndices()
{
	return wallIndices;
}
WORD* NameVertices::GetMirrorIndices()
{
	return mirrorIndices;
}
D3D11_PRIMITIVE_TOPOLOGY NameVertices::GetTopology()
{
	return topology;
}

UINT NameVertices::GetVerticesCount()
{
	return verticesCount;
}

UINT NameVertices::GetIndexCount()
{
	return indexCount;
}