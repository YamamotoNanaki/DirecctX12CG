#include "DirectX12.h"

using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;

void DirectX12::Adapter(HRESULT& result)
{
	// DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	// �A�_�v�^�[�̗񋓗p
	vector< ComPtr<IDXGIAdapter4>> adapters;
	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter); //���I�z��ɒǉ�����
	}

	for (size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc;
		adapters[i]->GetDesc3(&adapterDesc); // �A�_�v�^�[�̏����擾

		// �\�t�g�E�F�A�f�o�C�X�����
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			tmpAdapter = adapters[i]; //�̗p
			break;
		}
	}
}

void DirectX12::CreateDevice(HRESULT& result)
{
	// �Ή����x���̔z��
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
		// �̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&device));
		if (result == S_OK)
		{
			// �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}
}

void DirectX12::CmdList(HRESULT& result)
{
	// �R�}���h�A���P�[�^�𐶐�
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(result));

	// �R�}���h���X�g�̐���
	result = device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator.Get(), nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));
}

void DirectX12::CmdQueue(HRESULT& result)
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));
}

void DirectX12::SwapChain(HRESULT& result, HWND hwnd, int window_width, int window_height)
{
	// �e��ݒ�����ăX���b�v�`�F�[���𐶐�
	swapChainDesc.Width = window_width;
	swapChainDesc.Height = window_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // �F���̏���
	swapChainDesc.SampleDesc.Count = 1; // �}���`�T���v�����Ȃ�
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // �o�b�N�o�b�t�@�p
	swapChainDesc.BufferCount = 2; // �o�b�t�@�����Q�ɐݒ�
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //�t���b�v��͔j��
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr <IDXGISwapChain1> swapchain1;

	result = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapchain1);

	swapchain1.As(&swapchain);
}

void DirectX12::Heap(HRESULT& result)
{
	// �e��ݒ�����ăf�X�N���v�^�q�[�v�𐶐�
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount;	// ���\�̂Q��
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeaps));
}

void DirectX12::TargetView(HRESULT& result)
{
	backBuffers.resize(swapChainDesc.BufferCount);

	for (size_t i = 0; i < backBuffers.size(); i++)
	{
		// �X���b�v�`�F�[������o�b�t�@���擾
		swapchain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		// �f�X�N���v�^�q�[�v�̃n���h���̎擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		// �����\���A�h���X�������
		rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// �����_�[�^�[�Q�b�g�r���[�̐���
		device->CreateRenderTargetView(backBuffers[i].Get(), &rtvDesc, rtvHandle);
	}
}

void DirectX12::Fence(HRESULT& result)
{
	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
}

void DirectX12::ResourceBarrierSet()
{
	bbIndex = swapchain->GetCurrentBackBufferIndex();
	// 1�D���\�[�X�o���A�ŏ������݉\�ɕύX
	barrierDesc.Transition.pResource = backBuffers[bbIndex].Get(); // �o�b�N�o�b�t�@���w��
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // �\������
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`��
	commandList->ResourceBarrier(1, &barrierDesc);
}

void DirectX12::ExecutCommand(HRESULT& result)
{
	// ���߂̃N���[�Y
	result = commandList->Close();
	assert(SUCCEEDED(result));
	// �R�}���h���X�g�̎��s
	ID3D12CommandList* commandLists[] = { commandList.Get() }; // �R�}���h���X�g�̔z��
	commandQueue->ExecuteCommandLists(1, commandLists);

	// �o�b�t�@���t���b�v�i���\�̓��ւ��j
	result = swapchain->Present(1, 0);
	assert(SUCCEEDED(result));
	// �R�}���h���X�g�̎��s������҂�
	commandQueue->Signal(fence.Get(), ++fenceVal);
}

void DirectX12::RenderTarget()
{
	// 2�D�`���w��
	// �����_�[�^�[�Q�b�g�r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
	rtvHandle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
	//�[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvH);
}

void DirectX12::Clear()
{
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectX12::ResourceBarrierReturn()
{
	// 5�D���\�[�X�o���A��߂�
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`��
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;   // �\����
	commandList->ResourceBarrier(1, &barrierDesc);
}

DirectX12::DirectX12(HRESULT& result, HWND hwnd, int window_width, int window_height)
{
	Adapter(result);
	CreateDevice(result);
	CmdList(result);
	CmdQueue(result);
	SwapChain(result, hwnd, window_width, window_height);
	Heap(result);
	TargetView(result);
	Fence(result);
	DepthDesc(result, window_width, window_height);
	DepthHeap(result);
}

void DirectX12::BufferSwap()
{
	if (fence->GetCompletedValue() != fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
}

void DirectX12::DepthDesc(HRESULT result,int window_width, int window_height)
{
	//���\�[�X�ݒ�
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = window_width;			//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResDesc.Height = window_height;		//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//�f�v�X�X�e���V��

	//�[�x�n�p�q�[�v�v���p�e�B
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//�[�x�n�̃N���A��ݒ�
	depthClearValue.DepthStencil.Depth = 1.0f;			//�[�x�l1(�ő�l)�ŃN���A
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;		//�[�x�l�t�H�[�}�b�g

	result = device->CreateCommittedResource(
		&depthHeapProp, D3D12_HEAP_FLAG_NONE, &depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,					//�[�x�l�������݂Ɏg�p
		&depthClearValue, IID_PPV_ARGS(&depthBuffer));
}

void DirectX12::CommandReset(HRESULT& result)
{
	result = commandAllocator->Reset(); // �L���[���N���A
	assert(SUCCEEDED(result));
	result = commandList->Reset(commandAllocator.Get(), nullptr);  // �ĂуR�}���h���X�g�𒙂߂鏀��
	assert(SUCCEEDED(result));
}

void DirectX12::DrawBefore()
{
	ResourceBarrierSet();
	RenderTarget();
	Clear();
}

void DirectX12::DrawAfter(HRESULT& result)
{
	ResourceBarrierReturn();
	ExecutCommand(result);
	BufferSwap();
	CommandReset(result);
}

void DirectX12::SetClearColor(XMFLOAT4 color)
{
	clearColor[0] = color.x;
	clearColor[1] = color.y;
	clearColor[2] = color.z;
	clearColor[3] = color.w;
}

void DirectX12::DepthHeap(HRESULT result)
{
	//�[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
	dsvHeapDesc.NumDescriptors = 1;						//�[�x�r���[��1��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;	//�f�v�X�X�e���V���r���[
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//�[�x�r���[�쐬
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuffer.Get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());
}

void DirectX12::SetClearColor(float R, float G, float B)
{
	clearColor[0] = R;
	clearColor[1] = G;
	clearColor[2] = B;
	clearColor[3] = 1;
}