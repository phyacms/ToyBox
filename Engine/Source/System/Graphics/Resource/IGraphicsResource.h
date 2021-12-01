// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/Object.h"
#include "Type/Time.h"

class IGraphicsResource
{
public:
	IGraphicsResource() = default;
	virtual ~IGraphicsResource() noexcept = default;

	IGraphicsResource(const IGraphicsResource&) = delete;
	IGraphicsResource& operator=(const IGraphicsResource&) & = delete;
	IGraphicsResource(IGraphicsResource&&) noexcept = delete;
	IGraphicsResource& operator=(IGraphicsResource&&) & noexcept = delete;

public:
	virtual bool IsValid() const noexcept = 0;
	virtual bool BindResource() const noexcept = 0;
};
