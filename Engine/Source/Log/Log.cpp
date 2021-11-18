// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Log.h"
#include "Platform/Platform.h"

const FLog::FEndl& FLog::FEndl::GetInstance()
{
	static FLog::FEndl Instance{};
	return Instance;
}

void FLog::FStream::operator<<(const FEndl&) && noexcept
{
	PlatformFunctions::PrintDebugOutput(std::move(Buffer));
}

FLog& FLog::GetThreadLogger() noexcept
{
	thread_local FLog Instance{};
	return Instance;
}

FLog::FLog()
	: Identifier{ ::ToString(
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		TypeConversion::IntegerBase::Hexadecimal) }
{
}
