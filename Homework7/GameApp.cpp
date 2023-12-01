#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include "NameVertices.h"
using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC GameApp::VertexPosNormalColor::inputLayout[3] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
const D3D11_INPUT_ELEMENT_DESC GameApp::VertexPosNormalTex::inputLayout[3] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance),
	m_VSConstantBuffer(),
	m_PSConstantBuffer(),
	m_DirLight(),
	m_DirLight1(),
	m_DirLight2(),
	m_DirLight3(),
	m_DirLight4(),
	m_DirLight5(),
	m_PointLight(),
	m_SpotLight(),
	name(nullptr),// 初始化变量，置空、、、、、、、、、、、、、、、、、、、、、
	nameN(7),// 设置nameN的值、、、、 origin：20
	angle(0),
	viewPos(0.0f, 60.0f, -100.0f),
	viewRot(0.7f, 0.0f, 0.0f) {
}

GameApp::~GameApp() {
	delete name;// 释放内存、、、、、、、、、、、、、、、、、、、、、、、、、、
}

bool GameApp::Init() {
	if (!D3DApp::Init())
		return false;

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	// 初始化鼠标，键盘不需要
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);

	return true;
}

void GameApp::OnResize() {
	D3DApp::OnResize();
}

void GameApp::UpdateScene(float dt) {
	angle += 1.5f * dt;

	// 获取鼠标状态
	Mouse::State mouseState = m_pMouse->GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	// 获取键盘状态
	Keyboard::State keyState = m_pKeyboard->GetState();
	Keyboard::State lastKeyState = m_KeyboardTracker.GetLastState();

	// 更新鼠标按钮状态跟踪器，仅当鼠标按住的情况下才进行移动
	m_MouseTracker.Update(mouseState);
	m_KeyboardTracker.Update(keyState);

	XMFLOAT3 pos = { 0, 0, 0 };
	float moveSpeed = 150.0f;
	if (keyState.IsKeyDown(Keyboard::LeftShift)) moveSpeed *= 2.0f;
	if (keyState.IsKeyDown(Keyboard::W)) pos.z += moveSpeed * dt;
	if (keyState.IsKeyDown(Keyboard::S)) pos.z -= moveSpeed * dt;
	if (keyState.IsKeyDown(Keyboard::A)) pos.x -= moveSpeed * dt;
	if (keyState.IsKeyDown(Keyboard::D)) pos.x += moveSpeed * dt;
	if (keyState.IsKeyDown(Keyboard::Q)) viewRot.z += 0.5f * dt;
	if (keyState.IsKeyDown(Keyboard::E)) viewRot.z -= 0.5f * dt;


	viewRot.y += mouseState.x * 0.005f;
	viewRot.x += mouseState.y * 0.005f;

	if (viewRot.x > 1.5f) viewRot.x = 1.5f;
	else if (viewRot.x < -1.5f) viewRot.x = -1.5f;
	if (viewRot.z > 0.7f) viewRot.z = 0.7f;
	else if (viewRot.z < -0.7f) viewRot.z = -0.7f;

	XMStoreFloat3(&pos, XMVector3Transform(XMLoadFloat3(&pos), XMMatrixRotationX(viewRot.x) * XMMatrixRotationZ(viewRot.z) * XMMatrixRotationY(viewRot.y)));
	viewPos.x += pos.x;
	viewPos.y += pos.y;
	viewPos.z += pos.z;
	m_VSConstantBuffer.view = XMMatrixTranspose(XMMatrixTranslation(-viewPos.x, -viewPos.y, -viewPos.z) * XMMatrixRotationY(-viewRot.y) * XMMatrixRotationZ(-viewRot.z) * XMMatrixRotationX(-viewRot.x));
	m_PSConstantBuffer.eyePos = { viewPos.x, viewPos.y, viewPos.z, 0.0f };


	// 初始化采样器状态
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(m_pd3dDevice->CreateSamplerState(&sampDesc, m_pSamplerState.GetAddressOf()));



	if (m_KeyboardTracker.IsKeyPressed(Keyboard::D3)) {
		m_PSConstantBuffer.dirLight = m_DirLight;
		m_PSConstantBuffer.pointLight = PointLight();
		m_PSConstantBuffer.spotLight = SpotLight();
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::D1)) {
		m_PSConstantBuffer.dirLight = DirectionalLight();
		m_PSConstantBuffer.pointLight = m_PointLight;
		m_PSConstantBuffer.spotLight = SpotLight();
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::D2)) {
		m_PSConstantBuffer.dirLight = DirectionalLight();
		m_PSConstantBuffer.pointLight = PointLight();
		m_PSConstantBuffer.spotLight = m_SpotLight;
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::OemPlus)) {
		dirlightcount++;
		if (dirlightcount == 1) {
			m_PSConstantBuffer.dirlight1 = m_DirLight1;
		}
		if (dirlightcount == 2) {
			m_PSConstantBuffer.dirlight2 = m_DirLight2;
		}
		if (dirlightcount == 3) {
			m_PSConstantBuffer.dirlight3 = m_DirLight3;
		}
		if (dirlightcount == 4) {
			m_PSConstantBuffer.dirlight4 = m_DirLight4;
		}
		if (dirlightcount == 5) {
			m_PSConstantBuffer.dirlight5 = m_DirLight5;
		}
		if (dirlightcount > 5) dirlightcount = 5;
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::OemMinus)) {
		if (dirlightcount == 1) {
			m_PSConstantBuffer.dirlight1 = DirectionalLight();
		}
		if (dirlightcount == 2) {
			m_PSConstantBuffer.dirlight2 = DirectionalLight();
		}
		if (dirlightcount == 3) {
			m_PSConstantBuffer.dirlight3 = DirectionalLight();
		}
		if (dirlightcount == 4) {
			m_PSConstantBuffer.dirlight4 = DirectionalLight();
		}
		if (dirlightcount == 5) {
			m_PSConstantBuffer.dirlight5 = DirectionalLight();
		}
		dirlightcount--;
		if (dirlightcount < 0)dirlightcount = 0;
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::F)) {
		m_IsWireframeMode = !m_IsWireframeMode;
		m_pd3dImmediateContext->RSSetState(m_IsWireframeMode ? m_pRSWireframe.Get() : nullptr);
	}

	m_PSConstantBuffer.pointLight.position.x = sinf(angle * 0.2f) * 20.0f;
	m_PSConstantBuffer.pointLight.position.z = sinf(angle * 0.5f) * 20.0f;

	m_PSConstantBuffer.spotLight.position = viewPos;
	XMFLOAT3 dir = { 0, 0, 1.0f };
	XMStoreFloat3(&m_PSConstantBuffer.spotLight.direction, XMVector3Transform(XMLoadFloat3(&dir), XMMatrixRotationX(viewRot.x) * XMMatrixRotationZ(viewRot.z) * XMMatrixRotationY(viewRot.y)));

}

