#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dinput.h>

#define DIRECTINPUT_VERSION	0x0800		//DirectInputのバージョン指定

using namespace DirectX;
using namespace std;

#pragma comment(lib,"d3d12.lib") 
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")



//ウィンドウプロシージャ
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	//--- ウインドウの生成処理 -----------------------------------------------------//

// コンソールヘの文字出力
//OutputDebugStringA("Hello,DirectX!!\n");

// ウィンドウの生成（前処理）
#pragma region ウィンドウの生成（前処理）

	// ウィンドウサイズ
	const int window_width = 1280;  // 横幅
	const int window_height = 720;  // 縦幅

	WNDCLASSEX w{}; // ウィンドウクラスの設定
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc; // ウィンドウプロシージャを設定
	w.lpszClassName = L"DirectX12Game"; // ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr); // ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW); // カーソル指定

	// ウィンドウクラスをOSに登録
	RegisterClassEx(&w);
	// ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
	RECT wrc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // 自動でサイズ補正

	// ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(w.lpszClassName, // クラス名
		L"DirectXGame",         // タイトルバーの文字
		WS_OVERLAPPEDWINDOW,        // 標準的なウィンドウスタイル
		CW_USEDEFAULT,              // 表示X座標（OSに任せる）
		CW_USEDEFAULT,              // 表示Y座標（OSに任せる）
		wrc.right - wrc.left,       // ウィンドウ横幅
		wrc.bottom - wrc.top,   // ウィンドウ縦幅
		nullptr,                // 親ウィンドウハンドル
		nullptr,                // メニューハンドル
		w.hInstance,            // 呼び出しアプリケーションハンドル
		nullptr);               // オプション

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

#pragma endregion ウィンドウの生成（前処理）

	//------------------------------------------------------------------------------//

	MSG msg{}; // メッセージ

#pragma region デバッグ
#ifdef _DEBUG
// デバックレイヤーをオンに
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif
#pragma endregion デバッグ

#pragma region DirectX初期化処理

	#pragma region DirectX初期化
	HRESULT result;
	ID3D12Device* device = nullptr;
	IDXGIFactory6* dxgiFactory = nullptr;
	IDXGISwapChain4* swapchain = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12DescriptorHeap* rtvHeaps = nullptr;
#pragma endregion DirectX初期化

	#pragma region グラフィックスボードのアダプタを列挙
	// DXGIファクトリーの生成
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	// アダプターの列挙用
	vector<IDXGIAdapter4*> adapters;
	// ここに特定の名前を持アダプターオブジェクトを入れる
	IDXGIAdapter4* tmpAdapter = nullptr;
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter); //動的配列に追加する
	}

	for (size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc;
		adapters[i]->GetDesc3(&adapterDesc); // アダプターの情報を取得

		// ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			tmpAdapter = adapters[i]; //採用
			break;
		}
	}
#pragma endregion グラフィックスボードのアダプタを列挙

	#pragma region デバイスの生成
	// 対応レベルの配列
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++)
	{
		// 採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&device));
		if (result == S_OK)
		{
			// デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

#pragma endregion デバイスの生成

	#pragma region コマンドリスト
	// コマンドアロケータを生成
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(result));

	// コマンドリストの生成
	result = device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator, nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));
#pragma endregion コマンドリスト

	#pragma region コマンドキュー
	// 標準設定でコマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));
#pragma endregion コマンドキュー

	#pragma region スワップチェーンの生成
	// 各種設定をしてスワップチェーンを生成
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = window_width;
	swapChainDesc.Height = window_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色情報の書式
	swapChainDesc.SampleDesc.Count = 1; // マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファ用
	swapChainDesc.BufferCount = 2; // バッファ数を２つに設定
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //フリップ後は破壊
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	result = dxgiFactory->CreateSwapChainForHwnd( commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)&swapchain);
#pragma endregion スワップチェーンの生成

	#pragma region デスクリプタヒープの生成
	// 各種設定をしてデスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount;	// 裏表の２つ
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeaps));
#pragma endregion デスクリプタヒープの生成

	#pragma region レンダーターゲットビューの生成
	// 裏表の2つ分について
	vector<ID3D12Resource*> backBuffers(2);
	backBuffers.resize(swapChainDesc.BufferCount);

	for (size_t i = 0; i < backBuffers.size(); i++)
	{
		// スワップチェーンからバッファを取得
		swapchain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		// デスクリプタヒープのハンドルの取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		// 裏か表かアドレスがずれる
		rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// レンダーターゲットビューの生成
		device->CreateRenderTargetView(backBuffers[i], &rtvDesc, rtvHandle);
	}
