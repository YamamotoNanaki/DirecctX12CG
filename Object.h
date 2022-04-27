#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

#pragma comment(lib,"d3d12.lib") 

#pragma region 定数バッファ構造体

struct ConstBufferDataTransform
{
	DirectX::XMMATRIX mat;
};
#pragma endregion 定数バッファ構造体

class Object
{
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffTransform;
	//定数バッファマップ
	ConstBufferDataTransform* constMapTransform;
	//アフィン変換情報
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };
	//ワールド変換行列
	XMMATRIX matWorld;
	//親オブジェクトへのポインタ
	Object* parent = nullptr;

	void Initialize(HRESULT& result, ID3D12Device* device);
	void Update(XMMATRIX matView, XMMATRIX matProjection);
	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT indices);
};