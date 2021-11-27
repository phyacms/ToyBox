// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IMulticastDelegate.h"

IMulticastDelegate::IMulticastDelegate()
	: TObject<IMulticastDelegate>(*this)
	, Counter{ ADelegateHandle::InvalidHandle }
	, Revoked{}
	, Mutex{}
{
}

ADelegateHandle IMulticastDelegate::Issue()
{
	std::size_t Issued{ ADelegateHandle::InvalidHandle };

	std::unique_lock<std::mutex> Lock{ Mutex };
	if (!Revoked.empty())
	{
		Issued = *Revoked.cbegin();
		Revoked.erase(Issued);
	}
	else if (Counter != std::numeric_limits<std::size_t>::max())
	{
		Issued = ++Counter;
	}

	return Issued != ADelegateHandle::InvalidHandle
		? ADelegateHandle{ *this, Issued }
		: ADelegateHandle{};
}

std::size_t IMulticastDelegate::Revoke(ADelegateHandle& Handle) noexcept
{
	std::size_t Index{ ADelegateHandle::InvalidHandle };
	if (Handle.IsValid() && Handle.IsIssuedBy(*this))
	{
		Index = Handle.Index;

		std::unique_lock<std::mutex> Lock{ Mutex };
		Revoked.emplace(Handle.Index);

		Handle.Issuer.Release();
		Handle.Index = ADelegateHandle::InvalidHandle;
	}
	return Index;
}
