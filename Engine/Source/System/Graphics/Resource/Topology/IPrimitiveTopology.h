// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "System/Graphics/Data/PrimitiveTopology.h"
#include "System/Graphics/Resource/IGraphicsResource.h"

class IPrimitiveTopology
	: public IGraphicsResource
{
private:
	class FStatic final
	{
		friend class IGraphicsRenderer;

	public:
		static constexpr EPrimitiveTopology Targets[]
		{
			EPrimitiveTopology::PointList,
			EPrimitiveTopology::LineList,
			EPrimitiveTopology::LineStrip,
			EPrimitiveTopology::TriangleList,
			EPrimitiveTopology::TriangleStrip,
			EPrimitiveTopology::AdjacentLineList,
			EPrimitiveTopology::AdjacentLineStrip,
			EPrimitiveTopology::AdjacentTriangleList,
			EPrimitiveTopology::AdjacentTriangleStrip
		};

	public:
		inline const IPrimitiveTopology& operator[](EPrimitiveTopology Topology) const noexcept { return *Instances.at(Topology); }

	public:
		bool IsValid() const noexcept;

	private:
		inline void Clear() const noexcept { Instances.clear(); }

	private:
		mutable FPrimitiveTopologies Instances;
	};

public:
	static const FStatic& GetStatic() noexcept;

public:
	IPrimitiveTopology(EPrimitiveTopology Topology) : Topology{ Topology } {}
	virtual ~IPrimitiveTopology() noexcept = default;

	inline operator EPrimitiveTopology() const noexcept { return GetTopology(); }

public:
	virtual bool IsValid() const noexcept override final { return Topology != EPrimitiveTopology::Undefined; }
	inline EPrimitiveTopology GetTopology() const noexcept { return Topology; }

private:
	virtual void BindResourceImpl() const noexcept override = 0;

private:
	EPrimitiveTopology Topology;
};
