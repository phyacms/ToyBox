// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

template<typename T>
class TDelegate;

template<typename R, typename... Ts>
class TDelegate<R(Ts...)> final
{
public:
	TDelegate() : Stub{} {}
	template<typename... Parameters>
	TDelegate(Parameters&&... Params) : Stub{} { Bind(std::forward<Parameters>(Params)...); }
	~TDelegate() noexcept { Unbind(); }

	TDelegate(const TDelegate&) = default;
	TDelegate& operator=(const TDelegate&) = default;
	TDelegate(TDelegate&&) noexcept = default;
	TDelegate& operator=(TDelegate&&) noexcept = default;

	inline operator bool() const noexcept { return IsBound(); }
	inline R operator()(Ts... Params) const { return Execute(std::forward<Ts>(Params...)); }

public:
	template<
		typename T,
		typename = std::enable_if_t<!std::is_same_v<T, std::nullptr_t>>>
	inline void Bind(T&& Callable) noexcept { Stub = std::forward<T>(Callable); }
	template<
		typename T,
		typename MemFnType,
		typename = std::enable_if_t<!std::is_reference_v<T>>>
	inline void Bind(T& ObjRef, MemFnType MemFn) noexcept { Stub = std::bind(MemFn, ObjRef); }
	inline void Unbind() noexcept { Stub = nullptr; }

	inline bool IsBound() const noexcept { return static_cast<bool>(Stub); }

	inline R Execute(Ts... Params) const { return Stub(std::forward<Ts>(Params)...); }
	inline R ExecuteIfBound(Ts... Params) const { return IsBound() ? Stub(std::forward<Ts>(Params)...) : R(); }

private:
	std::function<R(Ts...)> Stub;
};