void GameApp::DrawScene() {
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// m_PSConstantBuffer.material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	// m_PSConstantBuffer.material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	// m_PSConstantBuffer.material.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 100.0f);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[1].Get(), 0);

	m_VSConstantBuffer.world = XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0, 0.0f, 0.0f));
	m_VSConstantBuffer.worldInvTranspose = XMMatrixInverse(nullptr, XMMatrixTranspose(m_VSConstantBuffer.world));

	
	
	DrawMirror();

	DrawForest(true);
	DrawPic(true);

	DrawWall();

	DrawForest(false);	
	DrawPic(false);

	HR(m_pSwapChain->Present(0, 0));
}
void GameApp::DrawForest(bool isReflection)
{	
	m_VSConstantBuffer.world = XMMatrixTranspose(XMMatrixScaling(3.0f, 3.0f, 3.0f) * XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	DrawName(isReflection);
	m_VSConstantBuffer.world = XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	for (int i = 1; i <= nameN; i++) {
		for (int j = 1; j <= nameN; j++) {
			float scale = (sinf(angle * 3.0f + i * j * 0.2f) + 1.3f) * 0.15f;
			DirectX::XMMATRIX mScale = XMMatrixScaling(scale, scale, scale);
			DirectX::XMMATRIX mRotate = XMMatrixRotationX(angle) * XMMatrixRotationY(angle);
			DirectX::XMMATRIX mTranslate = XMMatrixTranslation((i - (nameN + 1) / 2.0f) * 4.5f * 4 * 0.5, 0, (j - (nameN + 1) / 2.0f) * 4.5f * 4 * 0.5);
			m_VSConstantBuffer.world = XMMatrixTranspose(mScale * mRotate * mTranslate); // mul(vec, mat) 中为行向量，矩阵右乘，顺序SRT, 参考https://www.cnblogs.com/X-Jun/p/9808727.html#_lab2_1_1
			m_VSConstantBuffer.worldInvTranspose = XMMatrixInverse(nullptr, XMMatrixTranspose(m_VSConstantBuffer.world));
			DrawName(isReflection);
			srand(i * j + 1);
			DirectX::XMMATRIX mScaleChild = XMMatrixScaling(0.3f, 0.3f, 0.3f);
			DirectX::XMMATRIX mTranslateChild = XMMatrixTranslation(2.5f, 0, 0);
			for (int k = 1; k <= 1 + rand() % 3; k++) {
				DirectX::XMMATRIX mRotateChild = XMMatrixRotationX(rand() + angle) * XMMatrixRotationY(rand() + angle) * XMMatrixRotationY(rand() + angle);
				m_VSConstantBuffer.world = XMMatrixTranspose(mScaleChild * mTranslateChild * mScale * mRotateChild * mTranslate);
				m_VSConstantBuffer.worldInvTranspose = XMMatrixInverse(nullptr, XMMatrixTranspose(m_VSConstantBuffer.world));
				DrawName(isReflection);
			}
		}
	}
	m_VSConstantBuffer.world = XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0, 0.0f, 0.0f));
	m_VSConstantBuffer.worldInvTranspose = XMMatrixInverse(nullptr, XMMatrixTranspose(m_VSConstantBuffer.world));
}
void GameApp::DrawName(bool isReflection) 
{	
	if(isReflection){
		// 开启反射绘制
		m_VSConstantBuffer.isReflection = true;
		// 绘制不透明物体，需要顺时针裁剪
		// 仅对模板值为1的镜面区域绘制
		m_pd3dImmediateContext->RSSetState(RenderStates::RSCullClockWise.Get());
		m_pd3dImmediateContext->OMSetDepthStencilState(RenderStates::DSSDrawWithStencil.Get(), 1);
		m_pd3dImmediateContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	}
	else{
		//不透明绘制
		m_VSConstantBuffer.isReflection = false;
		m_pd3dImmediateContext->RSSetState(nullptr);
		m_pd3dImmediateContext->OMSetDepthStencilState(nullptr, 0);
		m_pd3dImmediateContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	}
	

	m_pName.SetWorldMatrix(m_VSConstantBuffer.world);
	m_pName.Draw(m_pd3dImmediateContext, m_VSConstantBuffer,m_PSConstantBuffer);
}

