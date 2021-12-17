// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Direct3D11SRV_Texture2D.h"

#ifdef PLATFORM_WINDOWS

#include "Direct3D11Texture2D.h"

bool FDirect3D11SRV_Texture2D::MapResource(ID3D11Device& Device, const FDirect3D11Texture2D& Texture) noexcept
{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{
			.Format{ Texture.Format() },
			.ViewDimension{ ViewDimension },
			.Texture2D{
				.MostDetailedMip{},
				.MipLevels{ 1 } } };

		if (!CreateShaderResourceView(Device, *Texture.GetPtr(), SRVDesc))
		{
			return false;
		}

		return true;
}

#endif
