// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/String.h"

class FLogEndl final
{
public:
	static const FLogEndl& GetInstance();

private:
	~FLogEndl() noexcept = default;
};

class FLogStream final
{
	friend class FLog;

private:
	FLogStream() : Buffer{} {}
	FLogStream(FLogStream&&) noexcept = default;

public:
	FLogStream(const FLogStream&) = delete;
	FLogStream& operator=(const FLogStream&) & = delete;
	FLogStream& operator=(FLogStream&&) & = delete;
	virtual ~FLogStream() noexcept = default;

	template<typename T>
	inline FLogStream operator<<(T&& Arg) &&
	{
		if constexpr (std::is_same_v<std::remove_cvref_t<T>, FString>)
		{
			Buffer += std::forward<T>(Arg);
			return std::move(*this);
		}
		else if constexpr (std::is_constructible_v<FString, T>)
		{
			return std::move(*this) << FString{ std::forward<T>(Arg) };
		}
		else { static_assert(false); }
	}
	void operator<<(const FLogEndl& Endl) && noexcept;

private:
	FString Buffer;
};

class FLog final
{
public:
	static FLog& GetThreadLogger() noexcept;

private:
	FLog();
	~FLog() noexcept = default;

public:
	template<typename T, typename = std::enable_if_t<!std::is_same_v<std::remove_cvref_t<T>, FLogEndl>>>
	inline FLogStream operator<<(T&& Param) const { return FLogStream{} << std::forward<T>(Param); }

public:
	void SetIdentifier(FStringView Identifier);
	inline FStringView GetIdentifier() const noexcept { return Identifier; }

private:
	FString Identifier;
};
