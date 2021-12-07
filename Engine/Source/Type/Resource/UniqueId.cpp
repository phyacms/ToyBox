// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "UniqueId.h"

FUniqueId::FUniqueId(FUniqueId&& Other) noexcept
	: Arg{ std::move(Other.Arg) }
	, Value{ std::move(Other.Value) }
{
	Other.Arg = InvalidArg;
	Other.Value = InvalidId;
}

FUniqueId& FUniqueId::operator=(FUniqueId&& Other) & noexcept
{
	if (this != &Other)
	{
		Arg = std::move(Other.Arg);
		Value = std::move(Other.Value);
		Other.Arg = InvalidArg;
		Other.Value = InvalidId;
	}
	return *this;
}

void FUniqueId::Swap(FUniqueId& Other) noexcept
{
	std::swap(Arg, Other.Arg);
	std::swap(Value, Other.Value);
}

AUniqueId::AUniqueId(AUniqueId&& Other) noexcept
	: Issuer{ std::move(Other.Issuer) }
	, Id{ std::move(Other.Id) }
{
	Other.Release();
}

AUniqueId& AUniqueId::operator=(AUniqueId&& Other) & noexcept
{
	if (this != &Other)
	{
		Release();
		Issuer = std::move(Other.Issuer);
		Id = std::move(Other.Id);
		Other.Release();
	}
	return *this;
}

AUniqueId::AUniqueId(FUniqueIdIssuer& Issuer, FUniqueId&& Issued)
	: Issuer{ Issuer }
	, Id{ std::move(Issued) }
{
}

bool AUniqueId::IsValid() const noexcept
{
	return Issuer.IsValid()
		&& Issuer->IsValid()
		&& Id.IsValid();
}

void AUniqueId::Swap(AUniqueId& Other) noexcept
{
	Issuer.Swap(Other.Issuer);
	Id.Swap(Other.Id);
}

void AUniqueId::Release() noexcept
{
	if (IsValid())
	{
		Issuer->Revoke(*this);
	}
	Issuer.Release();
}

std::atomic<FUniqueId::ArgumentType> FUniqueIdIssuer::IssuerIdCounter{ FUniqueId::InvalidArg };

FUniqueIdIssuer::FUniqueIdIssuer()
	: TObject<FUniqueIdIssuer>(*this)
	, Id(FUniqueId::InvalidArg)
	, Counter(FUniqueId::InvalidArg)
	, Revoked()
	, Mutex()
{
	if (IssuerIdCounter != std::numeric_limits<FUniqueId::ArgumentType>::max())
	{
		Id = ++IssuerIdCounter;
	}
}

AUniqueId FUniqueIdIssuer::Issue() noexcept
{
	FUniqueId::ArgumentType Arg{ FUniqueId::InvalidArg };

	std::unique_lock<std::mutex> Lock{ Mutex };
	if (!Revoked.empty())
	{
		Arg = *Revoked.cbegin();
		Revoked.erase(Arg);
	}
	else if (Counter != std::numeric_limits<FUniqueId::ArgumentType>::max())
	{
		Arg = ++Counter;
	}

	return AUniqueId{ *this, std::move(FUniqueId{ Arg, Evaluate(Arg) }) };
}

void FUniqueIdIssuer::Revoke(AUniqueId& Handle) noexcept
{
	if (Handle.IsValid()
		&& Handle.Issuer.GetAddress() == this
		&& Evaluate(Handle.Id.Arg) == Handle.Id.Value)
	{
		std::unique_lock<std::mutex> Lock{ Mutex };
		Revoked.emplace(Handle.Id.Arg);

		Handle.Id.Arg = FUniqueId::InvalidArg;
		Handle.Id.Value = FUniqueId::InvalidId;
	}
}
