// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/MulticastDelegate.h"

template<typename T>
class TEventDispatchPolicy;

template<typename R, typename... Ts>
class TEventDispatchPolicy<R(Ts...)> final
	: public IBroadcastPolicy<R(Ts...)>
{
private:
	static_assert(std::is_same_v<R, bool>);
	using typename IBroadcastPolicy<R(Ts...)>::FExecutors;

public:
	virtual R Broadcast(const FExecutors& Executors, Ts... Params) const override final
	{
		for (const auto& [Index, Delegate] : Executors)
		{
			if (Delegate.Execute(std::forward<Ts>(Params)...))
			{
				return true;
			}
		}
		return false;
	}
};

template<typename T>
using TEventDispatcher = TMulticastDelegate<T, TEventDispatchPolicy<T>>;
