// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "DelegateHandle.h"
#include "IMulticastDelegate.h"

ADelegateHandle::ADelegateHandle(ADelegateHandle&& Other) noexcept
	: Issuer{ std::move(Other.Issuer) }
	, Index{ std::move(Other.Index) }
{
	Other.Release();
}

ADelegateHandle::ADelegateHandle(IMulticastDelegate& Issuer, std::size_t Index)
	: Issuer{ Issuer }
	, Index{ Index }
{
}

ADelegateHandle& ADelegateHandle::operator=(ADelegateHandle&& Other) & noexcept
{
	if (this != &Other)
	{
		Issuer = std::move(Other.Issuer);
		Index = std::move(Other.Index);
		Other.Release();
	}
	return *this;
}

void ADelegateHandle::Release() noexcept
{
	if (IsValid())
	{
		Issuer->RemoveDynamic(*this);
	}
	Issuer.Release();
	Index = InvalidHandle;
}

FDelegateHandles& FDelegateHandles::operator+=(ADelegateHandle&& Handle) &
{
	if (Handle.IsValid())
	{
		Handles.emplace_back(std::move(Handle));
	}
	return *this;
}
