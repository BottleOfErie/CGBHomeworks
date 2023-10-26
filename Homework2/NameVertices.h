#pragma once

#include "GameApp.h"

class NameVertices
{
public:
	NameVertices(D3D11_PRIMITIVE_TOPOLOGY type);
	~NameVertices();

	// 获取顶点
	GameApp::VertexPosColor* GetNameVertices();
	// 获取索引
	WORD* GetNameIndices();
	// 获取绘制图元类型
	D3D11_PRIMITIVE_TOPOLOGY GetTopology();
	// 获取顶点个数 
	UINT GetVerticesCount();
	// 获取索引个数
	UINT GetIndexCount();

private:
	GameApp::VertexPosColor* nameVertices; // 顶点
	WORD* nameIndices; // 索引
	D3D11_PRIMITIVE_TOPOLOGY topology;// 图元类型 
	UINT verticesCount; // 顶点个数
	UINT indexCount; // 索引个数
};
