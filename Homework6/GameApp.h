#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "d3dApp.h"
#include "LightHelper.h"

class NameVertices;// 声明类， 避免头文件与类之间的相互依赖、、、、、、、、、、、、、、、、、、、、、、、、

class GameApp : public D3DApp
{
public:
	struct VertexPosNormalColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 color;
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
	};

	struct VSConstantBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
		DirectX::XMMATRIX worldInvTranspose;

	};

	struct PSConstantBuffer
	{
		DirectionalLight dirLight;
		PointLight pointLight;
		SpotLight spotLight;
		Material material;
		DirectX::XMFLOAT4 eyePos;
		DirectionalLight dirlight1;
		DirectionalLight dirlight2;
		DirectionalLight dirlight3;
		DirectionalLight dirlight4;
		DirectionalLight dirlight5;
	};

public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();


private:
	bool InitEffect();
	bool InitResource();
	void DrawName();
	void DrawMirror();


private:
	ComPtr<ID3D11InputLayout> m_pVertexLayout;	    // 顶点输入布局
	ComPtr<ID3D11Buffer> m_pVertexBuffer;			// 顶点缓冲区
	ComPtr<ID3D11Buffer> m_pIndexBuffer;			// 索引缓冲区
	ComPtr<ID3D11Buffer> m_pConstantBuffers[2];	    // 常量缓冲区

	ComPtr<ID3D11VertexShader> m_pVertexShader;	    // 顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;		// 像素着色器
	ComPtr<ID3D11GeometryShader> m_pGeometryShader; // 几何着色器
	VSConstantBuffer m_VSConstantBuffer;			// 用于修改用于VS的GPU常量缓冲区的变量
	PSConstantBuffer m_PSConstantBuffer;			// 用于修改用于PS的GPU常量缓冲区的变量
	
	DirectionalLight m_DirLight1;					// 五个方向光
	DirectionalLight m_DirLight2;
	DirectionalLight m_DirLight3;
	DirectionalLight m_DirLight4;
	DirectionalLight m_DirLight5;
	DirectionalLight m_DirLight;					// 默认环境光
	PointLight m_PointLight;						// 默认点光
	SpotLight m_SpotLight;						    // 默认汇聚光

	ComPtr<ID3D11RasterizerState> m_pRSWireframe;	// 光栅化状态: 线框模式
	bool m_IsWireframeMode;							// 当前是否为线框模式

	NameVertices* name;// 存储绘制名字的顶点、类型、、、、、、、、、、、、、、、、、、、、、、
	int nameN;
	int dirlightcount = 0;
	float angle;
	DirectX::XMFLOAT3 viewPos;
	DirectX::XMFLOAT3 viewRot;
};


#endif