// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "Type/String.h"
#include "Type/EventDispatcher.h"
#include "ScreenSpace.h"
#include "System/Input/KeyboardKey.h"
#include "System/Input/MouseButton.h"
#include "System/Input/InputTrigger.h"

class ISystemWindowProcedure;

#define EnumerateSystemWindowEvents(Enumerate)	\
	Enumerate(OnClosed)							\
	Enumerate(OnResized)						\
	Enumerate(OnKeyboardKey)					\
	Enumerate(OnMouseButton)					\
	Enumerate(OnMouseWheel)						\
	Enumerate(OnMouseMove)

#define EmplaceEventVariant(Event) F##Event,
#define DeclareEventDispatcher(Event) TEventDispatcher<bool(const SystemWindowEventTypes::F##Event&)> ##Event;

namespace SystemWindowEventTypes
{
	struct FOnClosed final {};
	struct FOnResized final { FScreenSize ClientAreaSize{}; };
	struct FOnKeyboardKey final { EKeyboardKey Key{}; ESwitchState State{}; };
	struct FOnMouseButton final { EMouseButton Button{}; ESwitchState State{}; };
	struct FOnMouseWheel final { std::vector<EMouseWheelTrigger> WheelDelta{}; };
	struct FOnMouseMove final { FScreenLocation CursorLocation{}; };

	struct Null final {};
	using FEvent = std::variant<EnumerateSystemWindowEvents(EmplaceEventVariant) Null>;
}

class FSystemWindow final
	: public TObject<FSystemWindow>
{
public:
	FSystemWindow(
		std::unique_ptr<ISystemWindowProcedure> WndProc,
		FStringView Title);
	~FSystemWindow() noexcept;

	FSystemWindow(const FSystemWindow&) = delete;
	FSystemWindow& operator=(const FSystemWindow&) & = delete;
	FSystemWindow(FSystemWindow&&) noexcept = delete;
	FSystemWindow& operator=(FSystemWindow&&) & noexcept = delete;

public:
	bool IsValid() const noexcept;
	inline FStringView GetTitle() const noexcept { return Title; }

	void Present() noexcept;
	void Close() noexcept;

	template<typename T>
	inline T& InterfaceAs() { return dynamic_cast<T&>(*WndProc); }

private:
	std::unique_ptr<ISystemWindowProcedure> WndProc;
	FString Title;

public:
	class FSystemWindowEvents final
	{
	public:
		FSystemWindowEvents();
		~FSystemWindowEvents() noexcept = default;

		FSystemWindowEvents(const FSystemWindowEvents&) = delete;
		FSystemWindowEvents& operator=(const FSystemWindowEvents&) & = delete;
		FSystemWindowEvents(FSystemWindowEvents&&) noexcept = delete;
		FSystemWindowEvents& operator=(FSystemWindowEvents&&) & noexcept = delete;

	public:
		void Enqueue(SystemWindowEventTypes::FEvent&& Event);
		void Process();

	private:
		std::array<std::queue<SystemWindowEventTypes::FEvent>, 2> Queues;
		std::size_t CurrentIndex;
		std::mutex Mutex;

	public:
		EnumerateSystemWindowEvents(DeclareEventDispatcher)
	}
	Events;
};

#undef EmplaceEventVariant
#undef DeclareEventDispatcher
