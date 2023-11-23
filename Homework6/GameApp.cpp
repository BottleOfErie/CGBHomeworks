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
	m_VSConstantBuffer.view = XMMatrixTranspose(XMMatrixTranslation(-viewPos.x, -viewPos.y, -viewPos.z) * XMMatrixRotationY(-viewRot.y) * XMMatrixRotationZ(-viewRot.z) * XMMatrixRotationX(-viewRot.x));
	m_PSConstantBuffer.eyePos = { viewPos.x, viewPos.y, viewPos.z, 0.0f };

	if (m_KeyboardTracker.IsKeyPressed(Keyboard::D3))
	{
		m_PSConstantBuffer.dirLight = m_DirLight;
		m_PSConstantBuffer.pointLight = PointLight();
		m_PSConstantBuffer.spotLight = SpotLight();
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::D1))
	{
		m_PSConstantBuffer.dirLight = DirectionalLight();
		m_PSConstantBuffer.pointLight = m_PointLight;
		m_PSConstantBuffer.spotLight = SpotLight();
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::D2))
	{
		m_PSConstantBuffer.dirLight = DirectionalLight();
		m_PSConstantBuffer.pointLight = PointLight();
		m_PSConstantBuffer.spotLight = m_SpotLight;
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::OemPlus))
	{
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
		if(dirlightcount>5) dirlightcount = 5;
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
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::F))
	{
		m_IsWireframeMode = !m_IsWireframeMode;
		m_pd3dImmediateContext->RSSetState(m_IsWireframeMode ? m_pRSWireframe.Get() : nullptr);
	}

	m_PSConstantBuffer.pointLight.position.x = sinf(angle * 0.2f) * 20.0f;
	m_PSConstantBuffer.pointLight.position.z = sinf(angle * 0.5f) * 20.0f;

	m_PSConstantBuffer.spotLight.position = viewPos;
	XMFLOAT3 dir = { 0, 0, 1.0f };
	XMStoreFloat3(&m_PSConstantBuffer.spotLight.direction, XMVector3Transform(XMLoadFloat3(&dir), XMMatrixRotationX(viewRot.x) * XMMatrixRotationZ(viewRot.z) * XMMatrixRotationY(viewRot.y)));

}

void GameApp::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_PSConstantBuffer.material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_PSConstantBuffer.material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_PSConstantBuffer.material.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 100.0f);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[1].Get(), 0);

	m_VSConstantBuffer.world = XMMatrixTranspose(XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixTranslation(0, 0.0f, 0.0f));
	m_VSConstantBuffer.worldInvTranspose = XMMatrixInverse(nullptr, XMMatrixTranspose(m_VSConstantBuffer.world));
	
	// DrawName();

	// m_PSConstantBuffer.material.ambient = XMFLOAT4(0.5f, 0.1f, 0.1f, 1.0f);
	// m_PSConstantBuffer.material.diffuse = XMFLOAT4(1.0f, 0.2f, 0.1f, 1.0f);
	// m_PSConstantBuffer.material.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 100.0f);
	// HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	// memcpy_s(mappedData.pData, sizeof(PSConstantBuffer), &m_PSConstantBuffer, sizeof(PSConstantBuffer));
	// m_pd3dImmediateContext->Unmap(m_pConstantBuffers[1].Get(), 0);

	DrawName();
	DrawMirror();
	for (int i = 1; i <= nameN; i++)
	{
		for (int j = 1; j <= nameN; j++)
		{
			float scale = (sinf(angle * 3.0f + i * j * 0.2f) + 1.3f) * 0.35f;
			DirectX::XMMATRIX mScale = XMMatrixScaling(scale, scale, scale);
			DirectX::XMMATRIX mRotate = XMMatrixRotationX(angle) * XMMatrixRotationY(angle);
			DirectX::XMMATRIX mTranslate = XMMatrixTranslation((i - (nameN + 1) / 2.0f) * 4.5f*4, 0, (j - (nameN + 1) / 2.0f) * 4.5f*4);
			m_VSConstantBuffer.world = XMMatrixTranspose(mScale * mRotate * mTranslate); // mul(vec, mat) 中为行向量，矩阵右乘，顺序SRT, 参考https://www.cnblogs.com/X-Jun/p/9808727.html#_lab2_1_1
			m_VSConstantBuffer.worldInvTranspose = XMMatrixInverse(nullptr, XMMatrixTranspose(m_VSConstantBuffer.world));

			DrawName();

			srand(i * j + 1);
			DirectX::XMMATRIX mScaleChild = XMMatrixScaling(0.3f, 0.3f, 0.3f);
			DirectX::XMMATRIX mTranslateChild = XMMatrixTranslation(2.5f, 0, 0);
			for (int k = 1; k <= 1 + rand() % 3; k++)
			{
				DirectX::XMMATRIX mRotateChild = XMMatrixRotationX(rand() + angle) * XMMatrixRotationY(rand() + angle) * XMMatrixRotationY(rand() + angle);
				m_VSConstantBuffer.world = XMMatrixTranspose(mScaleChild * mTranslateChild * mScale * mRotateChild * mTranslate);
				m_VSConstantBuffer.worldInvTranspose = XMMatrixInverse(nullptr, XMMatrixTranspose(m_VSConstantBuffer.world));

				DrawName();
			}
		}
	}

	HR(m_pSwapChain->Present(0, 0));
}