#pragma endregion レンダーターゲットビューの生成

	#pragma region フェンスの生成
	// フェンスの生成
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
#pragma endregion フェンスの生成

	#pragma region DirectInput

	IDirectInput8* directInput = nullptr;
	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	IDirectInputDevice8* keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

#pragma endregion DirectInput

	// DirectX初期化処理 ここまで
#pragma endregion DirectX初期化処理

#pragma region 描画初期化


	//-------------------------
	//頂点
	#pragma region 頂点データー

	XMFLOAT3 vertices[] = {
		{ -0.5f, -0.5f, 0.0f },
		{ -0.5f, +0.5f, 0.0f },
		{ +0.5f, -0.5f, 0.0f },
	};

	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	/*const float radius = 0.5f;
	const float radius2 = 0.25f;
	const int DIV = 10;*/

	//頂点データ
	//Vertex vertices[] = {
	//	//    x       y      z      u    v
	//	//前
	//	{{-5, -5, -5},{0.0f, 1.0f}},	//左下
	//	{{-5, +5, -5},{0.0f, 0.0f}},	//左上
	//	{{+5, -5, -5},{1.0f, 1.0f}},	//右下
	//	{{+5, +5, -5},{1.0f, 0.0f}},	//右上
	//	//後
	//	{{-5, -5, +5},{0.0f, 1.0f}},	//左下
	//	{{-5, +5, +5},{0.0f, 0.0f}},	//左上
	//	{{+5, -5, +5},{1.0f, 1.0f}},	//右下
	//	{{+5, +5, +5},{1.0f, 0.0f}},	//右上
	//	//左
	//	{{-5, -5, -5},{0.0f, 1.0f}},	//左下
	//	{{-5, -5, +5},{0.0f, 0.0f}},	//左上
	//	{{-5, +5, -5},{1.0f, 1.0f}},	//右下
	//	{{-5, +5, +5},{1.0f, 0.0f}},	//右上
	//	//右
	//	{{+5, -5, -5},{0.0f, 1.0f}},	//左下
	//	{{+5, -5, +5},{0.0f, 0.0f}},	//左上
	//	{{+5, +5, -5},{1.0f, 1.0f}},	//右下
	//	{{+5, +5, +5},{1.0f, 0.0f}},	//右上
	//	//下
	//	{{-5, +5, -5},{0.0f, 1.0f}},	//左下
	//	{{+5, +5, -5},{0.0f, 0.0f}},	//左上
	//	{{-5, +5, +5},{1.0f, 1.0f}},	//右下
	//	{{+5, +5, +5},{1.0f, 0.0f}},	//右上
	//	//上
	//	{{-5, -5, -5},{0.0f, 1.0f}},	//左下
	//	{{+5, -5, -5},{0.0f, 0.0f}},	//左上
	//	{{-5, -5, +5},{1.0f, 1.0f}},	//右下
	//	{{+5, -5, +5},{1.0f, 0.0f}},	//右上
	//};

	//インデックスデータ
	//unsigned short indices[] = {
	//	//
	//	0,1,2,
	//	1,2,3,
	//	//
	//	4,5,6,
	//	5,6,7,
	//	//
	//	8,9,10,
	//	9,10,11,
	//	//
	//	12,13,14,
	//	13,14,15,
	//	//
	//	16,17,18,
	//	17,18,19,
	//	//
	//	20,21,22,
	//	21,22,23
	//};

	//XMFLOAT3 vertices[DIV + 1] = {
	//	{0,0,0},
	//};

	////星
	//for (int i = 0; i < DIV; i++)
	//{
	//	if (i % 2 == 0)
	//	{
	//		vertices[i].x = radius * sinf(2 * XM_PI / DIV * i);
	//		vertices[i].y = radius * cosf(2 * XM_PI / DIV * i);
	//		vertices[i].z = 0;
	//	}
	//	else
	//	{
	//		vertices[i].x = radius2 * sinf(2 * XM_PI / DIV * i);
	//		vertices[i].y = radius2 * cosf(2 * XM_PI / DIV * i);
	//		vertices[i].z = 0;
	//	}
	//}
	//vertices[DIV] = { 0,0,0 };
	//
	//unsigned short indices[DIV * 3] =
	//{
	//	0,0,0
	//};

	//インデックスデータ
	//for (int i = 0; i < DIV; i++)
	//{
	//	indices[i * 3] = i;
	//	indices[i * 3 + 1] = i + 1;
	//	indices[i * 3 + 2] = DIV;
	//}
	//indices[DIV * 3 - 2] = 0;

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	//UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * _countof(vertices));


