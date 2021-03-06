#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <cstdint>
#include "Renderer.hpp"

class DisplayRenderer final: public Renderer
{
	struct CBParamsClickRuleStruct
	{
		uint32_t Flags;
	};

public:
	DisplayRenderer(int gpuIndex, HWND previewWnd, HWND clickRuleWnd);
	~DisplayRenderer();

	void ResizePreviewArea(uint32_t newWidth, uint32_t newHeight) override;

	bool GetClickRuleGridVisible() const override;
	void SetClickRuleGridVisible(bool bVisible) override;

	void DrawPreview()   override; //Only to be called from the background thread
	void DrawClickRule() override; //Only to be called from the background thread

	void SetCurrentBoard(ID3D11ShaderResourceView* srv) override;
	void SetCurrentClickRule(ID3D11ShaderResourceView* srv) override;

private:
	void CreateSwapChains(HWND previewWnd, HWND clickRuleWnd);
	void LoadShaders();

	void CreateSwapChain(IDXGIFactory* factory, HWND hwnd, uint32_t width, uint32_t height, IDXGISwapChain** outSwapChain);
	void InitRenderAreaSize(IDXGISwapChain* swapChain, uint32_t width, uint32_t height, ID3D11RenderTargetView** outRTV, D3D11_VIEWPORT* outViewport);

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> mPreviewSwapChain;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mClickRuleSwapChain;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mPreviewRTV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mClickRuleRTV;

	D3D11_VIEWPORT mPreviewViewport;
	D3D11_VIEWPORT mClickRuleViewport;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> mBoardSampler;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> mRenderVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  mRenderPixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  mRenderClickRulePixelShader;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBufferParamsClickRule;
	CBParamsClickRuleStruct              mCBufferParamsClickRuleCopy;

	ID3D11ShaderResourceView* mCurrentBoardSRV;     //Non-owning observer pointer
	ID3D11ShaderResourceView* mCurrentClickRuleSRV; //Non-owning observer pointer
};