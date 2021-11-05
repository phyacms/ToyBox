// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "Type/TimePoint.h"

class FSystemWindow;

class IGraphicsContext
{
public:
	explicit IGraphicsContext(FSystemWindow& OutputWindow);
	virtual ~IGraphicsContext() noexcept = default;

	IGraphicsContext(const IGraphicsContext&) = delete;
	IGraphicsContext& operator=(const IGraphicsContext&) = delete;
	IGraphicsContext(IGraphicsContext&&) noexcept = default;
	IGraphicsContext& operator=(IGraphicsContext&&) noexcept = default;

public:
	bool IsValid() const noexcept { return OutputWindow.IsValid() && IsValidImpl(); }
	virtual void Render(FTimeDuration DeltaTime) const = 0;

protected:
	FSystemWindow& GetOutputWindow() const noexcept { return *OutputWindow.GetAddress(); }

private:
	virtual bool IsValidImpl() const noexcept = 0;

private:
	AObject<FSystemWindow> OutputWindow;
};