void GameApp::DrawPic(bool isReflection) 
{
	if(isReflection)
	{
		m_VSConstantBuffer.isReflection = true;
		m_pd3dImmediateContext->RSSetState(RenderStates::RSNoCull.Get());
		m_pd3dImmediateContext->OMSetDepthStencilState(RenderStates::DSSDrawWithStencil.Get(), 1);
		m_pd3dImmediateContext->OMSetBlendState(RenderStates::BSTransparent.Get(), nullptr, 0xFFFFFFFF);

		m_pMirror.SetWorldMatrix(m_VSConstantBuffer.world);
		m_pMirror.Draw(m_pd3dImmediateContext, m_VSConstantBuffer,m_PSConstantBuffer);
		
	}
	else{
		m_VSConstantBuffer.isReflection = false;
		m_pd3dImmediateContext->RSSetState(RenderStates::RSNoCull.Get());
		m_pd3dImmediateContext->OMSetDepthStencilState(nullptr, 0);
		m_pd3dImmediateContext->OMSetBlendState(RenderStates::BSTransparent.Get(), nullptr, 0xFFFFFFFF);
	}
	

	m_pFace.SetWorldMatrix(m_VSConstantBuffer.world);
	m_pFace.Draw(m_pd3dImmediateContext,m_VSConstantBuffer,m_PSConstantBuffer);
}
void GameApp::DrawMirror() 
{
	// 裁剪掉背面三角形
	// 标记镜面区域的模板值为1
	// 不写入像素颜色
	m_pd3dImmediateContext->RSSetState(nullptr);
	m_pd3dImmediateContext->OMSetDepthStencilState(RenderStates::DSSWriteStencil.Get(), 1);
	m_pd3dImmediateContext->OMSetBlendState(RenderStates::BSNoColorWrite.Get(), nullptr, 0xFFFFFFFF);

	m_pMirror.SetWorldMatrix(m_VSConstantBuffer.world);
	m_pMirror.Draw(m_pd3dImmediateContext, m_VSConstantBuffer,m_PSConstantBuffer);
}
void GameApp::DrawWall()
{
	m_VSConstantBuffer.isReflection = false;
	m_pd3dImmediateContext->RSSetState(nullptr);
	m_pd3dImmediateContext->OMSetDepthStencilState(nullptr, 0);
	m_pd3dImmediateContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

	m_pWall.SetWorldMatrix(m_VSConstantBuffer.world);
	m_pWall.Draw(m_pd3dImmediateContext, m_VSConstantBuffer,m_PSConstantBuffer);
}


