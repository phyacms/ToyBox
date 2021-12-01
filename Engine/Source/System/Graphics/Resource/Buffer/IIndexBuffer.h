// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "IBuffer.h"
#include "System/Graphics/Data/DynamicVertex.h"

class IIndexBuffer
	: public IBuffer
{
public:
	explicit IIndexBuffer(const FIndexData& IndexData)
		: IBuffer()
		, Count{ IndexData.size() } {}
	virtual ~IIndexBuffer() noexcept = default;

public:
	virtual bool BindResource() const noexcept override = 0;

	inline virtual std::size_t GetCount() const noexcept override final { return Count; }
	inline virtual std::size_t GetStride() const noexcept override final { return sizeof(FIndexData::value_type); }

private:
	virtual bool IsValidImpl() const noexcept override = 0;

private:
	std::size_t Count;
};
