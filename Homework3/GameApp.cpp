#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include "NameVertices.h"
using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC GameApp::VertexPosColor::inputLayout[2] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance), m_CBuffer(),
	name(nullptr),// 初始化变量，置空、、、、、、、、、、、、、、、、、、、、、
	nameN(10),
	angle(0),
	viewPos(0.0f, 60.0f, -100.0f),
	viewRot(0.7f, 0.0f, 0.0f)
{
}

GameApp::~GameApp()
{
	delete name;// 释放内存、、、、、、、、、、、、、、、、、、、、、、、、、、
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	// 初始化鼠标，键盘不需要
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);

	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();
}

void GameApp::UpdateScene(float dt)
{
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
	float moveSpeed = 40.0f;
	if (keyState.IsKeyDown(Keyboard::LeftShift)) moveSpeed *= 2.0f;
	if (keyState.IsKeyDown(Keyboard::W)) pos.z += moveSpeed * dt;
	if (keyState.IsKeyDown(Keyboard::S)) pos.z -= moveSpeed * dt;
	if (keyState.IsKeyDown(Keyboard::A)) pos.x -= moveSpeed * dt;
	if (keyState.IsKeyDown(Keyboard::D)) pos.x += moveSpeed * dt;
	if (keyState.IsKeyDown(Keyboard::Q)) viewRot.z += 0.5f * dt;
	if (keyState.IsKeyDown(Keyboard::E)) viewRot.z -= 0.5f * dt;

	if (mouseState.leftButton == true && m_MouseTracker.leftButton == m_MouseTracker.HELD) // 这两者似乎只有在鼠标按下的那一帧存在区别(此时左为true，右为false)?
	{
		viewRot.y += (mouseState.x - lastMouseState.x) * 0.5f * dt;
		viewRot.x += (mouseState.y - lastMouseState.y) * 0.5f * dt;
	}

	if (viewRot.x > 1.5f) viewRot.x = 1.5f;
	else if (viewRot.x < -1.5f) viewRot.x = -1.5f;
	if (viewRot.z > 0.7f) viewRot.z = 0.7f;
	else if (viewRot.z < -0.7f) viewRot.z = -0.7f;

	XMStoreFloat3(&pos, XMVector3Transform(XMLoadFloat3(&pos),  XMMatrixRotationX(viewRot.x) * XMMatrixRotationZ(viewRot.z) * XMMatrixRotationY(viewRot.y)));
	viewPos.x += pos.x;
	viewPos.y += pos.y;
	viewPos.z += pos.z;
	m_CBuffer.view = XMMatrixTranspose(XMMatrixTranslation(-viewPos.x, -viewPos.y, -viewPos.z) * XMMatrixRotationY(-viewRot.y) * XMMatrixRotationZ(-viewRot.z) * XMMatrixRotationX(-viewRot.x));
}

void GameApp::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&black));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	const float dx = -30.0f, dy = 10.0f ,dz=100.0f;
	DirectX::XMMATRIX mTranslateCamera = XMMatrixTranslation(dx, dy, dz);

	for (int i = 1; i <= nameN; i++)
	{
		for (int j = 1; j <= nameN; j++)
		{
			float scale = (sinf(angle * 3.0f + i * j * 0.2f) + 1.3f) * 0.15f;
			DirectX::XMMATRIX mScale = XMMatrixScaling(scale, scale, scale);
			DirectX::XMMATRIX mRotate = XMMatrixRotationX(angle) * XMMatrixRotationY(angle);
			DirectX::XMMATRIX mTranslate = XMMatrixTranslation((i - 7.5f) * 35.5f, 0, (j - 7.5f) * 35.5f);
			m_CBuffer.world = XMMatrixTranspose(mScale * mRotate * mTranslate);
			DrawName();

			srand(i * j + 1);
			DirectX::XMMATRIX mScaleChild = XMMatrixScaling(0.07f, 0.07f, 0.07f);
			DirectX::XMMATRIX mTranslateChild = XMMatrixTranslation(10.0f, 10.0f, 10.0f);
			for (int k = 1; k <= 5; k++)
			{
				DirectX::XMMATRIX mRotateChild = XMMatrixRotationX(angle * rand() / RAND_MAX * 2.0f) * XMMatrixRotationY(angle * rand() / RAND_MAX * 2.0f);
				m_CBuffer.world = XMMatrixTranspose(mScaleChild * mTranslateChild * mRotateChild   * mTranslate *mTranslateCamera);

				DrawName();
			}
		}
	}

	HR(m_pSwapChain->Present(0, 0));
}

