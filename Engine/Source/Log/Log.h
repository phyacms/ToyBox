// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/String.h"

class FLog final
{
private:
	class FEndl final
	{
	public:
		static const FEndl& GetInstance();

	private:
		~FEndl() noexcept = default;
	};
	class FStream final
	{
		friend FLog;

	private:
		FStream() : Buffer{} {}
		FStream(FStream&&) noexcept = default;

	public:
		FStream(const FStream&) = delete;
		FStream& operator=(const FStream&) & = delete;
		FStream& operator=(FStream&&) & = delete;
		~FStream() noexcept = default;

		template<typename T>
		inline FStream operator<<(T&& Arg)&&
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
		void operator<<(const FEndl&) && noexcept;

	private:
		FString Buffer;
	};

public:
	static FLog& GetThreadLogger() noexcept;
	inline static const FEndl& GetEndl() noexcept { return FEndl::GetInstance(); }

private:
	FLog();
	~FLog() noexcept = default;

public:
	template<typename T, typename = std::enable_if_t<!std::is_same_v<std::remove_cvref_t<T>, FEndl>>>
	inline FStream operator<<(T&& Param) const { return FStream{} << std::forward<T>(Param); }

public:
	inline void SetIdentifier(const FString& Identifier) { this->Identifier = Identifier; }
	inline void SetIdentifier(FString&& Identifier) noexcept { this->Identifier = std::move(Identifier); }
	inline const FString& GetIdentifier() const noexcept { return Identifier; }

private:
	FString Identifier;
};

#define EnumerateLogLevel(Enumerate)	\
	Enumerate(Verbose)					\
	Enumerate(Log)						\
	Enumerate(Warning)					\
	Enumerate(Error)

#define DeclareLogLevel(Level)					\
struct FLogLevel_##Level final					\
{												\
private:										\
	~FLogLevel_##Level() noexcept = default;	\
												\
public:											\
	static constexpr							\
	const FString::CodeUnit*					\
	GetStr() noexcept { return USTR(#Level); }	\
};

EnumerateLogLevel(DeclareLogLevel)

#undef DeclareLogLevel
#undef EnumerateLogLevel

#define LogOutput(bOutput, Category, Level)				\
if constexpr (bOutput) {								\
	FLog::GetThreadLogger() << USTR("[")				\
	<< FLog::GetThreadLogger().GetIdentifier()			\
	<< USTR("][") << FLogCategory_##Category::GetStr()	\
	<< USTR("][") << FLogLevel_##Level::GetStr() << USTR("] ")

#ifndef NDEBUG
#define DEBUG_OUTPUT true
#else
#define DEBUG_OUTPUT false
#endif
#define PrintDebugLog(Category, Level)   LogOutput(DEBUG_OUTPUT, Category, Level)
#define PrintReleaseLog(Category, Level) LogOutput(true, Category, Level)

#define DebugVerbose(Category)   PrintDebugLog(Category, Verbose)
#define DebugLog(Category)       PrintDebugLog(Category, Log)
#define DebugWarning(Category)   PrintDebugLog(Category, Warning)
#define DebugError(Category)     PrintDebugLog(Category, Error)
#define ReleaseVerbose(Category) PrintReleaseLog(Category, Verbose)
#define ReleaseLog(Category)     PrintReleaseLog(Category, Log)
#define ReleaseWarning(Category) PrintReleaseLog(Category, Warning)
#define ReleaseError(Category)   PrintReleaseLog(Category, Error)

#define LogEndl FLog::GetEndl(); } int{} // @NOTE: Enforces semicolon at the end of statement.

// Macro to declare user-defined log categories.
#define DeclareLogCategory(Category)				\
struct FLogCategory_##Category final				\
{													\
private:											\
	~FLogCategory_##Category() noexcept = default;	\
													\
public:												\
	static constexpr								\
	const FString::CodeUnit*						\
	GetStr() noexcept { return USTR(#Category); }	\
};
