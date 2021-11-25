// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

template<typename T>
struct TObjectPtr final { T* Ptr{}; };

template<typename T>
class TObject;

template<typename T>
class AObject final
{
public:
	AObject() : Redirector{} {}
	AObject(TObject<T>& Reflector) : Redirector{ Reflector.Redirector } {}
	~AObject() noexcept { Release(); }

	AObject(const AObject&) = default;
	AObject& operator=(const AObject&) & = default;
	AObject(AObject&&) noexcept = default;
	AObject& operator=(AObject&&) & noexcept = default;

	inline operator bool() const noexcept { return IsValid(); }
	inline T* operator->() const noexcept { return GetAddress(); }

public:
	inline bool IsValid() const noexcept { return !Redirector.expired(); }
	inline T* GetAddress() const noexcept { return IsValid() ? Redirector.lock()->Ptr : nullptr; }

	inline void Release() noexcept { Redirector.reset(); }

private:
	std::weak_ptr<TObjectPtr<T>> Redirector;
};

template<typename T>
class AConstObject final
{
public:
	AConstObject() : Object{} {}
	AConstObject(const T& Object) : Object{ const_cast<T&>(Object) } {}
	~AConstObject() noexcept = default;

	AConstObject(const AConstObject&) = default;
	AConstObject& operator=(const AConstObject&) & = default;
	AConstObject(AConstObject&&) noexcept = default;
	AConstObject& operator=(AConstObject&&) & noexcept = default;

	inline operator bool() const noexcept { return IsValid(); }
	inline const T* operator->() const noexcept { return GetAddress(); }

public:
	inline bool IsValid() const noexcept { return Object.IsValid(); }
	inline const T* GetAddress() const noexcept { return Object.GetAddress(); }

	inline void Release() noexcept { Object.Release(); }

private:
	AObject<T> Object;
};

template<typename T>
class TObject
{
protected:
	explicit TObject(T& Object) : Redirector{ std::make_shared<TObjectPtr<T>>(&Object) } {}
	virtual ~TObject() noexcept = default;

	TObject(const TObject&) = delete;
	TObject& operator=(const TObject&) & {}
	TObject(TObject&&) noexcept = default;
	TObject& operator=(TObject&&) & noexcept = default;

protected:
	inline void RedirectObjectPtr(T& Object) { Redirector->Ptr = &Object; }

private:
	friend AObject<T>::AObject(TObject&);
	std::shared_ptr<TObjectPtr<T>> Redirector;
};
