// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_GUI.h"
#include "VRWidget.h"

AVRWidget::AVRWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWidgetClicked = false;
	bWidgetVisibility = true;
	WidgetScale = FVector(1, 1, 1);
}

FString AVRWidget::GetWidgetName()
{
	return WidgetName;
}

void AVRWidget::SetWidgetName(FString name)
{
	WidgetName = name;
}

FVector AVRWidget::GetWidgetCanvasCoords()
{
	FVector Loc = FVector(CoordX, CoordY, 0);

	return Loc;
}

void AVRWidget::SetWidgetLocation(FVector WorldLoc, float X, float Y)
{
	WidgetLocation = WorldLoc;
	SetActorLocation(WorldLoc);

	CoordX = X;
	CoordY = Y;
}

void AVRWidget::SetWidgetScale(FVector scale)
{
	WidgetScale = scale;
	SetActorScale3D(scale);
}

void AVRWidget::WidgetClicked(UPrimitiveComponent* ComponentClicked)
{
	bWidgetClicked = true;
	this->ComponentClicked = ComponentClicked;
}

void AVRWidget::WidgetClickRelease()
{
	bWidgetClicked = false;
	ComponentClicked = NULL;
}