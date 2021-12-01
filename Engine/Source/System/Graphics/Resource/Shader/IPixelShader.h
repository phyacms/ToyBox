// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "IShader.h"

class IPixelShader
	: public IShader
{
public:
	IPixelShader() = default;
	virtual ~IPixelShader() noexcept = default;

public:
	virtual bool IsValid() const noexcept override = 0;
	virtual bool BindResource() const noexcept override = 0;

	virtual std::size_t QueryConstantBufferIndex(std::string_view Name) const noexcept override = 0;
	virtual FDynamicBuffer QueryConstantBuffer(std::size_t SlotIndex) const& noexcept override = 0;
};
