#include "StabilityCalculator.hpp"
#include "Util.hpp"
#include <d3dcompiler.h>
#include "InitialState.hpp"
#include "EqualityChecker.hpp"

StabilityCalculator::StabilityCalculator(ID3D11Device* device, uint32_t width, uint32_t height): mBoardWidth(width), mBoardHeight(height)
{
	CreateTextures(device);
	LoadShaderData(device);
}

StabilityCalculator::~StabilityCalculator()
{
}

void StabilityCalculator::InitTextures(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	UINT clearVal[] = {1, 1, 1, 1};
	dc->ClearUnorderedAccessViewUint(mCurrStabilityUAV.Get(), clearVal);

	uint32_t newWidth  = 0;
	uint32_t newHeight = 0;

	InitialState is(device);
	if(is.LoadFromFile(device, dc, L"InitialState.png", mInitialBoardUAV.Get(), newWidth, newHeight) == LoadError::LOAD_SUCCESS)
	{
		mBoardWidth  = newWidth;
		mBoardHeight = newHeight;
	}
	else
	{
		is.CreateDefault(dc, mInitialBoardUAV.Get(), mBoardWidth, mBoardHeight);
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> initialBoardTex;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> currBoardTex;

	mInitialBoardSRV->GetResource(reinterpret_cast<ID3D11Resource**>(initialBoardTex.GetAddressOf()));
	mCurrBoardSRV->GetResource(reinterpret_cast<ID3D11Resource**>(currBoardTex.GetAddressOf()));

	dc->CopyResource(currBoardTex.Get(), initialBoardTex.Get());

	std::swap(mCurrStabilitySRV, mPrevStabilitySRV);
	std::swap(mCurrStabilityUAV, mPrevStabilityUAV);
	std::swap(mCurrBoardSRV,     mPrevBoardSRV);
	std::swap(mCurrBoardUAV,     mPrevBoardUAV);
}

void StabilityCalculator::StabilityNextStep(ID3D11DeviceContext* dc)
{
	ID3D11ShaderResourceView*  stabilityNextStepSRVs[] = { mPrevBoardSRV.Get(), mPrevStabilitySRV.Get() };
	ID3D11UnorderedAccessView* stabilityNextStepUAVs[] = { mCurrBoardUAV.Get(), mCurrStabilityUAV.Get() };

	dc->CSSetShaderResources(0, 2, stabilityNextStepSRVs);
	dc->CSSetUnorderedAccessViews(0, 2, stabilityNextStepUAVs, nullptr);

	dc->CSSetShader(mStabilityNextStepShader.Get(), nullptr, 0);
	dc->Dispatch((uint32_t)(ceilf(mBoardWidth / 32.0f)), (uint32_t)(ceilf(mBoardHeight / 32.0f)), 1);

	ID3D11ShaderResourceView*  nullSRVs[] = { nullptr, nullptr };
	ID3D11UnorderedAccessView* nullUAVs[] = { nullptr, nullptr };

	dc->CSSetShaderResources(0, 2, nullSRVs);
	dc->CSSetUnorderedAccessViews(0, 2, nullUAVs, nullptr);
	dc->CSSetShader(nullptr, nullptr, 0);

	std::swap(mCurrStabilitySRV, mPrevStabilitySRV);
	std::swap(mCurrStabilityUAV, mPrevStabilityUAV);
	std::swap(mCurrBoardSRV,     mPrevBoardSRV);
	std::swap(mCurrBoardUAV,     mPrevBoardUAV);
}

bool StabilityCalculator::CheckEquality(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	EqualityChecker checker(device, mBoardWidth, mBoardHeight);
	return checker.CheckEquality(dc, mPrevBoardSRV.Get(), mInitialBoardSRV.Get());
}

ID3D11ShaderResourceView* StabilityCalculator::GetLastStabilityState() const
{
	return mPrevStabilitySRV.Get();
}

void StabilityCalculator::CreateTextures(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC boardTexDesc;
	boardTexDesc.Width              = mBoardWidth;
	boardTexDesc.Height             = mBoardHeight;
	boardTexDesc.Format             = DXGI_FORMAT_R8_UINT;
	boardTexDesc.Usage              = D3D11_USAGE_DEFAULT;
	boardTexDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	boardTexDesc.CPUAccessFlags     = 0;
	boardTexDesc.ArraySize          = 1;
	boardTexDesc.MipLevels          = 1;
	boardTexDesc.SampleDesc.Count   = 1;
	boardTexDesc.SampleDesc.Quality = 0;
	boardTexDesc.MiscFlags          = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> prevStabilityTex = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> currStabilityTex = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> prevBoardTex = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> currBoardTex = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> initialBoardTex = nullptr; //For comparison

	ThrowIfFailed(device->CreateTexture2D(&boardTexDesc, nullptr, prevStabilityTex.GetAddressOf()));
	ThrowIfFailed(device->CreateTexture2D(&boardTexDesc, nullptr, currStabilityTex.GetAddressOf()));

	ThrowIfFailed(device->CreateTexture2D(&boardTexDesc, nullptr, prevBoardTex.GetAddressOf()));
	ThrowIfFailed(device->CreateTexture2D(&boardTexDesc, nullptr, currBoardTex.GetAddressOf()));

	ThrowIfFailed(device->CreateTexture2D(&boardTexDesc, nullptr, initialBoardTex.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC boardSrvDesc;
	boardSrvDesc.Format                    = DXGI_FORMAT_R8_UINT;
	boardSrvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	boardSrvDesc.Texture2D.MipLevels       = 1;
	boardSrvDesc.Texture2D.MostDetailedMip = 0;

	ThrowIfFailed(device->CreateShaderResourceView(prevStabilityTex.Get(), &boardSrvDesc, mPrevStabilitySRV.GetAddressOf()));
	ThrowIfFailed(device->CreateShaderResourceView(currStabilityTex.Get(), &boardSrvDesc, mCurrStabilitySRV.GetAddressOf()));

	ThrowIfFailed(device->CreateShaderResourceView(prevBoardTex.Get(), &boardSrvDesc, mPrevBoardSRV.GetAddressOf()));
	ThrowIfFailed(device->CreateShaderResourceView(currBoardTex.Get(), &boardSrvDesc, mCurrBoardSRV.GetAddressOf()));

	ThrowIfFailed(device->CreateShaderResourceView(initialBoardTex.Get(), &boardSrvDesc, mInitialBoardSRV.GetAddressOf()));

	D3D11_UNORDERED_ACCESS_VIEW_DESC boardUavDesc;
	boardUavDesc.Format             = DXGI_FORMAT_R8_UINT;
	boardUavDesc.ViewDimension      = D3D11_UAV_DIMENSION_TEXTURE2D;
	boardUavDesc.Texture2D.MipSlice = 0;

	ThrowIfFailed(device->CreateUnorderedAccessView(prevStabilityTex.Get(), &boardUavDesc, mPrevStabilityUAV.GetAddressOf()));
	ThrowIfFailed(device->CreateUnorderedAccessView(currStabilityTex.Get(), &boardUavDesc, mCurrStabilityUAV.GetAddressOf()));

	ThrowIfFailed(device->CreateUnorderedAccessView(prevBoardTex.Get(), &boardUavDesc, mPrevBoardUAV.GetAddressOf()));
	ThrowIfFailed(device->CreateUnorderedAccessView(currBoardTex.Get(), &boardUavDesc, mCurrBoardUAV.GetAddressOf()));

	ThrowIfFailed(device->CreateUnorderedAccessView(initialBoardTex.Get(), &boardUavDesc, mInitialBoardUAV.GetAddressOf()));
}

void StabilityCalculator::LoadShaderData(ID3D11Device* device)
{
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;

	ThrowIfFailed(D3DReadFileToBlob((GetShaderPath() + L"StabilityNextStepCS.cso").c_str(), shaderBlob.GetAddressOf()));
	ThrowIfFailed(device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, mStabilityNextStepShader.GetAddressOf()));
}
