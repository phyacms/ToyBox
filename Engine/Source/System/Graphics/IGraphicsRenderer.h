// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object/Object.h"
#include "Data/PrimitiveTopology.h"

class FSystemWindow;
class FGraphics;
class IGraphicsContext;
class IPrimitiveTopology;

class IGraphicsRenderer
	: public TObject<IGraphicsRenderer>
{
public:
	explicit IGraphicsRenderer(FGraphics& Graphics)
		: TObject<IGraphicsRenderer>(*this)
		, Graphics{ &Graphics } {}
	virtual ~IGraphicsRenderer() noexcept = default;

	IGraphicsRenderer(const IGraphicsRenderer&) = delete;
	IGraphicsRenderer& operator=(const IGraphicsRenderer&) & = delete;
	IGraphicsRenderer(IGraphicsRenderer&&) noexcept = delete;
	IGraphicsRenderer& operator=(IGraphicsRenderer&&) & noexcept = delete;

public:
	virtual bool IsValid() const noexcept = 0;

	bool CreateStaticResources() noexcept;
	virtual [[nodiscard]] std::unique_ptr<IGraphicsContext> CreateContext(FSystemWindow& OutputWindow) & = 0;

protected:
	inline FGraphics& GetGraphics() const noexcept { return *Graphics; }

private:
	virtual std::unique_ptr<IPrimitiveTopology> CreatePrimitiveTopology(EPrimitiveTopology Topology) noexcept = 0;

private:
	FGraphics* Graphics;
};
