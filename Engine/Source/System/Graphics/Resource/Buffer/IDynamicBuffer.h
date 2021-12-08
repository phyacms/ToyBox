// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "IBuffer.h"

class IDynamicBuffer
	: public IBuffer
{
public:
	IDynamicBuffer() = default;
	virtual ~IDynamicBuffer() noexcept = default;

public:
	virtual bool UpdateBuffer(const void* SrcData) = 0;

	virtual std::size_t GetCount() const noexcept override = 0;
	virtual std::size_t GetStride() const noexcept override = 0;

private:
	virtual bool IsValidImpl() const noexcept override = 0;
	virtual void BindResourceImpl() const noexcept override = 0;
};

// Proxy of IDynamicBuffer.
class FDynamicBuffer final
{
public:
	FDynamicBuffer() : Buffer{} {}
	FDynamicBuffer(IDynamicBuffer& Buffer) : Buffer{ &Buffer } {}

public:
	inline bool IsValid() const noexcept { return Buffer != nullptr && Buffer->IsValid(); }
	inline bool Update(const void* SrcData) { return IsValid() ? Buffer->UpdateBuffer(SrcData) : false; }

	inline std::size_t GetStride() const noexcept { return IsValid() ? Buffer->GetStride() : 0; }
	inline std::size_t GetCount() const noexcept { return IsValid() ? Buffer->GetCount() : 0; }
	inline std::size_t GetByteSize() const noexcept { return IsValid() ? Buffer->GetByteSize() : 0; }

private:
	IDynamicBuffer* Buffer;
};
