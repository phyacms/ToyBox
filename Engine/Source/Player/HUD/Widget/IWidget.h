// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Time.h"
#include "Type/String.h"
#include "Type/Object/Object.h"
#include "Type/Resource/UniqueId.h"
#include "Type/ScreenSpace/Dim.h"

class FHUD;
class IWidget;
template<typename T>
class AWidget;
class IGraphicsContext;

class IWidget
	: public TObject<IWidget>
{
	friend FHUD;

private:
	struct FChildren final
	{
		FUniqueIdIssuer IdIssuer{};
		std::unordered_map<std::size_t, std::unique_ptr<IWidget>> Widgets{};

		~FChildren() noexcept { Clear(); }

		inline bool IsEmpty() const noexcept { return Widgets.empty(); }
		bool Add(std::unique_ptr<IWidget>&& Widget);
		void Remove(IWidget& Widget) noexcept;
		void Clear() noexcept;
	};

private:
	IWidget(
		IWidget* Parent,
		AUniqueId&& UniqueId,
		UDim Dimension,
		bool bVisible);

	// Special constructor for widget root class, instantiated by HUD.
	IWidget() : IWidget(nullptr, {}, UDim::Default, true) {}

protected:
	// Constructor for derived classes.
	IWidget(
		IWidget& Parent, AUniqueId&& UniqueId, UDim Dimension, bool bVisible)
		: IWidget(&Parent, std::move(UniqueId), std::move(Dimension), bVisible) {}

public:
	virtual ~IWidget() noexcept;

	IWidget(const IWidget&) = delete;
	IWidget& operator=(const IWidget&) & = delete;
	IWidget(IWidget&&) noexcept = delete;
	IWidget& operator=(IWidget&&) & noexcept = delete;

public:
	bool IsValid() const noexcept;

	inline std::size_t GetIndex() const noexcept { return UniqueId.GetHash(); }
	inline FString GetName() const noexcept { return GetWidgetName() + USTR("_") + ::ToString(GetIndex()); }

	inline bool IsRoot() const noexcept { return Parent == nullptr; }
	inline IWidget& GetRoot() noexcept { return !IsRoot() ? Parent->GetRoot() : *this; }
	inline const IWidget& GetRoot() const noexcept { return !IsRoot() ? Parent->GetRoot() : *this; }
	inline bool IsLeaf() const noexcept { return Children.IsEmpty(); }

	inline TObjRef<IWidget> GetParent() const noexcept { return !IsRoot() ? *Parent : TObjRef<IWidget>{}; }

	// @NOTE: Note that [[nodiscard]] attribute is NOT enforced here.
	template<
		typename T,
		typename... Ts,
		typename = std::enable_if_t<std::is_base_of_v<IWidget, T>>>
	inline AWidget<T> CreateChild(Ts&&... Params)
	{
		auto Created{ std::make_unique<T>(
			*this,
			Children.IdIssuer.Issue(),
			std::forward<Ts>(Params)...) };
		if (Created != nullptr && Created->IsValid())
		{
			AWidget<T> Widget{ *Created };
			if (Children.Add(std::move(Created)))
			{
				return Widget;
			}
		}
		return {};
	}
	inline void RemoveChild(IWidget& Child) noexcept { Children.Remove(Child); }
	inline void RemoveChildren() noexcept { Children.Clear(); }

	inline void SetVisibility(bool bVisible) noexcept { this->bVisible = bVisible; }
	inline void ToggleVisibility() noexcept { SetVisibility(!bVisible); }
	inline bool IsVisible() const noexcept { return bVisible; }

	void CalcAbsoluteArea(const FScreenArea& Base);

	void Render(IGraphicsContext& Context, FTimeDuration DeltaTime);

private:
	virtual const FString& GetWidgetName() const noexcept = 0;
	virtual bool IsValidImpl() const noexcept = 0;
	virtual void RenderImpl(
		IGraphicsContext& Context,
		const FScreenArea& Base,
		FTimeDuration DeltaTime) = 0;

private:
	AUniqueId UniqueId;
	IWidget* Parent;
	FChildren Children;
	FDim Dimension;
	bool bVisible;
};

template<typename T>
class AWidget final
{
	static_assert(std::is_base_of_v<IWidget, T>);
	friend IWidget;

public:
	AWidget() : Object{}, Ptr{} {}

private:
	AWidget(T& Widget)
		: Object{ Widget }
		, Ptr{ &Widget } {}

public:
	inline operator bool() const noexcept { return IsValid(); }
	inline T* operator->() const noexcept { return GetAddress(); }

public:
	inline bool IsValid() const noexcept { return Object.IsValid(); }
	inline T* GetAddress() const noexcept { return IsValid() ? Ptr : nullptr; }

	inline void Release() noexcept { Object.Release(); }
	inline void Destroy() noexcept
	{
		if (IsValid() && !Ptr->IsRoot())
		{
			Ptr->GetParent()->RemoveChild(*Ptr);
		}
	}

private:
	TObjRef<IWidget> Object;
	T* Ptr;
};
