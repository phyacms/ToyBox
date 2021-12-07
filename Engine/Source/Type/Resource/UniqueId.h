// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object/Object.h"

class FUniqueIdIssuer;

class FUniqueId final
{
	friend FUniqueIdIssuer;

public:
	using ArgumentType = std::uint32_t;
	using ValueType = std::uint64_t;

	static constexpr ArgumentType InvalidArg{};
	static constexpr ValueType InvalidId{};

	// Insists that std::size range implies ValueType range.
	static_assert(
		std::numeric_limits<ValueType>::min() >= std::numeric_limits<std::size_t>::min() &&
		std::numeric_limits<ValueType>::max() <= std::numeric_limits<std::size_t>::max());

public:
	FUniqueId() : FUniqueId{ InvalidArg, InvalidId } {}
	~FUniqueId() noexcept = default;

	FUniqueId(const FUniqueId&) = default;
	FUniqueId& operator=(const FUniqueId&) & = default;
	FUniqueId(FUniqueId&& Other) noexcept;
	FUniqueId& operator=(FUniqueId&& Other) & noexcept;

private:
	FUniqueId(ArgumentType Arg, ValueType Value) : Arg{ Arg }, Value{ Value } {}

public:
	inline operator bool() const noexcept { return IsValid(); }
	inline operator std::size_t() const noexcept { return GetHash(); }

public:
	inline bool IsValid() const noexcept { return Value != InvalidId; }
	inline std::size_t GetHash() const noexcept { return static_cast<std::size_t>(Value); }

	void Swap(FUniqueId& Other) noexcept;

private:
	ArgumentType Arg;
	ValueType Value;
};

class AUniqueId final
{
	friend FUniqueIdIssuer;

public:
	AUniqueId() : Issuer{}, Id{} {}
	~AUniqueId() noexcept { Release(); }

	AUniqueId(const AUniqueId&) = delete;
	AUniqueId& operator=(const AUniqueId&) & = delete;
	AUniqueId(AUniqueId&& Other) noexcept;
	AUniqueId& operator=(AUniqueId&& Other) & noexcept;

private:
	AUniqueId(FUniqueIdIssuer& Issuer, FUniqueId&& Issued);

public:
	inline operator bool() const noexcept { return IsValid(); }
	inline operator std::size_t() const noexcept { return GetHash(); }

public:
	bool IsValid() const noexcept;
	inline std::size_t GetHash() const noexcept { return Id.GetHash(); }

	void Swap(AUniqueId& Other) noexcept;
	void Release() noexcept;

private:
	TObjRef<FUniqueIdIssuer> Issuer;
	FUniqueId Id;
};

template<>
struct std::hash<AUniqueId> final
{
	inline std::size_t operator()(const AUniqueId& Handle) const noexcept { return Handle.GetHash(); }
};

class FUniqueIdIssuer final
	: public TObject<FUniqueIdIssuer>
{
private:
	static std::atomic<FUniqueId::ArgumentType> IssuerIdCounter;

public:
	FUniqueIdIssuer();
	virtual ~FUniqueIdIssuer() noexcept = default;

	FUniqueIdIssuer(const FUniqueIdIssuer&) = delete;
	FUniqueIdIssuer& operator=(const FUniqueIdIssuer&) & = delete;
	FUniqueIdIssuer(FUniqueIdIssuer&&) noexcept = delete;
	FUniqueIdIssuer& operator=(FUniqueIdIssuer&&) & noexcept = delete;

public:
	inline bool IsValid() const noexcept { return Id != FUniqueId::InvalidArg; }
	inline bool IsAvailable() const noexcept { return Counter != std::numeric_limits<FUniqueId::ArgumentType>::max(); }

	[[nodiscard]] AUniqueId Issue() noexcept;
	void Revoke(AUniqueId& Handle) noexcept;

private:
	inline FUniqueId::ValueType Evaluate(FUniqueId::ArgumentType Arg) const noexcept { return Evaluate(Id, Arg); }
	static constexpr FUniqueId::ValueType Evaluate(FUniqueId::ArgumentType Arg1, FUniqueId::ArgumentType Arg2) noexcept
	{
		if (Arg1 == FUniqueId::InvalidArg || Arg2 == FUniqueId::InvalidArg)
		{
			return FUniqueId::InvalidId;
		}
		else if (Arg1 < Arg2)
		{
			return Evaluate(Arg2, Arg1) + 1;
		}
		else
		{
			auto Arg1_64 = static_cast<std::uint64_t>(Arg1);
			return Arg1_64 * Arg1_64 - 2 * (Arg1_64 - Arg2);
		}
	}

private:
	FUniqueId::ArgumentType Id;
	std::atomic<FUniqueId::ArgumentType> Counter;
	std::unordered_set<FUniqueId::ArgumentType> Revoked;
	std::mutex Mutex;
};
