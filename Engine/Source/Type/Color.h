// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Byte.h"
#include "TypeTraits.h"

namespace sRGB
{
	using uint32 = std::uint32_t;
	using byte4 = std::array<Byte, 4>;
	struct normal4 final { std::array<float, 4> Normals; };
	using ColorValue = std::variant<uint32, byte4, normal4>;
	enum class Format : std::size_t { ARGB, RGBA, BGRA, };

	struct ARGB final { ColorValue Value; };
	struct RGBA final { ColorValue Value; };
	struct BGRA final { ColorValue Value; };
	using ColorVar = std::variant<RGBA, ARGB, BGRA>;

	namespace Color
	{
		inline constexpr ARGB White{ 0xFFFFFFFF };
		inline constexpr ARGB Black{ 0xFF000000 };

		inline constexpr ARGB Red{ 0xFFFF0000 };
		inline constexpr ARGB Green{ 0xFF00FF00 };
		inline constexpr ARGB Blue{ 0xFF0000FF };

		inline constexpr ARGB Cyan{ 0xFF00FFFF };
		inline constexpr ARGB Magenta{ 0xFFFF00FF };
		inline constexpr ARGB Yellow{ 0xFFFFFF00 };

		inline constexpr ARGB CornflowerBlue{ 0xFF6495ED };
	}
}

namespace TypeTraits
{
	template<typename T>
	struct IsColorValue : public std::conditional_t<
		TypeTraits::bIsAnyTypeOf<T, sRGB::uint32, sRGB::byte4, sRGB::normal4>,
		std::true_type,
		std::false_type> {};
	template<typename T>
	inline constexpr auto bIsColorValue{ IsColorValue<T>::value };

	template<typename T>
	struct IsColorFormat : public std::conditional_t<
		TypeTraits::bIsAnyTypeOf<T, sRGB::ARGB, sRGB::RGBA, sRGB::BGRA>,
		std::true_type,
		std::false_type> {};
	template<typename T>
	inline constexpr auto bIsColorFormat{ IsColorFormat<T>::value };
}

namespace sRGB
{
#define IsFromTo(S, T) (std::is_same_v<SrcType, S> && std::is_same_v<DestType, T>)

	template<
		typename To,
		typename From,
		typename = std::enable_if_t<
		TypeTraits::bIsColorValue<std::remove_cv_t<To>>&&
		TypeTraits::bIsColorValue<std::remove_cvref_t<From>>>>
		inline To ConvertValue(const From& Value)
	{
		using DestType = std::remove_cvref_t<To>;
		using SrcType = std::remove_cvref_t<From>;
		if constexpr (std::is_same_v<SrcType, DestType>)
		{
			return Value;
		}

		static constexpr auto Normalize{
			[](byte4 Bytes)->normal4
			{
				normal4 Normalized{};
				std::transform(
					std::execution::par_unseq,
					std::cbegin(Bytes),
					std::cend(Bytes),
					std::begin(Normalized.Normals),
					[](Byte Value)->float { return static_cast<float>(Value) / 255.0f; });
				return Normalized;
			} };
		static constexpr auto Denormalize{
			[](const normal4& Normalized)->byte4
			{
				byte4 Denormalized{};
				std::transform(
					std::execution::par_unseq,
					std::cbegin(Normalized.Normals),
					std::cend(Normalized.Normals),
					std::begin(Denormalized),
					[](float Normal)->Byte { return static_cast<Byte>(std::round(Normal * 255.0f)); });
				return Denormalized;
			} };

		static constexpr auto UintToBytes{ &ToByteArray<uint32, std::endian::big> };
		static constexpr auto BytesToUint{
			[](const byte4& Bytes)->uint32 {
				return FromByteArray<uint32, std::endian::big>(Bytes.data()); } };

		if constexpr (false) {}
		else if constexpr (IsFromTo(uint32, byte4)) { return UintToBytes(Value); }
		else if constexpr (IsFromTo(uint32, normal4)) { return Normalize(UintToBytes(Value)); }
		else if constexpr (IsFromTo(byte4, uint32)) { return BytesToUint(Value); }
		else if constexpr (IsFromTo(byte4, normal4)) { return Normalize(Value); }
		else if constexpr (IsFromTo(normal4, uint32)) { return BytesToUint(Denormalize(Value)); }
		else if constexpr (IsFromTo(normal4, byte4)) { return Denormalize(Value); }
		else { static_assert(false); }
	}