#pragma endregion 頂点データー

	//---------------------------

	#pragma region 頂点バッファの確保
	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};   // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

	D3D12_RESOURCE_DESC resDesc{};  // リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの生成
	ID3D12Resource* vertBuff = nullptr;
	result = device->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE, &resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

#pragma endregion 頂点バッファの確保

	//---------------------------

	#pragma region 頂点バッファへのデータ転送
// GPU上のバッファに対応した仮想メモリを取得
	//Vertex* vertMap = nullptr;
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];   // 座標をコピー
	}

	// マップを解除
	vertBuff->Unmap(0, nullptr);

#pragma endregion 頂点バッファへのデータ転送

	//-----------------------------

	#pragma region 頂点バッファビューの作成
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(XMFLOAT3);

#pragma endregion 頂点バッファへビューの作成

	//-------------------------------

	#pragma region 頂点シェーダーファイルの読み込みとコンパイル

	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile( L"BasicVS.hlsl",  // シェーダファイル名
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0, &vsBlob, &errorBlob);


#pragma endregion 頂点シェーダーファイルの読み込みとコンパイル

	//-------------------------

	#pragma region シェーダーのエラー内容を表示
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

#pragma endregion シェーダーのエラー内容を表示

	//---------------------

	#pragma region ピクセルシェーダーの読み込みとコンパイル
// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile( L"BasicPS.hlsl",   // シェーダファイル名
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0, &psBlob, &errorBlob);


#pragma endregion ピクセルシェーダーの読み込みとコンパイル

	//------------------------

	#pragma region シェーダーのエラー内容を表示
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		string error;
		error.resize(errorBlob->GetBufferSize());

		copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

#pragma endregion シェーダーのエラー内容を表示

	//---------------------

	#pragma region 頂点レイアウト
	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{// xyz座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		//{// uv座標
		//	"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		//},
	};

#pragma endregion 頂点レイアウト

	//------------------------

	#pragma region グラフィックスパイプライン設定
// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

		#pragma region デプスステンシルステートの設定

	////デプスステンシルステートの設定
	//pipelineDesc.DepthStencilState.DepthEnable = true;		//深度テストを行う
	//pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	//書き込み許可
	//pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	//pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;		//深度値フォーマット

		#pragma endregion デプスステンシルステートの設定

	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;  // カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	//ブレンド設定
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	blenddesc.BlendEnable = true;						//ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;		//加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;			//ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;		//デストの値を  0%使う

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//1.0f-ソースのアルファ値

	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

#pragma endregion グラフィックスパイプライン設定

	//-----------------

	#pragma region ルートシグネチャの設定
	ID3D12RootSignature* rootsignature;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//rootSignatureDesc.pParameters = rootparams;
	//rootSignatureDesc.NumParameters = _countof(rootparams);
	////テクスチャ追加
	//rootSignatureDesc.pStaticSamplers = &samplerDesc;
	//rootSignatureDesc.NumStaticSamplers = 1;

	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();

	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootsignature;
#pragma endregion ルートシグネチャの設定

	//----------------------

	#pragma region パイプラインステートの生成
	ID3D12PipelineState* pipelinestate = nullptr;
	result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelinestate));
	assert(SUCCEEDED(result));
#pragma endregion パイプラインステートの生成

#pragma endregion 描画初期化

	//変数宣言
	//-----------------------
#pragma region DirectInput
	BYTE key[256], oldkey[256];
	for (int i = 0; i < 256; i++)
	{
		oldkey[i] = key[i];
		key[i] = 0;
	}
	keyboard->Acquire();
	result = keyboard->GetDeviceState(sizeof(key), key);
#pragma endregion DirectInput

	float clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f }; // 青っぽい色
	//-----------------------



	while (!key[DIK_ESCAPE])
	{
#pragma region メッセージ
		// メッセージがある？
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); // キー入力メッセージの処理
			DispatchMessage(&msg);  //プロシージャにメッセージを送る
		}

		//Xボタンで終了メッセージが来たらゲームループを抜ける
		if (msg.message == WM_QUIT)
		{
			break;
		}

#pragma endregion メッセージ

#pragma region DirectInput

		for (int i = 0; i < 256; i++)
		{
			oldkey[i] = key[i];
			key[i] = 0;
		}
		keyboard->Acquire();
		result = keyboard->GetDeviceState(sizeof(key), key);

#pragma endregion DirectInput


		if (key[DIK_SPACE])
		{
			clearColor[0] = 0.5f;
			clearColor[2] = 0.1f;
		}
		else
		{
			clearColor[2] = 0.5f;
			clearColor[0] = 0.1f;
		}


