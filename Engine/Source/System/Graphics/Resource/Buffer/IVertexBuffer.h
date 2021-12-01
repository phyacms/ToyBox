// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "IBuffer.h"
#include "System/Graphics/Data/DynamicVertex.h"

class IVertexBuffer
	: public IBuffer
{
public:
	explicit IVertexBuffer(const FVertexData& VertexData)
		: IBuffer()
		, Count{ VertexData.GetCount() }
		, Stride{ VertexData.GetStride() } {}
	virtual ~IVertexBuffer() noexcept = default;

public:
	virtual bool BindResource() const noexcept override = 0;

	inline virtual std::size_t GetCount() const noexcept override final { return Count; }
	inline virtual std::size_t GetStride() const noexcept override final { return Stride; }

private:
	virtual bool IsValidImpl() const noexcept override = 0;

private:
	std::size_t Count;
	std::size_t Stride;
};
