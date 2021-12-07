// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

template<typename T>
struct TRawPtr final { T* Ptr{}; };

template<typename T>
class TObject;

template<typename T>
class TObjRef final
{
public:
	TObjRef(std::nullptr_t = nullptr) : WeakPtr{} {}
	TObjRef(std::weak_ptr<TRawPtr<T>> WeakPtr) : WeakPtr{ std::move(WeakPtr) } {}
	~TObjRef() noexcept { Release(); }

	TObjRef(const TObjRef&) = default;
	TObjRef& operator=(const TObjRef&) & = default;
	TObjRef(TObjRef&&) noexcept = default;
	TObjRef& operator=(TObjRef&&) & noexcept = default;

	inline operator bool() const noexcept { return IsValid(); }
	inline T* operator->() const noexcept { return GetAddress(); }

public:
	inline bool IsValid() const noexcept { return !WeakPtr.expired(); }
	inline T* GetAddress() const noexcept { return IsValid() ? WeakPtr.lock()->Ptr : nullptr; }

	inline void Swap(TObjRef& Other) noexcept { std::swap(WeakPtr, Other.WeakPtr); }
	inline void Release() noexcept { WeakPtr.reset(); }

private:
	std::weak_ptr<TRawPtr<T>> WeakPtr;
};

template<typename T>
inline bool operator==(const TObjRef<T>& Lhs, const TObjRef<T>& Rhs) noexcept { return Lhs.GetAddress() == Rhs.GetAddress(); }
template<typename T>
inline bool operator!=(const TObjRef<T>& Lhs, const TObjRef<T>& Rhs) noexcept { return Lhs.GetAddress() != Rhs.GetAddress(); }

template<typename T>
class TConstObjRef final
{
public:
	TConstObjRef(std::nullptr_t = nullptr) : Object{} {}
	TConstObjRef(const T& Object) : Object{ const_cast<T&>(Object) } {}
	TConstObjRef(TObjRef<T> Object) : Object{ std::move(Object) } {}
	~TConstObjRef() noexcept = default;

	TConstObjRef(const TConstObjRef&) = default;
	TConstObjRef& operator=(const TConstObjRef&) & = default;
	TConstObjRef(TConstObjRef&&) noexcept = default;
	TConstObjRef& operator=(TConstObjRef&&) & noexcept = default;

	friend inline bool operator==(const TConstObjRef& Lhs, const TConstObjRef& Rhs) noexcept = default;
	friend inline bool operator!=(const TConstObjRef& Lhs, const TConstObjRef& Rhs) noexcept = default;

	inline operator bool() const noexcept { return IsValid(); }
	inline const T* operator->() const noexcept { return GetAddress(); }

public:
	inline bool IsValid() const noexcept { return Object.IsValid(); }
	inline const T* GetAddress() const noexcept { return Object.GetAddress(); }

	inline void Swap(TConstObjRef& Other) noexcept { Object.Swap(Other.Object); }
	inline void Release() noexcept { Object.Release(); }

private:
	TObjRef<T> Object;
};
