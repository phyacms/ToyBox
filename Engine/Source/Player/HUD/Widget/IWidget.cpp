// Copyrights 2021 by phyacms. All Rights Reserved.

#include "Engine.h"
#include "IWidget.h"

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

IWidget::IWidget(IWidget* Parent, AUniqueId&& UniqueId, UDim Dimension, bool bVisible)
	: TObject<IWidget>(*this)
	, UniqueId{ std::move(UniqueId) }
	, Parent{ Parent }
	, Children{}
	, Dimension{ std::move(Dimension) }
	, bVisible{ bVisible }
{
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

void IWidget::CalcAbsoluteArea(const FScreenArea& Base)
{
	Dimension.UpdateArea(Base);
	std::for_each(
		std::execution::par_unseq,
		std::cbegin(Children.Widgets),
		std::cend(Children.Widgets),
		[this](const auto& Pair)->void { Pair.second->CalcAbsoluteArea(Dimension.GetArea()); });
}

void IWidget::Render(IGraphicsContext& Context, FTimeDuration DeltaTime)
{
	if (IsVisible())
	{
		RenderImpl(Context, Dimension.GetArea(), DeltaTime);
		for (auto& [Index, Child] : Children.Widgets)
		{
			Child->Render(Context, DeltaTime);
		}
	}
}
