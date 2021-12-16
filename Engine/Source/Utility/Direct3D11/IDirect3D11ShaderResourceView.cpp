// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IDirect3D11ShaderResourceView.h"

#ifdef PLATFORM_WINDOWS

bool IDirect3D11ShaderResourceView::CreateShaderResourceView(
	ID3D11Device& Device,
	ID3D11Resource& Resource,
	const D3D11_SHADER_RESOURCE_VIEW_DESC& Desc) noexcept
{
	if (const auto ResAddr{ reinterpret_cast<std::uintptr_t>(&Resource) };
		ResAddr != LastUsed)
	{
		if (FAILED(Device.CreateShaderResourceView(&Resource, &Desc, &SRV)))
		{
			Clear();
			return false;
		}
		LastUsed = ResAddr;
	}
	BindSlot();
	return true;
}

#endif
