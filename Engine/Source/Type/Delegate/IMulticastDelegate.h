// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "DelegateHandle.h"
#include "Type/Object/Object.h"

class IMulticastDelegate
	: public TObject<IMulticastDelegate>
{
public:
	IMulticastDelegate();
	virtual ~IMulticastDelegate() noexcept = default;

	IMulticastDelegate(const IMulticastDelegate&) = delete;
	IMulticastDelegate& operator=(const IMulticastDelegate&) & = delete;
	IMulticastDelegate(IMulticastDelegate&&) noexcept = delete;
	IMulticastDelegate& operator=(IMulticastDelegate&&) & noexcept = delete;

public:
	virtual void RemoveDynamic(ADelegateHandle& Handle) & noexcept = 0;

protected:
	ADelegateHandle Issue();
	std::size_t Revoke(ADelegateHandle& Handle) noexcept;

private:
	std::atomic<std::size_t> Counter;
	std::unordered_set<std::size_t> Revoked;
	std::mutex Mutex;
};
