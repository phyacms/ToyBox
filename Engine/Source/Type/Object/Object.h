// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "ObjRef.h"

template<typename T>
class TObject
{
protected:
	explicit TObject(T& Object) : Redirector{ std::make_shared<TRawPtr<T>>(&Object) } {}
	virtual ~TObject() noexcept = default;

	TObject(const TObject&) = delete;
	TObject& operator=(const TObject&) & {}
	TObject(TObject&&) noexcept = default;
	TObject& operator=(TObject&&) & noexcept = default;

public:
	inline operator TObjRef<T>() const noexcept { return TObjRef<T>{ Redirector }; }
	inline operator TConstObjRef<T>() const noexcept { return static_cast<TObjRef<T>>(*this); }

protected:
	inline void RedirectObjectPtr(T& Object) { Redirector->Ptr = &Object; }

private:
	std::shared_ptr<TRawPtr<T>> Redirector;
};
