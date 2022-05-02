#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <wrl.h>
#include <DirectXMath.h>

#pragma comment(lib,"d3d12.lib") 
#pragma comment(lib,"dxgi.lib")


namespace IF
{
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
		// ���\��2���ɂ���
		vector< ComPtr<ID3D12Resource>> backBuffers = vector<ComPtr<ID3D12Resource>>(2);
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		// �t�F���X�̐���
		ComPtr<ID3D12Fence> fence = nullptr;
		UINT64 fenceVal = 0;
		vector<D3D12_VIEWPORT> viewport{};

	private:
		//������
		ComPtr < IDXGIAdapter4> tmpAdapter = nullptr;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		//�[�x�o�b�t�@
		D3D12_RESOURCE_DESC depthResDesc{};
		D3D12_HEAP_PROPERTIES depthHeapProp{};
		D3D12_CLEAR_VALUE depthClearValue{};
		ComPtr < ID3D12Resource> depthBuffer = nullptr;
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;

		//�`��O����
		UINT bbIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvH;
		D3D12_RESOURCE_BARRIER barrierDesc{};

		// 3�D��ʃN���A           R     G     B    A
		float clearColor[4] = { 0.1f ,0.25f ,0.5f ,1.0f };

	private:
		HRESULT Adapter();
		HRESULT CreateDevice();
		HRESULT CmdList();
		HRESULT CmdQueue();
		HRESULT SwapChain(HWND hwnd, int window_width, int window_height);
		void Heap();
		void TargetView();
		HRESULT Fence();
		HRESULT DepthDesc(int window_width, int window_height);
		HRESULT DepthHeap();

		void ResourceBarrierSet();
		void RenderTarget();
		void Clear();

		void ResourceBarrierReturn();
		HRESULT ExecutCommand();
		void BufferSwap();
		HRESULT CommandReset();

	public:
		DirectX12(HRESULT& result, HWND hwnd, int window_width, int window_height);
		void DrawBefore();
		HRESULT DrawAfter();
		void SetNewViewPort(float width, float height, float topX, float topY, float minDepth = 0.0f, float maxDepth = 1.0f);

	public:
		void SetClearColor(XMFLOAT4 color);
		void SetClearColor(float Red, float Green, float Bule);
	};
}