	template<
		typename To,
		typename From,
		typename = std::enable_if_t<
		TypeTraits::bIsColorFormat<std::remove_cv_t<To>>&&
		TypeTraits::bIsColorFormat<std::remove_cvref_t<From>>>>
		inline To ConvertFormat(const From& Var)
	{
		using DestType = std::remove_cvref_t<To>;
		using SrcType = std::remove_cvref_t<From>;
		if constexpr (std::is_same_v<SrcType, DestType>)
		{
			return Var;
		}

		static constexpr auto Reverse{ [](auto& Container)->void { std::reverse(std::execution::par_unseq, std::begin(Container), std::end(Container)); } };
		static constexpr auto RotateLeft{ [](auto& Container)->void { std::rotate(std::rbegin(Container), std::rbegin(Container) + 1, std::rend(Container)); } };
		static constexpr auto RotateRight{ [](auto& Container)->void { std::rotate(std::begin(Container), std::begin(Container) + 1, std::end(Container)); } };

		static constexpr auto Manipulate{
			[](ColorValue Value, auto& Callable)->ColorValue
			{
				std::visit(stdhelp::overloaded{
					[&Callable](uint32& ColorCode)->void
					{
						struct FContainer final
						{
							Byte* pBytes{};
							inline Byte* begin() noexcept { return pBytes; }
							inline Byte* end() noexcept { return pBytes + sizeof(uint32) / sizeof(Byte); }
							inline decltype(auto) rbegin() noexcept { return std::reverse_iterator<Byte*>(end()); }
							inline decltype(auto) rend() noexcept { return std::reverse_iterator<Byte*>(begin()); }
						};
						FContainer Bytes{ reinterpret_cast<Byte*>(&ColorCode) };
						std::invoke(Callable, Bytes);
					},
					[&Callable](byte4& Bytes)->void { std::invoke(Callable, Bytes); },
					[&Callable](normal4& Normalized)->void { std::invoke(Callable, Normalized.Normals); } },
					Value);
				return Value;
			} };

		if constexpr (false) {}
		else if constexpr (IsFromTo(ARGB, BGRA) || IsFromTo(BGRA, ARGB)) { return DestType{ .Value = Manipulate(Var.Value, Reverse) }; }
		else if constexpr (IsFromTo(ARGB, RGBA)) { return DestType{ .Value = Manipulate(Var.Value, RotateLeft) }; }
		else if constexpr (IsFromTo(RGBA, ARGB)) { return DestType{ .Value = Manipulate(Var.Value, RotateRight) }; }
		else if constexpr (IsFromTo(RGBA, BGRA)) { return DestType{ .Value = Manipulate(Manipulate(Var.Value, RotateRight), Reverse) }; }
		else if constexpr (IsFromTo(BGRA, RGBA)) { return DestType{ .Value = Manipulate(Manipulate(Var.Value, Reverse), RotateLeft) }; }
		else { static_assert(false); }
	}

#undef IsFromTo
}

class FColor final
{
public:
	FColor() : FColor(sRGB::Color::White) {};
	FColor(const FColor&) = default;
	FColor(FColor&&) noexcept = default;
	FColor(const sRGB::ColorVar& Var) : Var{ Var } {}
	FColor(const sRGB::ColorValue& Value, sRGB::Format Format) : FColor()
	{
		switch (Format)
		{
			case sRGB::Format::ARGB: Var = sRGB::ARGB{ Value }; break;
			case sRGB::Format::RGBA: Var = sRGB::RGBA{ Value }; break;
			case sRGB::Format::BGRA: Var = sRGB::BGRA{ Value }; break;
			default:
				break;
		}
	}
	~FColor() noexcept = default;

	FColor& operator=(const FColor&) & = default;
	FColor& operator=(FColor&&) &noexcept = default;
	template<typename... Ts>
	inline FColor& operator=(Ts&&... Params) { Set(std::forward<Ts>(Params)...); return *this; }

public:
	inline void Set(const FColor& Other) { if (this != &Other) { Set(Other.Var); } }
	inline void Set(FColor&& Other) noexcept { if (this != &Other) { Set(Other); } }
	template<typename... Ts>
	inline void Set(Ts&&... Params) { operator=(FColor{ std::forward<Ts>(Params)... }); }

	template<typename T>
	inline T GetAs() const
	{
		return std::visit(stdhelp::overloaded{
			[](const T& Var)->T { return Var; },
			[](const auto& Var)->T { return sRGB::ConvertFormat<T>(Var); }, },
			Var);
	}
	template<typename T, sRGB::Format Format>
	inline T GetAs() const
	{
		using VarType =
			std::conditional_t<Format == sRGB::Format::ARGB, sRGB::ARGB,
			std::conditional_t<Format == sRGB::Format::RGBA, sRGB::RGBA,
			std::conditional_t<Format == sRGB::Format::BGRA, sRGB::BGRA, void>>>;
		static_assert(!std::is_same_v<VarType, void>);

		return std::visit(stdhelp::overloaded{
			[](T&& Value)->T { return std::move(Value); },
			[](auto&& Value)->T { return sRGB::ConvertValue<T>(std::move(Value)); }, },
			GetAs<VarType>().Value);
	}

private:
	sRGB::ColorVar Var;
};
