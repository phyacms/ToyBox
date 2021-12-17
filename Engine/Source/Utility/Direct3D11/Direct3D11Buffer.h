// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "Direct3D11.h"

class FDirect3D11Buffer final
{
public:
	inline static constexpr auto ByteSizeLimit{ D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT };
	inline static constexpr auto CalcElementCountLimit(std::size_t ElemSize) noexcept { return ByteSizeLimit / ElemSize; }

	class Mapping final
	{
	public:
		Mapping() : Buffer{} {}
		Mapping(FDirect3D11Buffer& Buffer) : Buffer{
			(Buffer.IsDynamic() && Buffer.IsWritable())
			? &Buffer
			: nullptr } {}

	public:
		inline bool IsValid() const noexcept { return Buffer != nullptr; }

		template<typename T = std::size_t> inline T GetByteSize() const noexcept { return IsValid() ? Buffer->GetByteSize<T>() : 0; }
		template<typename T = std::size_t> inline T GetStride() const noexcept { return IsValid() ? Buffer->GetStride<T>() : 0; }

		inline bool UpdateBuffer(ID3D11DeviceContext& Context, const void* SrcData, std::size_t ByteSize) noexcept { return IsValid() ? Buffer->UpdateBuffer(Context, SrcData, ByteSize) : false; }

	private:
		FDirect3D11Buffer* Buffer;
	};

public:
	FDirect3D11Buffer(
		ID3D11Device& Device,
		UINT BindFlags,
		D3D11_USAGE Usage,
		UINT CPUAccessFlags,
		UINT StructureCount,
		UINT StructureByteStride,
		UINT MiscFlags,
		const void* InitialSrcData,
		UINT RowPitch,
		UINT SlicePitch);
	FDirect3D11Buffer(const FDirect3D11Buffer&) = default;
	FDirect3D11Buffer& operator=(const FDirect3D11Buffer&) & = default;
	FDirect3D11Buffer(FDirect3D11Buffer&&) noexcept = default;
	FDirect3D11Buffer& operator=(FDirect3D11Buffer&&) & noexcept = default;
	~FDirect3D11Buffer() noexcept { Buffer.Reset(); }

public:
	inline bool IsValid() const noexcept { return Buffer != nullptr; }

	inline bool IsDynamic() const noexcept { return bDynamic; }
	inline bool IsWritable() const noexcept { return bWritable; }

	template<typename T = std::size_t> inline T GetStride() const noexcept { return static_cast<T>(Stride); }
	template<typename T = std::size_t> inline T GetByteSize() const noexcept { return static_cast<T>(ByteSize); }

	inline ID3D11Buffer* GetPtr() const noexcept { return Buffer.Get(); }
	inline ID3D11Buffer* const* GetAddressOf() const noexcept { return Buffer.GetAddressOf(); }

private:
	bool UpdateBuffer(ID3D11DeviceContext& Context, const void* SrcData, std::size_t ByteSize) noexcept;

private:
	TComPtr<ID3D11Buffer> Buffer;
	bool bDynamic;
	bool bWritable;
	std::size_t Stride;
	std::size_t ByteSize;
};

#endif