#pragma region DirectX毎フレームの処理 前

	#pragma region リソースバリアの変更
		//バックバッファの番号を取得（2つなので0番か1番）
		UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

		// 1．リソースバリアで書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = backBuffers[bbIndex]; // バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画
		commandList->ResourceBarrier(1, &barrierDesc);
#pragma endregion リソースバリアの変更

	#pragma region 描画先指定
		// 2．描画先指定
		// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
		//D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr/*&dsvH*/);

#pragma endregion 描画先指定

	#pragma region 画面クリア
		// 3．画面クリア           R     G     B    A
		

		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		/*cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);*/
#pragma endregion 画面クリア

#pragma endregion DirectX毎フレームの処理 前

#pragma region 描画コマンド

	#pragma region ビューポート設定コマンド
		D3D12_VIEWPORT viewport{};

		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		commandList->RSSetViewports(1, &viewport);
#pragma endregion ビューポート設定コマンド5

	#pragma region シザー矩形の設定コマンド
		D3D12_RECT scissorrect{};

		scissorrect.left = 0;
		scissorrect.right = scissorrect.left + window_width;
		scissorrect.top = 0;
		scissorrect.bottom = scissorrect.top + window_height;

		commandList->RSSetScissorRects(1, &scissorrect);
#pragma endregion シザー矩形の設定コマンド

	#pragma region パイプラインステート
		commandList->SetPipelineState(pipelinestate);
		commandList->SetGraphicsRootSignature(rootsignature);

		////デスクリプタヒープを設定
		//ID3D12DescriptorHeap* ppHeaps[] = { basicDescHeap };
		//cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		////定数バッファビューをセット
		////cmdList->SetGraphicsRootDescriptorTable(0, basicDescHeap->GetGPUDescriptorHandleForHeapStart());

		////テクスチャ追加
		////デスクリプタヒープの先頭ハンドルを取得(GPU版)
		//D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		////ヒープの先頭にあるCBVをルートパラメータ0番に設定
		//cmdList->SetGraphicsRootDescriptorTable(0, gpuDescHandle);
		////ハンドルを1つ進める
		//gpuDescHandle.ptr += dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		////ヒープの2番目にあるSRVをルートパラメータを1番に設定
		//cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandle);

#pragma endregion パイプラインステート

	#pragma region プリミティブ形状
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#pragma endregion プリミティブ形状

	#pragma region 頂点バッファビューの設定コマンド
		commandList->IASetVertexBuffers(0, 1, &vbView);
#pragma endregion 頂点バッファビューの設定コマンド

	#pragma region インデックスバッファビューのセット
		//commandList->IASetIndexBuffer(&ibView);
#pragma endregion インデックスバッファビューのセット

	#pragma region 描画コマンド
		//commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);
		commandList->DrawInstanced(_countof(vertices), 1, 0, 0);
#pragma endregion 描画コマンド

#pragma endregion 描画コマンド

#pragma region DirectX毎フレームの処理 後

	#pragma region リソースバリアの復帰コマンド
// 5．リソースバリアを戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;   // 表示に
		commandList->ResourceBarrier(1, &barrierDesc);
#pragma endregion リソースバリアの復帰コマンド

	#pragma region ためておいた命令を実行する
		// 命令のクローズ
		result = commandList->Close();
		assert(SUCCEEDED(result));
		// コマンドリストの実行
		ID3D12CommandList* commandLists[] = { commandList }; // コマンドリストの配列
		commandQueue->ExecuteCommandLists(1, commandLists);

		// バッファをフリップ（裏表の入替え）
		result = swapchain->Present(1, 0);
		assert(SUCCEEDED(result));
#pragma endregion ためておいた命令を実行する

	#pragma region 画面入れ替え
		// コマンドリストの実行完了を待つ
		commandQueue->Signal(fence, ++fenceVal);

		if (fence->GetCompletedValue() != fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		result = commandAllocator->Reset(); // キューをクリア
		assert(SUCCEEDED(result));
		result = commandList->Reset(commandAllocator, nullptr);  // 再びコマンドリストを貯める準備
		assert(SUCCEEDED(result));
#pragma endregion 画面入れ替え

#pragma endregion DirectX毎フレームの処理 後
	}

	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}

//ウィンドウプロシージャ
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// メッセージで分岐
	switch (msg)
	{
	case WM_DESTROY: //ウィンドーが破壊された
		PostQuitMessage(0); // OSに対して、アプリの終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
}