bool GameApp::InitEffect() {
	ComPtr<ID3DBlob> blob;

	// 创建顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\Light_VS.cso", L"HLSL\\Light_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	// 创建并绑定顶点布局
	HR(m_pd3dDevice->CreateInputLayout(VertexPosNormalColor::inputLayout, ARRAYSIZE(VertexPosNormalColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));
	//创建几何着色器
	HR(CreateShaderFromFile(L"HLSL\\make_GS.cso", L"HLSL\\make_GS.hlsl", "GS", "gs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pGeometryShader.GetAddressOf()));
	// 创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\Light_PS.cso", L"HLSL\\Light_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));



	//tex
	// 创建顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\Tex_VS.cso", L"HLSL\\Tex_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader_Tex.GetAddressOf()));
	// 创建并绑定顶点布局
	HR(m_pd3dDevice->CreateInputLayout(VertexPosNormalTex::inputLayout, ARRAYSIZE(VertexPosNormalTex::inputLayout), blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout_Tex.GetAddressOf()));
	//创建几何着色器
	HR(CreateShaderFromFile(L"HLSL\\Tex_GS.cso", L"HLSL\\Tex_GS.hlsl", "GS", "gs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pGeometryShader_Tex.GetAddressOf()));
	// 创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\Tex_PS.cso", L"HLSL\\Tex_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader_Tex.GetAddressOf()));

	//mirror
	//创建几何着色器
	HR(CreateShaderFromFile(L"HLSL\\Mirror_GS.cso", L"HLSL\\Mirror_GS.hlsl", "GS", "gs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pGeometryShader_Mirror.GetAddressOf()));

	return true;
}

bool GameApp::InitResource() {
	// 创建对象，绘制类型为0、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、
	name = new NameVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ******************
	// 初始化游戏对象
	//
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material{};
	material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	//初始化名字
	m_pName.isTex = false;
	m_pName.SetMaterial(material);
	m_pName.SetWorldMatrix(XMMatrixIdentity());
	m_pName.SetShaderLayout(m_pVertexShader, m_pGeometryShader, m_pPixelShader, m_pVertexLayout);
	m_pName.SetBuffer(m_pd3dDevice.Get(), name->GetNameVertices(), (UINT)name->GetVerticesCount(), name->GetNameIndices(), (UINT)name->GetIndexCount());

	

	//初始化墙面
	m_pWall.isTex = true;
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"assests\\wall.dds", nullptr, texture.GetAddressOf()));
	m_pWall.SetTexture(texture.Get());
	m_pWall.SetMaterial(material);
	m_pWall.SetWorldMatrix(XMMatrixIdentity());
	m_pWall.SetShaderLayout(m_pVertexShader_Tex, m_pGeometryShader_Mirror, m_pPixelShader_Tex, m_pVertexLayout_Tex);
	m_pWall.SetBuffer(m_pd3dDevice.Get(), name->GetWallVertices(), (UINT)4, name->GetWallIndices(), (UINT)6);



	
	material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	material.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	//初始化合照
	m_pFace.isTex = true;
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"assests\\4face.dds", nullptr, texture.GetAddressOf()));
	m_pFace.SetTexture(texture.Get());
	m_pFace.SetMaterial(material);
	m_pFace.SetWorldMatrix(XMMatrixIdentity());
	m_pFace.SetShaderLayout(m_pVertexShader_Tex, m_pGeometryShader_Tex, m_pPixelShader_Tex, m_pVertexLayout_Tex);
	m_pFace.SetBuffer(m_pd3dDevice.Get(), name->GetNameTexVertices(), (UINT)4, name->GetTexNameIndices(),(UINT) 6);
	//初始化镜面
	m_pMirror.isTex = true;
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"assests\\ice.dds", nullptr, texture.GetAddressOf()));
	m_pMirror.SetTexture(texture.Get());
	m_pMirror.SetMaterial(material);
	m_pMirror.SetWorldMatrix(XMMatrixIdentity());
	m_pMirror.SetShaderLayout(m_pVertexShader_Tex, m_pGeometryShader_Mirror, m_pPixelShader_Tex, m_pVertexLayout_Tex);
	m_pMirror.SetBuffer(m_pd3dDevice.Get(), name->GetMirrorVertices(), (UINT)4, name->GetMirrorIndices(), (UINT)6);



	// ******************
	// 设置常量缓冲区描述
	//
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(VSConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 新建用于VS和PS的常量缓冲区
	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[0].GetAddressOf()));
	cbd.ByteWidth = sizeof(PSConstantBuffer);
	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[1].GetAddressOf()));

	m_PSConstantBuffer.reflect = XMMatrixTranspose(XMMatrixReflect(XMVectorSet(0.0f, 0.0f, -1.0f, 100.0f)));

	// ******************
	// 初始化默认光照
	// 方向光
	m_DirLight.lightIntensity = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_DirLight.direction = XMFLOAT3(-0.577f, -0.577f, 0.577f);
	//五个方向光
	m_DirLight1.lightIntensity = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_DirLight1.direction = XMFLOAT3(-0.5f, -0.5f, -0.5f);
	m_DirLight2.lightIntensity = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_DirLight2.direction = XMFLOAT3(0.5f, 0.5f, 0.5f);
	m_DirLight3.lightIntensity = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	m_DirLight3.direction = XMFLOAT3(0.7f, 0.7f, 0.7f);
	m_DirLight4.lightIntensity = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight4.direction = XMFLOAT3(-0.7f, -0.7f, -0.7f);
	m_DirLight5.lightIntensity = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_DirLight5.direction = XMFLOAT3(0.7f, -0.7f, -0.7f);
	// 点光
	m_PointLight.position = XMFLOAT3(0.0f, 0.0f, -10.0f);
	m_PointLight.lightIntensity = XMFLOAT4(0.4f, 0.8f, 0.4f, 1.0f);
	m_PointLight.att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	m_PointLight.range = 25.0f;
	// 聚光灯
	m_SpotLight.position = XMFLOAT3(0.0f, 0.0f, -5.0f);
	m_SpotLight.direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_SpotLight.lightIntensity = XMFLOAT4(0.4f, 0.4f, 0.8f, 1.0f);
	m_SpotLight.att = XMFLOAT3(0.0f, 0.01f, 0.0f);
	m_SpotLight.spot = 12.0f;
	m_SpotLight.range = 10000.0f;
	// 初始化用于VS的常量缓冲区的值
	m_VSConstantBuffer.world = XMMatrixIdentity();
	m_VSConstantBuffer.view = XMMatrixTranspose(XMMatrixTranslation(-viewPos.x, -viewPos.y, -viewPos.z) * XMMatrixRotationY(-viewRot.y) * XMMatrixRotationZ(-viewRot.z) * XMMatrixRotationX(-viewRot.x));
	m_VSConstantBuffer.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio(), 1.0f, 1000.0f));
	m_VSConstantBuffer.worldInvTranspose = XMMatrixIdentity();
	m_VSConstantBuffer.isReflection = 0;
	// 初始化用于PS的常量缓冲区的值
	m_PSConstantBuffer.material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_PSConstantBuffer.material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_PSConstantBuffer.material.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 5.0f);
	// 使用默认平行光
	m_PSConstantBuffer.dirLight = m_DirLight;
	m_PSConstantBuffer.dirlight1 = DirectionalLight();
	m_PSConstantBuffer.dirlight2 = DirectionalLight();
	m_PSConstantBuffer.dirlight3 = DirectionalLight();
	m_PSConstantBuffer.dirlight4 = DirectionalLight();
	m_PSConstantBuffer.dirlight5 = DirectionalLight();
	// 注意不要忘记设置此处的观察位置，否则高亮部分会有问题
	m_PSConstantBuffer.eyePos = { viewPos.x, viewPos.y, viewPos.z, 0.0f };

	// 更新PS常量缓冲区资源
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[1].Get(), 0);

	// ******************
	// 初始化光栅化状态
	//
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthClipEnable = true;
	HR(m_pd3dDevice->CreateRasterizerState(&rasterizerDesc, m_pRSWireframe.GetAddressOf()));


	RenderStates::InitAll(m_pd3dDevice.Get());

	// ******************
	// 给渲染管线各个阶段绑定好所需资源
	//

	// 设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(name->GetTopology());// 通过对象设置图元类型、、、、、、、、、、、、、、、、、、、、、
	// VS常量缓冲区对应HLSL寄存于b0的常量缓冲区
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffers[0].GetAddressOf());
	m_pd3dImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBuffers[1].GetAddressOf());
	// GS常量缓冲区对应HLSL寄存于b0的常量缓冲区
	// m_pd3dImmediateContext->GSSetShader(m_pGeometryShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->GSSetConstantBuffers(0, 1, m_pConstantBuffers[0].GetAddressOf());
	m_pd3dImmediateContext->GSSetConstantBuffers(1, 1, m_pConstantBuffers[1].GetAddressOf());
	// PS常量缓冲区对应HLSL寄存于b1的常量缓冲区
	m_pd3dImmediateContext->PSSetConstantBuffers(0, 1, m_pConstantBuffers[0].GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffers[1].GetAddressOf());
	// PS采样器对应位图资源寄存于s0的采样器
	m_pd3dImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());

	return true;
}


