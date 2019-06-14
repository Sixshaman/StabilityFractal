#pragma once

//http://lightstrout.com/index.php/2019/05/31/stability-fractal/

#include <d3d11.h>

#include <Windows.h>
#include <wrl/client.h>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "Downscaler.hpp"
#include "StabilityCalculator.hpp"

class FractalGen
{
public:
	FractalGen(uint32_t pow2Size);
	~FractalGen();

	void ComputeFractal(bool SaveVideoFrames);
	void SaveFractalImage(const std::string& filename);

private:
	Microsoft::WRL::ComPtr<ID3D11Device>        mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDeviceContext;

	std::unique_ptr<Downscaler>          mDownscaler;
	std::unique_ptr<StabilityCalculator> mStabilityCalculator;

	uint32_t mSizeLo;
};