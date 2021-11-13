// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "WindowsPlatform.h"

#ifdef PLATFORM_WINDOWS

#include "WindowsPlatformWindowProcedure.h"

IPlatform& FWindowsPlatform::GetInterface() noexcept
{
	return GetSpecific();
}

FWindowsPlatform& FWindowsPlatform::GetSpecific() noexcept
{
	static FWindowsPlatform Instance{};
	return Instance;
}

void FWindowsPlatform::PrintDebugOutput(FStringView Message) const noexcept
{
	static std::mutex Mutex{};
	std::unique_lock<std::mutex> Lock{ Mutex };
	::OutputDebugStringW(reinterpret_cast<LPCWSTR>(Message.GetStr()));
	::OutputDebugStringW(TEXT("\n"));
}

std::unique_ptr<ISystemWindowProcedure> FWindowsPlatform::CreateWindowProcedure() const noexcept
{
	return std::make_unique<WindowsPlatform::FWndProc>();
}

std::optional<std::vector<char8_t>> FWindowsPlatform::StringToUTF8(const FString& Str) const
{
	auto RequiredBufferSize{ ::WideCharToMultiByte(
		CP_UTF8,
		0,
		reinterpret_cast<LPCWSTR>(Str.GetStr()),
		-1,
		nullptr,
		0,
		nullptr,
		nullptr) - 1 };

	std::vector<char8_t> Multibyte(RequiredBufferSize, u8'\0');
	const auto BytesWritten{ ::WideCharToMultiByte(
		CP_UTF8,
		0,
		reinterpret_cast<LPCWSTR>(Str.GetStr()),
		static_cast<int>(Str.Length<EStringLength::CodePoint>()),
		reinterpret_cast<LPCH>(Multibyte.data()),
		static_cast<int>(Multibyte.size()),
		nullptr,
		nullptr) };
	if (BytesWritten == 0)
	{
		return std::nullopt;
	}

	return std::vector<char8_t>{
		std::cbegin(Multibyte),
		std::find(
			std::cbegin(Multibyte),
			std::cend(Multibyte),
			u8'\0') };
}

std::optional<FString> FWindowsPlatform::UTF8ToString(const std::vector<char8_t>& u8Chars) const
{
	auto RequiredCharacterSize{ ::MultiByteToWideChar(
		CP_UTF8,
		DWORD{},
		reinterpret_cast<LPCCH>(u8Chars.data()),
		-1,
		nullptr,
		0) - 1 };

	std::u16string u16Chars(RequiredCharacterSize + std::size_t{ 1 }, u'\0');
	const auto CharactersWritten{ ::MultiByteToWideChar(
		CP_UTF8,
		DWORD{},
		reinterpret_cast<LPCCH>(u8Chars.data()),
		static_cast<int>(u8Chars.size()),
		reinterpret_cast<LPWSTR>(u16Chars.data()),
		static_cast<int>(u16Chars.size())) };
	if (CharactersWritten == 0)
	{
		return std::nullopt;
	}

	return FString(u16Chars.data(), u16Chars.find_first_of(u'\0'));
}

#endif
