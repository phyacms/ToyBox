// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "System/Graphics/Resource/IGraphicsResource.h"

class IBuffer
	: public IGraphicsResource
{
public:
	IBuffer() = default;
	virtual ~IBuffer() noexcept = default;

public:
	virtual bool IsValid() const noexcept override final { return GetByteSize() != 0 && IsValidImpl(); }
	virtual bool BindResource() const noexcept override = 0;

	virtual std::size_t GetCount() const noexcept = 0;
	virtual std::size_t GetStride() const noexcept = 0;
	inline std::size_t GetByteSize() const noexcept { return GetStride() * GetCount(); }

private:
	virtual bool IsValidImpl() const noexcept = 0;
};
