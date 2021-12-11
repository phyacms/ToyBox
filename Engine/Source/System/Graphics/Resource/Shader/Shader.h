// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Common.h"
#include "IShader.h"
#include "System/Graphics/Data/DynamicVertex.h"
#include "System/Graphics/Resource/IGraphicsResource.h"

class IVertexShader;
class IPixelShader;

enum class EShader : std::size_t
{
	VertexShader,
	PixelShader
};

class FShader
	: public IGraphicsResource
{
private:
	class FShaderProxy;
	static const FVertexDesc InvalidVertexDesc;

public:
	FShader();
	virtual ~FShader() noexcept = default;

public:
	FShaderProxy operator[](EShader Shader) const noexcept;

private:
	class FShaderProxy final
		: public IShader
	{
		friend FShaderProxy FShader::operator[](EShader Shader) const noexcept;

	private:
		FShaderProxy(IShader* Shader = nullptr) : Shader{ Shader } {}

	public:
		virtual ~FShaderProxy() noexcept = default;

	public:
		inline virtual bool IsValid() const noexcept override final { return Shader != nullptr && Shader->IsValid(); }

		inline virtual std::size_t QueryConstantBufferIndex(std::string_view Name) const noexcept override final
		{
			return IsValid()
				? Shader->QueryConstantBufferIndex(Name)
				: Common::InvalidIndexValue;
		}
		inline virtual FDynamicBuffer QueryConstantBuffer(std::size_t SlotIndex) const& noexcept override final
		{
			return IsValid()
				? Shader->QueryConstantBuffer(SlotIndex)
				: FDynamicBuffer{};
		}

	private:
		inline virtual void BindResourceImpl() const noexcept override final { Shader->BindResource(); }

	private:
		IShader* Shader;
	};

public:
	virtual bool IsValid() const noexcept override final;

	FShader& AddVertexShader(std::unique_ptr<IVertexShader>&& VertexShader) noexcept;
	FShader& AddPixelShader(std::unique_ptr<IPixelShader>&& PixelShader) noexcept;

	const FVertexDesc& GetVertexDescription() const noexcept;

private:
	virtual void BindResourceImpl() const noexcept override final;

	void MakeCache() const noexcept;

private:
	std::unique_ptr<IVertexShader> VertexShader;
	std::unique_ptr<IPixelShader> PixelShader;
	mutable std::vector<IShader*> Cached;
};
