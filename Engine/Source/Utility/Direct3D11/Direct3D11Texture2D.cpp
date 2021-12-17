// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11Texture2D.h"

#ifdef PLATFORM_WINDOWS

FDirect3D11Texture2D FDirect3D11Texture2D::GetSwapChainBackBuffer(IDXGISwapChain& SwapChain) noexcept
{
	if (DXGI_SWAP_CHAIN_DESC SwapChainDesc{};
		SUCCEEDED(SwapChain.GetDesc(&SwapChainDesc)))
	{
		if (FDirect3D11Texture2D BackBuffer{};
			SUCCEEDED(SwapChain.GetBuffer(0, IID_PPV_ARGS(&BackBuffer.Texture))))
		{
			BackBuffer.Texture->GetDesc(&BackBuffer.Desc);
			return BackBuffer;
		}
	}
	return {};
}

FDirect3D11Texture2D::FDirect3D11Texture2D(
	ID3D11Device& Device,
	const FImageFrame& Frame,
	UINT BindFlags,
	D3D11_USAGE Usage,
	UINT CPUAccessFlags,
	UINT MiscFlags)
	: FDirect3D11Texture2D()
{
	D3D11_TEXTURE2D_DESC Desc{
		.Width{ Frame.Width<UINT>() },
		.Height{ Frame.Height<UINT>() },
		.MipLevels{ 1 },
		.ArraySize{ 1 },
		.Format{ DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM },
		.SampleDesc{.Count{ 1 }, .Quality{} },
		.Usage{ Usage },
		.BindFlags{ BindFlags },
		.CPUAccessFlags{ CPUAccessFlags },
		.MiscFlags{ MiscFlags } };
	D3D11_SUBRESOURCE_DATA SubRes{
		.pSysMem{ Frame.GetPtr() },
		.SysMemPitch{ Frame.RowPitch<UINT>() },
		.SysMemSlicePitch{ Frame.GetByteSize<UINT>() } };
	auto hr = Device.CreateTexture2D(&Desc, &SubRes, &Texture);
	if (FAILED(hr))
	{
		return;
	}

	this->Desc = std::move(Desc);
}

#endif
