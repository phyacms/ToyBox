// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "System/Graphics/Resource/IGraphicsResource.h"
#include "System/Graphics/Resource/Buffer/IDynamicBuffer.h"

class IShader
	: public IGraphicsResource
{
public:
	IShader() = default;
	virtual ~IShader() noexcept = default;

public:
	virtual bool IsValid() const noexcept override = 0;

	virtual std::size_t QueryConstantBufferIndex(std::string_view Name) const noexcept = 0;
	virtual FDynamicBuffer QueryConstantBuffer(std::size_t SlotIndex) const& noexcept = 0;
	inline FDynamicBuffer QueryConstantBuffer(std::string_view Name) const& noexcept { return QueryConstantBuffer(QueryConstantBufferIndex(Name)); }

private:
	virtual void BindResourceImpl() const noexcept override = 0;
};
