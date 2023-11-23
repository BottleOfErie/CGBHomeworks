#include "NameVertices.h"

using namespace DirectX;
NameVertices::NameVertices(D3D11_PRIMITIVE_TOPOLOGY type)
{
	topology = type;

	verticesCount = 5344;
	indexCount = 24162;
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
}

NameVertices::~NameVertices()
{
	delete nameVertices;
}

GameApp::VertexPosNormalColor* NameVertices::GetNameVertices()
{
	return nameVertices;
}

WORD* NameVertices::GetNameIndices()
{
	return nameIndices;
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