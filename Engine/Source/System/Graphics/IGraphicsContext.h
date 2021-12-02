// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Type/Object.h"
#include "Type/Time.h"
#include "Type/Color.h"
#include "Type/Delegate/Delegate.h"
#include "Type/Delegate/MulticastDelegate.h"
#include "Type/ScreenSpace/Dim.h"
#include "Math/Affine.h"
#include "Data/Projection.h"
#include "IGraphicsRenderer.h"

class FSystemWindow;

class IGraphicsContext
	: public TObject<IGraphicsContext>
{
private:
	class FScene final
	{
		friend class IGraphicsContext;

	private:
		FScene(const IGraphicsContext& Context)
			: Context{ &Context }
		{
			Context.BeginScene(
				Context.BackgroundColor,
				Context.ClearColor);
		}

	public:
		~FScene() noexcept { Context->EndScene(); }

		FScene(const FScene&) = delete;
		FScene& operator=(const FScene&) & = delete;
		FScene(FScene&&) noexcept = delete;
		FScene& operator=(FScene&&) & noexcept = delete;

	private:
		const IGraphicsContext* Context;
	};

public:
	IGraphicsContext(
		IGraphicsRenderer& Renderer,
		FSystemWindow& OutputWindow);
	IGraphicsContext(IGraphicsContext&&) noexcept = default;
	IGraphicsContext& operator=(IGraphicsContext&&) & noexcept = default;
	virtual ~IGraphicsContext() noexcept;

	IGraphicsContext(const IGraphicsContext&) = delete;
	IGraphicsContext& operator=(const IGraphicsContext&) & = delete;

public:
	inline bool IsValid() const noexcept { return Renderer.IsValid() && OutputWindow.IsValid() && IsValidImpl(); }

	void SetViewportDimension(UDim Dimension);
	void SetViewportAspectRatio(float MinimumAspectRatio, float MaximumAspectRatio);
	inline void SetViewportAspectRatio(float AspectRatio = 0.0f) { SetViewportAspectRatio(AspectRatio, AspectRatio); }
	inline const FScreenArea& GetViewportArea() const noexcept { return Viewport.GetArea(); }

	void SetProjection(FProjection Projection);
	virtual const Affine::Matrix4x4f& GetProjectionMatrix() const noexcept = 0;

	inline void SetBackgroundColor(FColor BackgroundColor) noexcept { this->BackgroundColor = std::move(BackgroundColor); }
	inline const FColor& GetBackgroundColor() const noexcept { return BackgroundColor; }

	inline void SetClearColor(FColor ClearColor) noexcept { this->ClearColor = std::move(ClearColor); }
	inline const FColor& GetClearColor() const noexcept { return ClearColor; }

	inline [[nodiscard]] FScene CreateScene() const noexcept { return FScene{ *this }; }
	virtual void DrawLine(const FScreenLocation& Begin, const FScreenLocation& End, const FColor& Color = {}, float Width = 1.0f) = 0;
	virtual void DrawRect(const FScreenArea& Rect, const FColor& Color = {}, float Width = 1.0f) = 0;

protected:
	void UpdateViewport();
	inline void UpdateProjection() { UpdateProjection(Projection); }

	inline FSystemWindow& GetOutputWindow() const noexcept { return *OutputWindow.GetAddress(); }

private:
	virtual bool IsValidImpl() const noexcept = 0;

	virtual void ResizeBuffer(const FScreenSize& ClientAreaSize) = 0;
	virtual void UpdateViewport(const FScreenArea& ViewportArea) = 0;
	virtual void UpdateProjection(const FProjection& Projection) = 0;

	virtual void BeginScene(const FColor& BackgroundColor, const FColor& ClearColor) const = 0;
	virtual void EndScene() const = 0;

	FScreenArea GetViewportTarget() const noexcept;

private:
	AObject<IGraphicsRenderer> Renderer;

	AObject<FSystemWindow> OutputWindow;
	ADelegateHandle DH_OnResized;

	FAspectRatio Viewport;
	FProjection Projection;

	FColor BackgroundColor;
	FColor ClearColor;

public:
	using FOnOutputWindowSizeChanged = TMulticastDelegate<void(const FScreenSize&)>;
	FOnOutputWindowSizeChanged OnOutputWindowSizeChanged;
	using FOnViewportChanged = TMulticastDelegate<void(const FScreenArea&)>;
	FOnViewportChanged OnViewportChanged;
};
