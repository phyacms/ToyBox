// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "Shader.h"
#include "IVertexShader.h"
#include "IPixelShader.h"

const FVertexDesc FShader::InvalidVertexDesc{};

FShader::FShader()
	: VertexShader{}
	, PixelShader{}
	, Cached{}
{
	Cached.reserve(6);
}

FShader::FShaderProxy FShader::operator[](EShader Shader) const noexcept
{
	switch (Shader)
	{
		case EShader::VertexShader: return VertexShader.get();
		case EShader::PixelShader: return PixelShader.get();
		default: return {};
	}
}

bool FShader::IsValid() const noexcept
{
	return !Cached.empty();
}

FShader& FShader::AddVertexShader(std::unique_ptr<IVertexShader>&& VertexShader) noexcept
{
	this->VertexShader = (VertexShader != nullptr && VertexShader->IsValid())
		? std::move(VertexShader)
		: nullptr;
	MakeCache();
	return *this;
}

FShader& FShader::AddPixelShader(std::unique_ptr<IPixelShader>&& PixelShader) noexcept
{
	this->PixelShader = (PixelShader != nullptr && PixelShader->IsValid())
		? std::move(PixelShader)
		: nullptr;
	MakeCache();
	return *this;
}

const FVertexDesc& FShader::GetVertexDescription() const noexcept
{
	return (VertexShader != nullptr && VertexShader->IsValid())
		? VertexShader->GetVertexDescription()
		: InvalidVertexDesc;
}

void FShader::BindResourceImpl() const noexcept
{
	for (const auto Shader : Cached)
	{
		Shader->BindResource();
	}
}

void FShader::MakeCache() const noexcept
{
	Cached.clear();
	if (VertexShader != nullptr) { Cached.emplace_back(VertexShader.get()); }
	if (PixelShader != nullptr) { Cached.emplace_back(PixelShader.get()); }
}
