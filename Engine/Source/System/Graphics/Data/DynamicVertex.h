// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Byte.h"

struct FVertexElementDesc final
{
	inline static constexpr auto InvalidPos{ static_cast<std::size_t>(-1) };

	std::size_t ByteSize{};
	std::size_t OffsetPos{ InvalidPos };

	inline bool IsValid() const noexcept { return OffsetPos != InvalidPos; }
};

class FVertexDesc final
{
private:

#ifdef _DEBUG
	using SemanticNameHash = std::string;
	inline static SemanticNameHash GetHash(std::string_view SemanticName) noexcept { return std::string{ SemanticName }; }
#else
	using SemanticNameHash = std::size_t;
	inline static SemanticNameHash GetHash(std::string_view SemanticName) noexcept { return std::hash<std::string_view>{}(SemanticName); }
#endif

	using FSemanticIndices = std::unordered_map<std::size_t, FVertexElementDesc>;
	using FElementDescs = std::unordered_map<SemanticNameHash, FSemanticIndices>;

public:
	FVertexDesc() : ElementDescs{} {}
	FVertexDesc(const FVertexDesc&) = default;
	FVertexDesc& operator=(const FVertexDesc&) & = default;
	FVertexDesc(FVertexDesc&&) noexcept = default;
	FVertexDesc& operator=(FVertexDesc&&) & noexcept = default;
	~FVertexDesc() noexcept = default;

public:
	inline bool IsValid() const noexcept { return CachedTotalByteSize != 0; }

	FVertexElementDesc GetElementDesc(
		std::string_view SemanticName,
		std::size_t SemanticIndex) const noexcept;
	inline std::size_t GetByteSize() const noexcept { return CachedTotalByteSize; }

	std::size_t AddElementDesc(
		std::string_view SemanticName,
		std::size_t SemanticIndex,
		FVertexElementDesc&& ElementDesc);

private:
	FElementDescs ElementDescs;
	std::size_t CachedTotalByteSize{};
};

class FVertexData final
{
private:
	class FVertexProxy final
	{
		friend class FVertexData;

	private:
		class FElementProxy final
		{
			friend class FVertexProxy;

		public:
			FElementProxy() : FElementProxy(nullptr, 0) {}
			FElementProxy(const FElementProxy&) = default;
			FElementProxy& operator=(const FElementProxy&) & = default;
			FElementProxy(FElementProxy&&) noexcept = default;
			FElementProxy& operator=(FElementProxy&&) &noexcept = default;
			~FElementProxy() noexcept = default;

		private:
			FElementProxy(Byte* Dest, std::size_t ByteSize)
				: Dest{ Dest }, ByteSize{ ByteSize } {}

		public:
			template<
				typename T,
				typename = std::enable_if_t<
					std::is_trivial_v<T> &&
					std::is_standard_layout_v<T>>>
			inline FElementProxy& operator=(T&& Value) noexcept
			{
				if (IsValid())
				{
					std::memcpy(Dest, &Value, ByteSize);
				}
				return *this;
			}
			template<
				typename T,
				std::size_t N,
				typename = std::enable_if_t<
				std::is_trivial_v<T>&&
				std::is_standard_layout_v<T>>>
				inline FElementProxy& operator=(const T(&Array)[N]) noexcept
			{
				if (IsValid())
				{
					std::memcpy(Dest, Array, ByteSize);
				}
				return *this;
			}

		public:
			inline bool IsValid() const noexcept { return Dest != nullptr && ByteSize != 0; }

		private:
			Byte* Dest;
			std::size_t ByteSize;
		};

	public:
		FVertexProxy() : Desc{}, Dest{} {}
		FVertexProxy(const FVertexProxy&) = default;
		FVertexProxy& operator=(const FVertexProxy&) & = default;
		FVertexProxy(FVertexProxy&&) noexcept = default;
		FVertexProxy& operator=(FVertexProxy&&) & noexcept = default;
		~FVertexProxy() noexcept = default;

	private:
		FVertexProxy(const FVertexDesc& Desc, Byte* Dest)
			: Desc{ &Desc }, Dest{ Dest } {}

	public:
		FElementProxy At(std::string_view SemanticName, std::size_t SemanticIndex);

	private:
		const FVertexDesc* Desc;
		Byte* Dest;
	};

public:
	FVertexData() : Desc{}, Bytes {} {}
	FVertexData(const FVertexDesc& Desc, std::size_t Count = 0)
		: Desc{ &Desc }, Bytes(Desc.GetByteSize() * Count) {}
	FVertexData(const FVertexData&) = default;
	FVertexData& operator=(const FVertexData&) & = default;
	FVertexData(FVertexData&&) noexcept = default;
	FVertexData& operator=(FVertexData&&) & noexcept = default;
	~FVertexData() noexcept = default;

public:
	inline FVertexProxy operator[](std::size_t Index)& { return At(Index); }
	FVertexProxy At(std::size_t Index)&;

public:
	inline bool IsValid() const noexcept { return Desc != nullptr; }
	inline std::size_t GetCount() const noexcept { return IsValid() ? GetByteSize() / GetStride() : 0; }
	inline std::size_t GetStride() const noexcept { return IsValid() ? Desc->GetByteSize() : 0; }
	inline std::size_t GetByteSize() const noexcept { return Bytes.GetByteSize(); }

	inline Byte* GetPtr() noexcept { return Bytes.GetPtr(); }
	inline const Byte* GetPtr() const noexcept { return Bytes.GetPtr(); }

	inline void Clear() noexcept { Bytes.Clear(); }
	inline void Resize(std::size_t Count) { Bytes.Reallocate(GetStride() * Count); }

private:
	const FVertexDesc* Desc;
	FBytes Bytes;
};