void GameApp::DrawName()
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(VSConstantBuffer), &m_VSConstantBuffer, sizeof(VSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[0].Get(), 0);
	m_pd3dImmediateContext->DrawIndexed(name->GetIndexCount()-6, 0, 0);// 通过对象获取索引个数、、、、、、、、、、、、、、、、、
}
void GameApp::DrawMirror()
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(VSConstantBuffer), &m_VSConstantBuffer, sizeof(VSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[0].Get(), 0);
	m_pd3dImmediateContext->DrawIndexed(6, 24156, 0);// 通过对象获取索引个数、、、、、、、、、、、、、、、、、
}

bool GameApp::InitEffect()
{
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

	return true;
}

bool GameApp::InitResource()
{
	// 创建对象，绘制类型为0、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、
	name = new NameVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 设置三角形顶点
	VertexPosNormalColor* vertices = name->GetNameVertices(); // 通过对象获取顶点、、、、、、、、、、、、、、、、、、、、、
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
	cbd.ByteWidth = sizeof(VSConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 新建用于VS和PS的常量缓冲区
	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[0].GetAddressOf()));
	cbd.ByteWidth = sizeof(PSConstantBuffer);
	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[1].GetAddressOf()));

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
	memcpy_s(mappedData.pData, sizeof(PSConstantBuffer), &m_VSConstantBuffer, sizeof(PSConstantBuffer));
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

	// ******************
	// 给渲染管线各个阶段绑定好所需资源
	//

	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexPosNormalColor);	// 跨越字节数
	UINT offset = 0;						// 起始偏移量

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	// 设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(name->GetTopology());// 通过对象设置图元类型、、、、、、、、、、、、、、、、、、、、、
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	// 将着色器绑定到渲染管线
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	// VS常量缓冲区对应HLSL寄存于b0的常量缓冲区
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffers[0].GetAddressOf());
	// GS常量缓冲区对应HLSL寄存于b0的常量缓冲区
	m_pd3dImmediateContext->GSSetShader(m_pGeometryShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->GSSetConstantBuffers(0, 1, m_pConstantBuffers[0].GetAddressOf());
	// PS常量缓冲区对应HLSL寄存于b1的常量缓冲区
	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffers[1].GetAddressOf());
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

	// ******************
	// 设置调试对象名
	//
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");
	D3D11SetDebugObjectName(m_pVertexLayout.Get(), "VertexPosNormalTexLayout");
	D3D11SetDebugObjectName(m_pConstantBuffers[0].Get(), "VSConstantBuffer");
	D3D11SetDebugObjectName(m_pConstantBuffers[1].Get(), "PSConstantBuffer");
	D3D11SetDebugObjectName(m_pVertexShader.Get(), "Light_VS");
	D3D11SetDebugObjectName(m_pPixelShader.Get(), "Light_PS");

	return true;
}
