#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC GameApp::VertexPosColor::inputLayout[2] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance), m_CBuffer()
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();
}

void GameApp::UpdateScene(float dt)
{
	
	static float phi = 0.0f, theta = 0.0f;
	phi += 0.0001f, theta += 0.00015f;
	m_CBuffer.world = XMMatrixTranspose(XMMatrixRotationX(phi) * XMMatrixRotationY(theta));
	// 更新常量缓冲区，让立方体转起来
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(m_CBuffer), &m_CBuffer, sizeof(m_CBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer.Get(), 0);
}

void GameApp::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&black));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 绘制立方体
	m_pd3dImmediateContext->DrawIndexed(708, 0, 0);
	HR(m_pSwapChain->Present(0, 0));
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
	// ******************
	// 设置立方体顶点
	//    5________ 6
	//    /|      /|
	//   /_|_____/ |
	//  1|4|_ _ 2|_|7
	//   | /     | /
	//   |/______|/
	//  0       3
	VertexPosColor vertices[] =
	{
		{ XMFLOAT3(-1.273100,0.588442,0.787134), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(-1.184780,0.757046,0.787135), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(-0.795693,0.553229,0.787134), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(-0.884014,0.384624,0.787134), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(-1.351894,0.061960,0.787134), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(-1.263572,0.230564,0.787134), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(-0.874486,0.026747,0.787134), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(-0.962808,-0.141858,0.787134), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(-1.304708,-0.763794,0.787134), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(-1.403939,-0.601370,0.787134), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(-1.029118,-0.372377,0.787134), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(-0.929887,-0.534800,0.787134), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(-0.327478,0.715989,0.787145), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(-0.514313,0.752329,0.787124), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(-0.482049,0.881870,0.787124), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(-0.295213,0.881870,0.787145), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(-0.669878,-1.044393,0.787145), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(-0.856713,-1.008053,0.787123), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(0.812069,0.881870,0.787145), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(0.779806,0.715989,0.787145), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(-0.029549,0.469846,0.787134), XMFLOAT4(1, 0, 1, 1.0f) },
{ XMFLOAT3(0.094831,0.325771,0.787134), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(-0.025312,0.222050,0.787134), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(-0.149693,0.366124,0.787134), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(0.182786,0.653156,0.787134), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(0.307167,0.509081,0.787134), XMFLOAT4(1, 0, 1, 1.0f) },
{ XMFLOAT3(0.418277,0.325771,0.787134), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(0.418277,0.222050,0.787134), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(-0.025312,-0.035561,0.787134), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(-0.025312,-0.139222,0.787134), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(-0.149693,-0.139222,0.787134), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(-0.149693,0.005778,0.787134), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(0.548853,0.325771,0.787134), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(0.548853,0.222050,0.787134), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(0.548853,-0.035561,0.787134), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(0.548853,-0.139222,0.787134), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(0.418277,-0.139222,0.787134), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(0.418277,-0.035561,0.787134), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(-0.025312,-0.244926,0.787134), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(-0.149693,-0.244926,0.787134), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(0.548853,-0.244926,0.787134), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(0.418277,-0.244926,0.787134), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(0.275889,-0.244926,0.787134), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(0.275889,-0.139222,0.787134), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(-0.025312,0.068098,0.787134), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(-0.149693,0.150778,0.787134), XMFLOAT4(1, 0, 1, 1.0f) },
{ XMFLOAT3(0.548853,0.068100,0.787134), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(0.418277,0.068100,0.787134), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(0.124637,-0.139222,0.787134), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(0.124637,-0.244926,0.787134), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(0.275889,-0.871734,0.787134), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(0.275889,-1.044393,0.787134), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(0.124637,-1.044392,0.787134), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(0.124637,-0.871734,0.787134), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(-0.121005,-0.842701,0.787134), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(-0.046354,-0.720778,0.787134), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(-0.409331,-0.785283,0.787134), XMFLOAT4(1, 0, 1, 1.0f) },
{ XMFLOAT3(-0.508562,-0.622860,0.787134), XMFLOAT4(1, 0, 1, 1.0f) },
{ XMFLOAT3(-0.133740,-0.393865,0.787134), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(-0.034508,-0.556288,0.787134), XMFLOAT4(1, 0, 1, 1.0f) },
{ XMFLOAT3(0.420153,-0.563533,0.787134), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(0.524201,-0.404153,0.787134), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(0.892000,-0.644267,0.787134), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(0.787950,-0.803647,0.787134), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(-1.273100,0.588442,0.526353), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(-0.884014,0.384624,0.526353), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(-0.795693,0.553229,0.526353), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(-1.184778,0.757046,0.526353), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(-1.351893,0.061960,0.526353), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(-0.962808,-0.141858,0.526353), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(-0.874486,0.026747,0.526353), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(-1.263572,0.230564,0.526353), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(-1.304708,-0.763793,0.526353), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(-0.929885,-0.534800,0.526353), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(-1.029116,-0.372377,0.526353), XMFLOAT4(1, 0, 1, 1.0f) },
{ XMFLOAT3(-1.403939,-0.601370,0.526353), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(-0.327478,0.715989,0.526364), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(-0.295213,0.881870,0.526364), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(-0.482049,0.881868,0.526342), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(-0.514313,0.752329,0.526342), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(-0.669876,-1.044393,0.526363), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(-0.856712,-1.008053,0.526341), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(0.779806,0.715989,0.526364), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(0.812069,0.881870,0.526364), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(-0.029549,0.469846,0.526353), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(-0.149693,0.366124,0.526353), XMFLOAT4(1, 0, 1, 1.0f) },
{ XMFLOAT3(-0.025312,0.222050,0.526353), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(0.094831,0.325772,0.526353), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(0.182786,0.653156,0.526353), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(0.307167,0.509081,0.526353), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(0.418277,0.222050,0.526353), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(0.418278,0.325772,0.526353), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(-0.025312,-0.035561,0.526353), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(-0.149693,0.005778,0.526353), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(-0.149693,-0.139222,0.526353), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(-0.025312,-0.139222,0.526353), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(0.548853,0.222050,0.526353), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(0.548853,0.325772,0.526353), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(0.548853,-0.035561,0.526353), XMFLOAT4(0, 1, 0, 1.0f) },
{ XMFLOAT3(0.418277,-0.035561,0.526353), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(0.418277,-0.139222,0.526353), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(0.548853,-0.139222,0.526353), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(-0.149693,-0.244926,0.526353), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(-0.025312,-0.244926,0.526353), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(0.418278,-0.244926,0.526353), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(0.548853,-0.244926,0.526353), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(0.275889,-0.244926,0.526353), XMFLOAT4(1, 0, 1, 1.0f) },
{ XMFLOAT3(0.275889,-0.139222,0.526353), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(-0.149693,0.150778,0.526353), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(-0.025312,0.068098,0.526353), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(0.418277,0.068100,0.526353), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(0.548853,0.068100,0.526353), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(0.124638,-0.244926,0.526353), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(0.124638,-0.139222,0.526353), XMFLOAT4(1, 0, 0, 1.0f) },
{ XMFLOAT3(0.275889,-0.871734,0.526352), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(0.124638,-0.871734,0.526352), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(0.124638,-1.044392,0.526352), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(0.275889,-1.044393,0.526352), XMFLOAT4(0, 0, 1, 1.0f) },
{ XMFLOAT3(-0.046354,-0.720778,0.526353), XMFLOAT4(1, 1, 0, 1.0f) },
{ XMFLOAT3(-0.121005,-0.842701,0.526352), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(-0.409330,-0.785283,0.526352), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(-0.034508,-0.556289,0.526353), XMFLOAT4(1, 0, 1, 1.0f) },
{ XMFLOAT3(-0.133740,-0.393865,0.526353), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(-0.508561,-0.622860,0.526353), XMFLOAT4(0, 1, 1, 1.0f) },
{ XMFLOAT3(0.420153,-0.563533,0.526353), XMFLOAT4(1, 1, 1, 1.0f) },
{ XMFLOAT3(0.787951,-0.803647,0.526352), XMFLOAT4(1, 0, 1, 1.0f) },
{ XMFLOAT3(0.892000,-0.644267,0.526353), XMFLOAT4(0, 0, 0, 1.0f) },
{ XMFLOAT3(0.524203,-0.404153,0.526353), XMFLOAT4(0, 1, 1, 1.0f) },
	};
	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof vertices;
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
	WORD indices[] = {
		3,2,0,
4,7,5,
11,10,8,
15,14,12,
16,12,17,
12,19,15,
20,23,21,
20,21,24,
22,27,21,
31,30,28,
26,27,32,
34,37,35,
29,30,38,
35,36,40,
42,41,43,
23,45,22,
45,31,44,
33,27,46,
46,47,34,
28,37,44,
38,49,29,
49,42,48,
53,52,50,
42,49,50,
52,53,54,
56,59,57,
63,62,60,
64,67,65,
71,70,68,
75,74,72,
79,78,76,
80,81,76,
83,82,77,
84,87,85,
89,87,88,
91,90,87,
95,94,92,
97,96,91,
98,101,99,
95,103,94,
105,104,101,
107,100,106,
109,108,86,
92,93,109,
96,111,90,
111,98,110,
92,109,99,
103,95,112,
112,113,106,
117,116,114,
106,114,112,
116,119,115,
120,123,121,
127,126,124,
57,123,56,
45,108,31,
24,88,20,
67,64,1,
56,120,59,
29,95,28,
0,64,3,
121,122,59,
3,65,2,
122,123,58,
34,98,46,
66,67,2,
127,124,61,
27,90,47,
91,87,26,
71,68,5,
60,124,63,
22,86,27,
4,68,7,
63,125,62,
7,69,6,
126,127,62,
35,101,34,
44,109,22,
6,70,5,
31,93,30,
9,75,8,
28,92,37,
33,96,32,
8,72,11,
47,110,44,
97,91,32,
73,74,11,
38,103,49,
74,75,10,
113,95,48,
81,80,17,
46,111,33,
37,99,36,
15,77,14,
107,113,43,
39,102,38,
79,81,13,
16,80,12,
52,116,51,
30,94,39,
14,78,13,
50,114,42,
41,104,40,
40,105,35,
19,82,18,
51,117,50,
18,83,15,
49,112,53,
42,106,41,
12,76,19,
36,100,43,
25,89,24,
55,118,54,
20,84,23,
54,119,52,
23,85,45,
115,118,53,
21,87,25,
2,1,0,
7,6,5,
10,9,8,
14,13,12,
12,13,17,
19,18,15,
23,22,21,
21,25,24,
27,26,21,
30,29,28,
27,33,32,
37,36,35,
30,39,38,
36,41,40,
41,36,43,
45,44,22,
31,28,44,
27,47,46,
47,37,34,
37,47,44,
49,48,29,
42,43,48,
52,51,50,
49,53,50,
53,55,54,
59,58,57,
62,61,60,
67,66,65,
70,69,68,
74,73,72,
78,77,76,
81,79,76,
82,76,77,
87,86,85,
87,84,88,
90,86,87,
94,93,92,
96,90,91,
101,100,99,
103,102,94,
104,100,101,
100,104,106,
108,85,86,
93,108,109,
111,110,90,
98,99,110,
109,110,99,
95,113,112,
113,107,106,
116,115,114,
114,115,112,
119,118,115,
123,122,121,
126,125,124,
123,120,56,
108,93,31,
88,84,20,
64,0,1,
120,121,59,
95,92,28,
64,65,3,
122,58,59,
65,66,2,
123,57,58,
98,111,46,
67,1,2,
124,60,61,
90,110,47,
87,21,26,
68,4,5,
124,125,63,
86,90,27,
68,69,7,
125,126,62,
69,70,6,
127,61,62,
101,98,34,
109,86,22,
70,71,5,
93,94,30,
75,72,8,
92,99,37,
96,97,32,
72,73,11,
110,109,44,
91,26,32,
74,10,11,
103,112,49,
75,9,10,
95,29,48,
80,16,17,
111,96,33,
99,100,36,
77,78,14,
113,48,43,
102,103,38,
81,17,13,
80,76,12,
116,117,51,
94,102,39,
78,79,13,
114,106,42,
104,105,40,
105,101,35,
82,83,18,
117,114,50,
83,77,15,
112,115,53,
106,104,41,
76,82,19,
100,107,43,
89,88,24,
118,119,54,
84,85,23,
119,116,52,
85,108,45,
118,55,53,
87,89,25,
	};
	// 设置索引缓冲区描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof indices;
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
	m_CBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH(
		XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	));
	m_CBuffer.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio(), 1.0f, 1000.0f));


	// ******************
	// 给渲染管线各个阶段绑定好所需资源
	//

	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexPosColor);	// 跨越字节数
	UINT offset = 0;						// 起始偏移量

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	// 设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
