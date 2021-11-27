// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Delegate.h"
#include "IMulticastDelegate.h"

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
	template<typename... Ts>
	inline [[nodiscard]] ADelegateHandle AddDynamic(Ts&&... Params) &
	{
		ADelegateHandle Issued{ Issue() };
		if (Issued.IsValid())
		{
			Executors.emplace_back(
				std::make_pair(
					Issued.GetIndex(),
					CompatibleDelegateType{ std::forward<Ts>(Params)... }));
		}
		return Issued;
	}
	inline virtual void RemoveDynamic(ADelegateHandle& Handle) & noexcept override final
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
