#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "d3dApp.h"
#include "LightHelper.h"
#include "RenderStates.h"

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
	struct VertexPosNormalTex
	{
	// VertexPosNormalTex() = default;

	// VertexPosNormalTex(const VertexPosNormalTex&) = default;
	// VertexPosNormalTex& operator=(const VertexPosNormalTex&) = default;

	// VertexPosNormalTex(VertexPosNormalTex&&) = default;
	// VertexPosNormalTex& operator=(VertexPosNormalTex&&) = default;

	// constexpr VertexPosNormalTex(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT3& _normal,
	// 	const DirectX::XMFLOAT2& _tex) :
	// 	pos(_pos), normal(_normal), tex(_tex) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 tex;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};

	

	struct VSConstantBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
		DirectX::XMMATRIX worldInvTranspose;
		int isReflection;

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
		DirectX::XMMATRIX reflect;
	};

	class GameObject
	{
	public:
		GameObject();

		// 获取位置
		DirectX::XMFLOAT3 GetPosition() const;
		// 设置缓冲区
		template<class VertexType, class IndexType>
		void SetBuffer(ID3D11Device * device, VertexType* vertices, UINT VertexCount, IndexType* indices, UINT IndexCount);
		void SetShaderLayout(const ComPtr<ID3D11VertexShader> VS,const ComPtr<ID3D11GeometryShader> GS ,const ComPtr<ID3D11PixelShader> PS,const ComPtr<ID3D11InputLayout> IL);
		// 设置纹理
		void SetTexture(ID3D11ShaderResourceView * texture);
		// 设置材质
		void SetMaterial(const Material & material);
		// 设置矩阵
		void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
		void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);
		// 绘制
		void Draw(ComPtr<ID3D11DeviceContext> deviceContext,VSConstantBuffer& m_VSConstantBuffer,PSConstantBuffer& m_PSConstantBuffer);

		// 设置调试对象名
		// 若缓冲区被重新设置，调试对象名也需要被重新设置
		void SetDebugObjectName(const std::string& name);
		bool isTex;	

	private:
		DirectX::XMFLOAT4X4 m_WorldMatrix;				    // 世界矩阵
		Material m_Material;								// 物体材质
		ComPtr<ID3D11ShaderResourceView> m_pTexture;		// 纹理
		ComPtr<ID3D11Buffer> m_pVertexBuffer;				// 顶点缓冲区
		ComPtr<ID3D11Buffer> m_pIndexBuffer;				// 索引缓冲区
		UINT m_VertexStride;								// 顶点字节大小
		UINT m_IndexCount;								    // 索引数目	
												// 是否有纹理
		ComPtr<ID3D11VertexShader> m_VS;	    				// 顶点着色器
		ComPtr<ID3D11PixelShader> m_PS;						// 像素着色器
		ComPtr<ID3D11GeometryShader> m_GS; 					// 几何着色器
		ComPtr<ID3D11InputLayout> m_IL; 						// 顶点输入布局
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
	void DrawForest(bool isReflection);
	void DrawName(bool isReflection);
	void DrawPic(bool isReflection);
	void DrawMirror();
	void DrawWall();


private:
	ComPtr<ID3D11InputLayout> m_pVertexLayout;	    // 顶点输入布局
	ComPtr<ID3D11InputLayout> m_pVertexLayout_Tex;	    // 顶点输入布局2
	ComPtr<ID3D11Buffer> m_pVertexBuffer[2];			// 顶点缓冲区
	ComPtr<ID3D11Buffer> m_pIndexBuffer[2];			// 索引缓冲区
	ComPtr<ID3D11Buffer> m_pConstantBuffers[2];	    // 常量缓冲区

	ComPtr<ID3D11VertexShader> m_pVertexShader;	    // 顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;		// 像素着色器
	ComPtr<ID3D11GeometryShader> m_pGeometryShader; // 几何着色器

	ComPtr<ID3D11VertexShader> m_pVertexShader_Tex;	    // 顶点着色器2
	ComPtr<ID3D11PixelShader> m_pPixelShader_Tex;		// 像素着色器2
	ComPtr<ID3D11GeometryShader> m_pGeometryShader_Tex; // 几何着色器2
	ComPtr<ID3D11GeometryShader> m_pGeometryShader_otherWall;//另一面墙
	ComPtr<ID3D11GeometryShader> m_pGeometryShader_Mirror; // 几何着色器3

	VSConstantBuffer m_VSConstantBuffer;			// 用于修改用于VS的GPU常量缓冲区的变量
	PSConstantBuffer m_PSConstantBuffer;			// 用于修改用于PS的GPU常量缓冲区的变量

	GameObject m_pFace;			    // 脸纹理
	GameObject m_pName;				// 名字
	GameObject m_pMirror; 			// 镜子
	GameObject m_pWall; 				// 墙
	GameObject m_pWall2;
	ComPtr<ID3D11SamplerState> m_pSamplerState;				    // 采样器状态
	
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