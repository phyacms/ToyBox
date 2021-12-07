// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Delegate.h"
#include "Type/Object/Object.h"

class ADelegateHandle;
class IMulticastDelegate;

class ADelegateHandle final
{
	friend IMulticastDelegate;

public:
	inline static constexpr std::size_t InvalidHandle{};

public:
	ADelegateHandle() : Issuer{}, Index{ InvalidHandle } {}
	~ADelegateHandle() noexcept { Release(); }

	ADelegateHandle(const ADelegateHandle&) = delete;
	ADelegateHandle& operator=(const ADelegateHandle&) & = delete;
	ADelegateHandle(ADelegateHandle&& Other) noexcept;
	ADelegateHandle& operator=(ADelegateHandle&& Other) & noexcept;

private:
	ADelegateHandle(IMulticastDelegate& Issuer, std::size_t Index);

public:
	inline operator bool() const noexcept { return IsValid(); }

public:
	inline bool IsValid() const noexcept { return Issuer.IsValid() && Index != InvalidHandle; }
	inline bool IsIssuedBy(const IMulticastDelegate& Issuer) const noexcept { return this->Issuer.GetAddress() == &Issuer; }
	inline std::size_t GetIndex() const noexcept { return Issuer.IsValid() ? Index : InvalidHandle; }

	void Release() noexcept;

private:
	TObjRef<IMulticastDelegate> Issuer;
	std::size_t Index;
};

class FDelegateHandles final
{
public:
	FDelegateHandles() : Handles{} {}
	~FDelegateHandles() noexcept = default;

	FDelegateHandles(FDelegateHandles&&) noexcept = default;
	FDelegateHandles& operator=(FDelegateHandles&&) & noexcept = default;
	FDelegateHandles(const FDelegateHandles&) = delete;
	FDelegateHandles& operator=(const FDelegateHandles&) & = delete;

	FDelegateHandles& operator+=(ADelegateHandle&& Handle) &;

public:
	inline void Clear() noexcept { Handles.clear(); }

private:
	std::vector<ADelegateHandle> Handles;
};
