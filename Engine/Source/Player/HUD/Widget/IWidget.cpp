// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IWidget.h"

const URect IWidget::DefaultRect{
	.Location{ URelCoord{ 0, 0 } },
	.Size{ URelCoord{ 1, 1 } } };

bool IWidget::FChildren::Add(std::unique_ptr<IWidget>&& Widget)
{
	const auto Index{ Widget->GetIndex() };
	Widgets.erase(Index);
	auto [cIt, bEmplaced] = Widgets.emplace(std::make_pair(Index, std::move(Widget)));
	return bEmplaced;
}

void IWidget::FChildren::Remove(IWidget& Widget) noexcept
{
	const auto Index{ Widget.GetIndex() };
	Widgets.erase(Index);
}

void IWidget::FChildren::Clear() noexcept
{
	Widgets.clear();
}

IWidget::IWidget(IWidget* Parent, AUniqueId&& UniqueId, URect Rect, bool bVisible)
	: TObject<IWidget>(*this)
	, UniqueId{ std::move(UniqueId) }
	, Parent{ Parent }
	, Children{}
	, Dimension{
		.Rect{ std::move(Rect) },
		.CachedAbsoluteArea{} }
	, bVisible{ bVisible }
{
	if (!IsRoot())
	{
		CalcAbsoluteArea(Parent->Dimension.CachedAbsoluteArea);
	}
}

IWidget::~IWidget() noexcept
{
}

bool IWidget::IsValid() const noexcept
{
	return !GetWidgetName().IsEmpty()
		&& (!IsRoot()
			? UniqueId.IsValid()
			: true)
		&& IsValidImpl();
}

void IWidget::Render(ISurface2D& Surface, FTimeDuration DeltaTime)
{
	if (IsVisible())
	{
		RenderImpl(Surface, Dimension.CachedAbsoluteArea, DeltaTime);
		std::for_each(
			std::execution::seq,
			std::cbegin(Children.Widgets),
			std::cend(Children.Widgets),
			[&Surface, &DeltaTime](const auto& Pair)->void { Pair.second->Render(Surface, DeltaTime); });
	}
}

void IWidget::CalcAbsoluteArea(const FScreenArea& Base)
{
	Dimension.CachedAbsoluteArea = ::ToScreenSpace(Dimension.Rect, Base);
	std::for_each(
		std::execution::par_unseq,
		std::cbegin(Children.Widgets),
		std::cend(Children.Widgets),
		[this](const auto& Pair)->void { Pair.second->CalcAbsoluteArea(Dimension.CachedAbsoluteArea); });
}
