#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dinput.h>

using namespace DirectX;
using namespace std;

#pragma comment(lib,"d3d12.lib") 
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//�E�B���h�E�v���V�[�W��
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	//--- �E�C���h�E�̐������� -----------------------------------------------------//

// �R���\�[���w�̕����o��
//OutputDebugStringA("Hello,DirectX!!\n");

// �E�B���h�E�̐����i�O�����j
#pragma region �E�B���h�E�̐����i�O�����j

	// �E�B���h�E�T�C�Y
	const int window_width = 1280;  // ����
	const int window_height = 720;  // �c��

	WNDCLASSEX w{}; // �E�B���h�E�N���X�̐ݒ�
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc; // �E�B���h�E�v���V�[�W����ݒ�
	w.lpszClassName = L"DirectX12Game"; // �E�B���h�E�N���X��
	w.hInstance = GetModuleHandle(nullptr); // �E�B���h�E�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW); // �J�[�\���w��

	// �E�B���h�E�N���X��OS�ɓo�^
	RegisterClassEx(&w);
	// �E�B���h�E�T�C�Y{ X���W Y���W ���� �c�� }
	RECT wrc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // �����ŃT�C�Y�␳

	// �E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(w.lpszClassName, // �N���X��
		L"DirectXGame",         // �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,        // �W���I�ȃE�B���h�E�X�^�C��
		CW_USEDEFAULT,              // �\��X���W�iOS�ɔC����j
		CW_USEDEFAULT,              // �\��Y���W�iOS�ɔC����j
		wrc.right - wrc.left,       // �E�B���h�E����
		wrc.bottom - wrc.top,   // �E�B���h�E�c��
		nullptr,                // �e�E�B���h�E�n���h��
		nullptr,                // ���j���[�n���h��
		w.hInstance,            // �Ăяo���A�v���P�[�V�����n���h��
		nullptr);               // �I�v�V����

	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

#pragma endregion �E�B���h�E�̐����i�O�����j

	//------------------------------------------------------------------------------//

	MSG msg{}; // ���b�Z�[�W

#pragma region �f�o�b�O
#ifdef _DEBUG
// �f�o�b�N���C���[���I����
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif
#pragma endregion �f�o�b�O

#pragma region DirectX����������

	#pragma region DirectX������
	HRESULT result;
	ID3D12Device* device = nullptr;
	IDXGIFactory6* dxgiFactory = nullptr;
	IDXGISwapChain4* swapchain = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	ID3D12CommandQueue* cmdQueue = nullptr;
	ID3D12DescriptorHeap* rtvHeaps = nullptr;
#pragma endregion DirectX������

	#pragma region �O���t�B�b�N�X�{�[�h�̃A�_�v�^���
	// DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	// �A�_�v�^�[�̗񋓗p
	vector<IDXGIAdapter4*> adapters;
	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIAdapter4* tmpAdapter = nullptr;
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND; i++)
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
#pragma endregion �O���t�B�b�N�X�{�[�h�̃A�_�v�^���

	#pragma region �f�o�C�X�̐���
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
		result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&device));
		if (result == S_OK)
		{
			// �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}

#pragma endregion �f�o�C�X�̐���

	#pragma region �R�}���h���X�g
	// �R�}���h�A���P�[�^�𐶐�
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(result));

	// �R�}���h���X�g�̐���
	result = device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator, nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));
#pragma endregion �R�}���h���X�g

	#pragma region �R�}���h�L���[
	// �W���ݒ�ŃR�}���h�L���[�𐶐�
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&cmdQueue));
	assert(SUCCEEDED(result));
#pragma endregion �R�}���h�L���[

	#pragma region �X���b�v�`�F�[���̐���
	// �e��ݒ�����ăX���b�v�`�F�[���𐶐�
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = window_width;
	swapChainDesc.Height = window_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // �F���̏���
	swapChainDesc.SampleDesc.Count = 1; // �}���`�T���v�����Ȃ�
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // �o�b�N�o�b�t�@�p
	swapChainDesc.BufferCount = 2; // �o�b�t�@�����Q�ɐݒ�
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //�t���b�v��͔j��
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue, hwnd, &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)&swapchain);
#pragma endregion �X���b�v�`�F�[���̐���

	#pragma region �f�X�N���v�^�q�[�v�̐���
	// �e��ݒ�����ăf�X�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount;	// ���\�̂Q��
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeaps));
#pragma endregion �f�X�N���v�^�q�[�v�̐���

	#pragma region �����_�[�^�[�Q�b�g�r���[�̐���
	// ���\��2���ɂ���
	vector<ID3D12Resource*> backBuffers(2);
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
		device->CreateRenderTargetView(backBuffers[i], &rtvDesc, rtvHandle);
	}
#pragma endregion �����_�[�^�[�Q�b�g�r���[�̐���

	#pragma region �t�F���X�̐���
	// �t�F���X�̐���
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
#pragma endregion �t�F���X�̐���

	// DirectX���������� �����܂�
#pragma endregion DirectX����������



	while (true)
	{
#pragma region ���b�Z�[�W
		// ���b�Z�[�W������H
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); // �L�[���̓��b�Z�[�W�̏���
			DispatchMessage(&msg);  //�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
		}

		//X�{�^���ŏI�����b�Z�[�W��������Q�[�����[�v�𔲂���
		if (msg.message == WM_QUIT)
		{
			break;
		}

#pragma endregion ���b�Z�[�W

#pragma region DirectX���t���[���̏���
#pragma endregion DirectX���t���[���̏���


	}

	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}

//�E�B���h�E�v���V�[�W��
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// ���b�Z�[�W�ŕ���
	switch (msg)
	{
	case WM_DESTROY: //�E�B���h�[���j�󂳂ꂽ
		PostQuitMessage(0); // OS�ɑ΂��āA�A�v���̏I����`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // �W���̏������s��
}