// GameObject ***************************
// 游戏对象类实现
GameApp::GameObject::GameObject() :
	m_IndexCount(),
	m_Material(),
	m_VertexStride(),
	m_WorldMatrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f) {
}

DirectX::XMFLOAT3 GameApp::GameObject::GetPosition() const {
	return XMFLOAT3(m_WorldMatrix(3, 0), m_WorldMatrix(3, 1), m_WorldMatrix(3, 2));
}

//传入顶点和索引
template<class VertexType, class IndexType>
void GameApp::GameObject::SetBuffer(ID3D11Device* device, VertexType* vertices, UINT VertexCount, IndexType* indices, UINT IndexCount) {
	// 释放旧资源
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// 设置顶点缓冲区描述
	m_VertexStride = sizeof(VertexType);
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = m_VertexStride * VertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

	// 设置索引缓冲区描述
	m_IndexCount = IndexCount;
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = m_IndexCount * sizeof(IndexType);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
}

void GameApp::GameObject::SetTexture(ID3D11ShaderResourceView* texture) {
	m_pTexture = texture;
}

void GameApp::GameObject::SetMaterial(const Material& material) {
	m_Material = material;
}

void GameApp::GameObject::SetWorldMatrix(const XMFLOAT4X4& world) {
	m_WorldMatrix = world;
}

