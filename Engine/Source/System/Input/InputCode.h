// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "KeyboardKey.h"
#include "MouseButton.h"

struct FInputCode final
{
public:
	static constexpr auto InvalidValue{ std::size_t{} };
	static constexpr auto CodeLimit{ KeyboardFunctions::KeyCount + MouseFunctions::ButtonCount };
	static_assert(CodeLimit < std::numeric_limits<std::size_t>::max());

public:
	FInputCode() : Value{ InvalidValue } {}
	FInputCode(EKeyboardKey Key);
	FInputCode(EMouseButton Button);
	FInputCode(const FInputCode&) = default;
	FInputCode& operator=(const FInputCode&) & = default;
	FInputCode(FInputCode&&) noexcept = default;
	FInputCode& operator=(FInputCode&&) & noexcept = default;
	~FInputCode() noexcept = default;

	friend bool operator==(const FInputCode&, const FInputCode&) noexcept = default;
	friend bool operator!=(const FInputCode&, const FInputCode&) noexcept = default;

	inline operator bool() const noexcept { return IsValid(); }
	inline operator std::size_t() const noexcept { return GetValue(); }
	operator EKeyboardKey() const noexcept;
	operator EMouseButton() const noexcept;

public:
	bool IsValid() const noexcept;
	bool IsKeyboardKey() const noexcept;
	bool IsMouseButton() const noexcept;
	inline std::size_t GetValue() const noexcept { return Value; }

private:
	std::size_t Value;
};

template<>
struct std::hash<FInputCode> final
{
	inline std::size_t operator()(const FInputCode& InputCode) const noexcept { return static_cast<std::size_t>(InputCode); }
};

struct FInputCodes final
{
	using ContainerType = std::unordered_set<FInputCode>;

	ContainerType Values{};

	friend bool operator==(const FInputCodes&, const FInputCodes&) = default;
	friend bool operator!=(const FInputCodes&, const FInputCodes&) = default;
	inline FInputCodes& operator+=(const FInputCode& InputCode) noexcept { Values.emplace(InputCode); return *this; }
	inline FInputCodes& operator-=(const FInputCode& InputCode) noexcept { Values.erase(InputCode); return *this; }
};
