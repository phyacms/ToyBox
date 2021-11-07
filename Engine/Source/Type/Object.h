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
class TObject
{
protected:
	explicit TObject(T& Object)
		: Redirector{ std::make_shared<TObjectPtr<T>>(
			TObjectPtr<T>{ .Ptr = &Object }) } {}
	virtual ~TObject() noexcept = default;

	TObject(const TObject&) = delete;
	TObject& operator=(const TObject&) & = delete;
	TObject(TObject&&) noexcept = default;
	TObject& operator=(TObject&&) & noexcept = default;

private:
	friend AObject<T>::AObject(TObject&);
	std::shared_ptr<TObjectPtr<T>> Redirector;
};