void GameApp::DrawName()
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(m_CBuffer), &m_CBuffer, sizeof(m_CBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer.Get(), 0);
	m_pd3dImmediateContext->DrawIndexed(name->GetIndexCount(), 0, 0);// 通过对象获取索引个数、、、、、、、、、、、、、、、、、
}

bool GameApp::InitEffect()
{
	ComPtr<ID3DBlob> blob;

	// 创建顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\Cube_VS.cso", L"HLSL\\Cube_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	// 创建顶点布局
	HR(m_pd3dDevice->CreateInputLayout(VertexPosColor::inputLayout, ARRAYSIZE(VertexPosColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

	// 创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\Cube_PS.cso", L"HLSL\\Cube_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

	return true;
}

bool GameApp::InitResource()
{
	// 创建对象，绘制类型为0、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、
	name = new NameVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 设置三角形顶点
	VertexPosColor* vertices = name->GetNameVertices(); // 通过对象获取顶点、、、、、、、、、、、、、、、、、、、、、
	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (sizeof * vertices) * name->GetVerticesCount(); // 计算位宽、、、、、、、、、、、、、、、、、、、、、、、、、
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

	// ******************
	// 索引数组
	//
	WORD* indices = name->GetNameIndices(); // 通过对象获取索引、、、、、、、、、、、、、、、、、、、、、
	// 设置索引缓冲区描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = (sizeof * indices) * name->GetIndexCount(); // 计算位宽、、、、、、、、、、、、、、、、、、、、、、、、、
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = indices;
	HR(m_pd3dDevice->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
	// 输入装配阶段的索引缓冲区设置
	m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);


	// ******************
	// 设置常量缓冲区描述
	//
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 新建常量缓冲区，不使用初始数据
	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffer.GetAddressOf()));

	// 初始化常量缓冲区的值
	m_CBuffer.world = XMMatrixIdentity();	// 单位矩阵的转置是它本身
	m_CBuffer.view = XMMatrixTranspose(XMMatrixTranslation(-viewPos.x, -viewPos.y, -viewPos.z) * XMMatrixRotationY(-viewRot.y) * XMMatrixRotationZ(-viewRot.z) * XMMatrixRotationX(-viewRot.x));
	m_CBuffer.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio(), 1.0f, 1000.0f));


	// ******************
	// 给渲染管线各个阶段绑定好所需资源
	//

	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexPosColor);	// 跨越字节数
	UINT offset = 0;						// 起始偏移量

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	// 设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(name->GetTopology());// 通过对象设置图元类型、、、、、、、、、、、、、、、、、、、、、
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	// 将着色器绑定到渲染管线
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	// 将更新好的常量缓冲区绑定到顶点着色器
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());

	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

	// ******************
	// 设置调试对象名
	//
	D3D11SetDebugObjectName(m_pVertexLayout.Get(), "VertexPosColorLayout");
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");
	D3D11SetDebugObjectName(m_pConstantBuffer.Get(), "ConstantBuffer");
	D3D11SetDebugObjectName(m_pVertexShader.Get(), "Cube_VS");
	D3D11SetDebugObjectName(m_pPixelShader.Get(), "Cube_PS");

	return true;
}
