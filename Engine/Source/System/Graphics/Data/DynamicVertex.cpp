// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "DynamicVertex.h"

FVertexElementDesc FVertexDesc::GetElementDesc(
	std::string_view SemanticName,
	std::size_t SemanticIndex) const noexcept
{
	if (IsValid())
	{
		if (const auto& cItIndices{ ElementDescs.find(GetHash(SemanticName)) };
			cItIndices != std::cend(ElementDescs))
		{
			if (const auto& cItElement{ cItIndices->second.find(SemanticIndex) };
				cItElement != std::cend(cItIndices->second))
			{
				return cItElement->second;
			}
		}
	}
	return {};
}

std::size_t FVertexDesc::AddElementDesc(
	std::string_view SemanticName,
	std::size_t SemanticIndex,
	FVertexElementDesc&& ElementDesc)
{
	if (ElementDesc.IsValid())
	{
		if (const auto& [cIt, bEmplaced] = ElementDescs[
			GetHash(SemanticName)]
			.emplace(
				SemanticIndex,
				std::move(ElementDesc));
				bEmplaced)
		{
			CachedTotalByteSize += cIt->second.ByteSize;
		}
	}
	return CachedTotalByteSize;
}

FVertexData::FVertexProxy::FElementProxy FVertexData::FVertexProxy::At(
	std::string_view SemanticName,
	std::size_t SemanticIndex)
{
	if (Desc != nullptr)
	{
		if (const auto& ElementDesc{ Desc->GetElementDesc(SemanticName, SemanticIndex) };
			ElementDesc.IsValid() && Dest != nullptr)
		{
			return { Dest + ElementDesc.OffsetPos, ElementDesc.ByteSize };
		}
	}
	return {};
}

FVertexData::FVertexProxy FVertexData::At(std::size_t Index) &
{
	const auto Count{ GetCount() };
	if (Count == 0 || Index >= Count)
	{
		throw std::out_of_range{ "Invalid vertex index." };
	}

	return { *Desc, Bytes.GetPtr() + GetStride() * Index };
}
