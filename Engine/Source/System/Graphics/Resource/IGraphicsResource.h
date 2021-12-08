// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Type/Time.h"
#include "Type/Object/Object.h"

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
	inline void BindResource() const noexcept { if (IsValid()) { BindResourceImpl(); } 	}

private:
	virtual void BindResourceImpl() const noexcept = 0;
};
