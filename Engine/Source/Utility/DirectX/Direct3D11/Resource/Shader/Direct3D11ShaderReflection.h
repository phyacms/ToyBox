// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#ifdef PLATFORM_WINDOWS

#include "System/Graphics/Resource/Buffer/IDynamicBuffer.h"
#include "Utility/DirectX/Direct3D11/Direct3D11.h"

class FDirect3D11Renderer;
class IDirect3D11ShaderConstantBuffer;

class FDirect3D11ShaderReflection
{
public:
	inline static constexpr auto InvalidIndex{ static_cast<std::size_t>(-1) };

public:
	FDirect3D11ShaderReflection();
	virtual ~FDirect3D11ShaderReflection() noexcept;

	FDirect3D11ShaderReflection(const FDirect3D11ShaderReflection&) = delete;
	FDirect3D11ShaderReflection& operator=(const FDirect3D11ShaderReflection&) & = delete;
	FDirect3D11ShaderReflection(FDirect3D11ShaderReflection&&) noexcept = delete;
	FDirect3D11ShaderReflection& operator=(FDirect3D11ShaderReflection&&) & noexcept = delete;

protected:
	template<typename ConstBufT>
	inline bool Initialize(FDirect3D11Renderer& Renderer, ID3DBlob& ByteCode) noexcept
	{
		return CreateReflection(ByteCode)
			&& InitializeImpl(Renderer, ByteCode)
			&& CreateConstantBuffers<ConstBufT>(Renderer);
	}
	void Terminate() noexcept;

	inline bool IsReflected() const noexcept { return Reflector != nullptr; }

private:
	bool CreateReflection(ID3DBlob& ByteCode) noexcept;
	template<
		typename T,
		typename = std::enable_if_t<std::is_base_of_v<IDirect3D11ShaderConstantBuffer, T>>>
	inline bool CreateConstantBuffers(FDirect3D11Renderer& Renderer) noexcept
	{
		if (!ReflectConstantBuffer())
		{
			return false;
		}

		for (std::size_t BufIndex{}; BufIndex != ConstBuf.BufDescs.size(); ++BufIndex)
		{
			if (!AddConstantBuffer(std::make_unique<T>(
				Renderer,
				ConstBuf.InputBindDescs[BufIndex].BindPoint,
				ConstBuf.BufDescs[BufIndex].Size,
				nullptr)))
			{
				return false;
			}
		}

		return true;
	}

	bool ReflectConstantBuffer() noexcept;
	bool AddConstantBuffer(std::unique_ptr<IDirect3D11ShaderConstantBuffer>&& Created) noexcept;

public:
	inline bool IsValid() const noexcept { return IsReflected() && IsValidImpl(); }
	bool BindResource(ID3D11DeviceContext& Context) const noexcept;

	inline ID3D11ShaderReflection& GetShaderReflector() const& noexcept { return *Reflector.Get(); }
	inline const D3D11_SHADER_DESC& GetShaderDescriptions() const& noexcept { return ShaderDesc; }
	inline D3D11_SHADER_DESC GetShaderDescriptions() const&& noexcept { return ShaderDesc; }

	std::size_t QueryConstantBufferIndex(std::string_view Name) const noexcept;
	FDynamicBuffer QueryConstantBuffer(std::size_t SlotIndex) const& noexcept;
	inline FDynamicBuffer QueryConstantBuffer(std::string_view Name) const& noexcept { return QueryConstantBuffer(QueryConstantBufferIndex(Name)); }

private:
	virtual bool InitializeImpl(FDirect3D11Renderer& Renderer, ID3DBlob& ByteCode) noexcept = 0;
	virtual void TerminateImpl() noexcept = 0;

	virtual bool IsValidImpl() const noexcept = 0;
	virtual void BindResourceImpl(ID3D11DeviceContext& Context) const noexcept = 0;

private:
	TComPtr<ID3D11ShaderReflection> Reflector;
	D3D11_SHADER_DESC ShaderDesc;
	struct
	{
		std::vector<D3D11_SHADER_BUFFER_DESC> BufDescs{};
		std::vector<D3D11_SHADER_INPUT_BIND_DESC> InputBindDescs{};
		std::unordered_map<std::size_t, std::unique_ptr<IDirect3D11ShaderConstantBuffer>> Objects{};
	}
	ConstBuf;
};

#endif
