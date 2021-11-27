// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Time.h"
#include "Type/Delegate/EventDispatcher.h"
#include "System/Input/Code/KeyboardKey.h"
#include "System/Input/Code/MouseButton.h"
#include "System/Input/Code/MouseWheel.h"
#include "ScreenSpace.h"

#define EnumerateSystemWindowEvents(Enumerate)	\
	Enumerate(OnClosed)							\
	Enumerate(OnResized)						\
	Enumerate(OnKeyboardKey)					\
	Enumerate(OnMouseButton)					\
	Enumerate(OnMouseWheel)						\
	Enumerate(OnMouseMove)

#define EmplaceEventVariant(Event) F##Event,
#define DeclareEventDispatcher(Event) TEventDispatcher<bool(const F##Event&)> ##Event;

struct FOnClosed final {};
struct FOnResized final { FScreenSize ClientAreaSize{}; };
struct FOnKeyboardKey final : public FTimeStamp { EKeyboardKey Key{}; ESwitchState State{}; };
struct FOnMouseButton final : public FTimeStamp { EMouseButton Button{}; ESwitchState State{}; };
struct FOnMouseWheel final : public FTimeStamp { FMouseWheelDelta WheelDelta{}; };
struct FOnMouseMove final : public FTimeStamp { FScreenLocation CursorLocation{}; };

namespace SystemWindowEventHelp { struct Null final {}; }
using FSystemWindowEvent = std::variant<EnumerateSystemWindowEvents(EmplaceEventVariant) SystemWindowEventHelp::Null>;

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
	void Enqueue(FSystemWindowEvent&& Event);
	void Process();

private:
	std::array<std::queue<FSystemWindowEvent>, 2> Queues;
	std::size_t CurrentIndex;
	std::mutex Mutex;

public:
	EnumerateSystemWindowEvents(DeclareEventDispatcher)
};

#undef EmplaceEventVariant
#undef DeclareEventDispatcher