void XM_CALLCONV GameApp::GameObject::SetWorldMatrix(FXMMATRIX world) {
	XMStoreFloat4x4(&m_WorldMatrix, world);
}
void GameApp::GameObject::SetShaderLayout(const ComPtr<ID3D11VertexShader> VS, const ComPtr<ID3D11GeometryShader> GS, const ComPtr<ID3D11PixelShader> PS, const ComPtr<ID3D11InputLayout> IL) {
	m_VS = VS;
	m_GS = GS;
	m_PS = PS;
	m_IL = IL;
}
void GameApp::GameObject::Draw(ComPtr<ID3D11DeviceContext> deviceContext,VSConstantBuffer& m_VSConstantBuffer,PSConstantBuffer& m_PSConstantBuffer) {
	// 设置顶点/索引缓冲区
	UINT strides = m_VertexStride;
	UINT offsets = 0;
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	// 获取之前已经绑定到渲染管线上的常量缓冲区并进行修改
	ComPtr<ID3D11Buffer> cBuffer = nullptr;
	deviceContext->VSGetConstantBuffers(0, 1, cBuffer.GetAddressOf());
	// 更新常量缓冲区
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(deviceContext->Map(cBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(VSConstantBuffer), &m_VSConstantBuffer, sizeof(VSConstantBuffer));
	deviceContext->Unmap(cBuffer.Get(), 0);

	deviceContext->PSGetConstantBuffers(1, 1, cBuffer.GetAddressOf());
	m_PSConstantBuffer.material = m_Material;
	HR(deviceContext->Map(cBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
	deviceContext->Unmap(cBuffer.Get(), 0);



	//更改着色器 顶点布局
	deviceContext->IASetInputLayout(m_IL.Get());
	deviceContext->VSSetShader(m_VS.Get(), nullptr, 0);
	deviceContext->GSSetShader(m_GS.Get(), nullptr, 0);
	deviceContext->PSSetShader(m_PS.Get(), nullptr, 0);
	// 设置纹理 判断
	if (isTex) {
		deviceContext->PSSetShaderResources(0, 1, m_pTexture.GetAddressOf());
	}
	// 可以开始绘制
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

void GameApp::GameObject::SetDebugObjectName(const std::string& name) {
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	std::string vbName = name + ".VertexBuffer";
	std::string ibName = name + ".IndexBuffer";
	m_pVertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(vbName.length()), vbName.c_str());
	m_pIndexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(ibName.length()), ibName.c_str());
#else
	UNREFERENCED_PARAMETER(name);
#endif
}
