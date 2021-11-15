// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Log.h"
#include "Platform/Platform.h"

const FLogEndl& FLogEndl::GetInstance()
{
	static FLogEndl Instance{};
	return Instance;
}

void FLogStream::operator<<(const FLogEndl& Endl) && noexcept
{
	PlatformFunctions::PrintDebugOutput(Buffer);
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

void FLog::SetIdentifier(FStringView Identifier)
{
	this->Identifier = Identifier;
}
