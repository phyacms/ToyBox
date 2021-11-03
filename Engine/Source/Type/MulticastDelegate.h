// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Object.h"
#include "Delegate.h"

class ADelegateHandle;

class IMulticastDelegate
	: public TObject<IMulticastDelegate>
{
public:
	IMulticastDelegate();
	virtual ~IMulticastDelegate() noexcept = default;

	IMulticastDelegate(const IMulticastDelegate&) = delete;
	IMulticastDelegate& operator=(const IMulticastDelegate&) = delete;
	IMulticastDelegate(IMulticastDelegate&&) noexcept = delete;
	IMulticastDelegate& operator=(IMulticastDelegate&&) noexcept = delete;

public:
	virtual void RemoveDynamic(ADelegateHandle& Handle) noexcept = 0;

protected:
	ADelegateHandle Issue();
	std::size_t Revoke(ADelegateHandle& Handle) noexcept;

private:
	std::atomic<std::size_t> Counter;
	std::unordered_set<std::size_t> Revoked;
	std::mutex Mutex;
};

class ADelegateHandle final
{
	friend class IMulticastDelegate;

public:
	inline static constexpr auto InvalidHandle{ std::size_t{} };

public:
	ADelegateHandle() : Issuer{}, Index{ InvalidHandle } {}
	~ADelegateHandle() noexcept { Release(); }

	ADelegateHandle(const ADelegateHandle&) = delete;
	ADelegateHandle& operator=(const ADelegateHandle&) = delete;
	ADelegateHandle(ADelegateHandle&& Other) noexcept;
	ADelegateHandle& operator=(ADelegateHandle&& Other) noexcept;

	inline operator bool() const noexcept { return IsValid(); }

private:
	ADelegateHandle(IMulticastDelegate& Issuer, std::size_t Index) : Issuer{ Issuer }, Index{ Index } {}

public:
	inline bool IsValid() const noexcept { return Issuer.IsValid() && Index != InvalidHandle; }
	inline bool IsIssuedBy(IMulticastDelegate& Issuer) const noexcept { return this->Issuer.GetAddress() == &Issuer; }
	inline std::size_t GetIndex() const noexcept { return Index; }

	void Release() noexcept;

private:
	AObject<IMulticastDelegate> Issuer;
	std::size_t Index;
};

using FDelegateHandles = std::vector<ADelegateHandle>;

template<typename T>
using TDelegateExecutor = std::pair<std::size_t, TDelegate<T>>;

template<typename T>
using TDelegateExecutors = std::list<TDelegateExecutor<T>>;

template<typename T>
class IBroadcastPolicy;

template<typename R, typename... Ts>
class IBroadcastPolicy<R(Ts...)>
{
protected:
	using FExecutors = TDelegateExecutors<R(Ts...)>;

public:
	virtual R Broadcast(const FExecutors& Executors, Ts... Params) const = 0;
};

template<typename T, typename B = std::execution::sequenced_policy>
class TMulticastDelegate;

template<typename R, typename... Ts, typename BroadcastPolicyType>
class TMulticastDelegate<R(Ts...), BroadcastPolicyType> final
	: public IMulticastDelegate
{
private:
	inline static constexpr auto bIsStdPolicy{ std::is_execution_policy_v<BroadcastPolicyType> };
	inline static constexpr auto bIsCustomPolicy{ std::is_base_of_v<IBroadcastPolicy<R(Ts...)>, BroadcastPolicyType> };
	static_assert(bIsStdPolicy || bIsCustomPolicy);

private:
	using FExecutors = TDelegateExecutors<R(Ts...)>;

public:
	using CompatibleDelegateType = FExecutors::value_type::second_type;

public:
	explicit TMulticastDelegate(BroadcastPolicyType&& Broadcaster = BroadcastPolicyType{})
		: IMulticastDelegate()
		, Executors{}
		, Broadcaster{ std::move(Broadcaster) } {}
	virtual ~TMulticastDelegate() = default;

	inline operator bool() const noexcept { return IsBound(); }
	inline R operator()(Ts... Params) const { return Broadcast(std::forward<Ts>(Params...)); }

public:
	[[nodiscard]] ADelegateHandle AddDynamic(CompatibleDelegateType Delegate)
	{
		ADelegateHandle Issued{ Issue() };
		if (Issued.IsValid())
		{
			Executors.emplace_back(std::make_pair(Issued.GetIndex(), std::move(Delegate)));
		}
		return Issued;
	}
	inline virtual void RemoveDynamic(ADelegateHandle& Handle) noexcept override final
	{
		auto Index{ Revoke(Handle) };
		if (Index != ADelegateHandle::InvalidHandle)
		{
			Executors.remove_if([Index](const auto& Executor)->bool { return Executor.first == Index; });
		}
	}

	inline bool IsBound() const noexcept { return !Executors.empty(); }

	inline R Broadcast(Ts... Params) const
	{
		if constexpr (bIsStdPolicy)
		{
			std::for_each(
				Broadcaster,
				std::cbegin(Executors),
				std::cend(Executors),
				[&Params...](const auto& Executor)->void { Executor.second.Execute(std::forward<Ts>(Params)...); });
			return R();
		}
		else if constexpr (bIsCustomPolicy)
		{
			return Broadcaster.Broadcast(Executors, std::forward<Ts>(Params)...);
		}
		else { static_assert(false); }
	}

private:
	FExecutors Executors;
	BroadcastPolicyType Broadcaster;
};
