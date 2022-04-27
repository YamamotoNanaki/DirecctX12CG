#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <wrl.h>
#include <DirectXMath.h>

#pragma comment(lib,"d3d12.lib") 
#pragma comment(lib,"dxgi.lib")


class DirectX12
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	template<class T> using vector = std::vector<T>;

public:
	ComPtr < ID3D12Device> device = nullptr;
	ComPtr < IDXGIFactory6> dxgiFactory = nullptr;
	ComPtr < IDXGISwapChain4> swapchain = nullptr;
	ComPtr < ID3D12CommandAllocator> commandAllocator = nullptr;
	ComPtr < ID3D12GraphicsCommandList> commandList = nullptr;
	ComPtr < ID3D12CommandQueue> commandQueue = nullptr;
	ComPtr < ID3D12DescriptorHeap> rtvHeaps = nullptr;
	// 裏表の2つ分について
	vector< ComPtr<ID3D12Resource>> backBuffers = vector<ComPtr<ID3D12Resource>>(2);
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	// フェンスの生成
	ComPtr<ID3D12Fence> fence = nullptr;
	UINT64 fenceVal = 0;

private:
	//初期化
	ComPtr < IDXGIAdapter4> tmpAdapter = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	//深度バッファ
	D3D12_RESOURCE_DESC depthResDesc{};
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	D3D12_CLEAR_VALUE depthClearValue{};
	ComPtr < ID3D12Resource> depthBuffer = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;

	//描画前処理
	UINT bbIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
	D3D12_RESOURCE_BARRIER barrierDesc{};

	// 3．画面クリア           R     G     B    A
	float clearColor[4] = { 0.1f ,0.25f ,0.5f ,1.0f };

private:
	void Adapter(HRESULT& result);
	void CreateDevice(HRESULT& result);
	void CmdList(HRESULT& result);
	void CmdQueue(HRESULT& result);
	void SwapChain(HRESULT& result, HWND hwnd, int window_width, int window_height);
	void Heap(HRESULT& result);
	void TargetView(HRESULT& result);
	void Fence(HRESULT& result);
	void DepthDesc(HRESULT result, int window_width, int window_height);
	void DepthHeap(HRESULT result);

	void ResourceBarrierSet();
	void RenderTarget();
	void Clear();

	void ResourceBarrierReturn();
	void ExecutCommand(HRESULT& result);
	void BufferSwap();
	void CommandReset(HRESULT& result);

public:
	DirectX12(HRESULT& result, HWND hwnd,int window_width, int window_height);
	void DrawBefore();
	void DrawAfter(HRESULT& result);

public:
	void SetClearColor(XMFLOAT4 color);
	void SetClearColor(float Red, float Green, float Bule);
};