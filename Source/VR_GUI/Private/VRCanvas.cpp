// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_GUI.h"
#include "VRCanvas.h"

AVRCanvas::AVRCanvas(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	RootComponent = Root;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("StaticMesh'/Game/GUI/Meshes/Mesh_Canvas.Mesh_Canvas'"));

	CanvasMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CanvasMesh"));
	CanvasMesh->SetStaticMesh(Mesh.Object);

	CanvasMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CanvasMesh->SetCollisionObjectType(ECollisionChannel::ECC_EngineTraceChannel1);

	CanvasMesh->AttachTo(Root);

	static ConstructorHelpers::FObjectFinder<UMaterial> Mat(TEXT("Material'/Game/GUI/Materials/Mat_Canvas.Mat_Canvas'"));

	CanvasMat = CanvasMesh->CreateDynamicMaterialInstance(0, Mat.Object);
	
	CanvasColor = FLinearColor(0, 0, 0, 0.7);
	CanvasMat->SetVectorParameterValue("Color", CanvasColor);

	DistanceFromViewer = 200;
}

//-----------------Create widgets here-------------------//
//Might have a seperate class for creating/adding widgets later//

void AVRCanvas::BeginPlay()
{
	Super::BeginPlay();
	
	//Resize Canvas
	ResizeCanvas(432, 243, 400);

	//Layer Creation
	AlwaysVisibleLayer.LayerName = "AlwaysVisible";
	AddWidgetLayer("CanvasPropertyWidgets", true);

	//Layer Toggle Button

	AVRButton* LayerButton = Cast<AVRButton>(GetWorld()->SpawnActor(AVRButton::StaticClass()));
	AddWidget(LayerButton, 360, 10, "LayerToggle", "AlwaysVisible");

	BindWidgetToLayerToggle(LayerButton);
	LayerButton->SetButtonText(" Next");

	//-----------------------
	//Widgets for editing Canvas Appearence
	//-----------------------

	//Checkbox for Visiblity

	AVRCheckbox* VisibleCheckbox = Cast<AVRCheckbox>(GetWorld()->SpawnActor(AVRCheckbox::StaticClass()));
	AddWidget(VisibleCheckbox, 20, 20, "Visibility", "CanvasPropertyWidgets");

	VisibleCheckbox->SetWidgetScale(FVector(1.5, 1.5, 1.5));

	BindWidgetToVisibility(VisibleCheckbox);

	AVRText* VisibleText = Cast<AVRText>(GetWorld()->SpawnActor(AVRText::StaticClass()));
	AddWidget(VisibleText, 45, 40, "VisText", "CanvasPropertyWidgets");

	VisibleText->SetText("Background");

	//------Sliders for Colors and Transparency-----//
	//Red

	AVRSlider* RSlider = Cast<AVRSlider>(GetWorld()->SpawnActor(AVRSlider::StaticClass()));
	AddWidget(RSlider, 20, 50, "Red", "CanvasPropertyWidgets");

	RSlider->ChangeSliderLength(100);

	BindWidgetToRed(RSlider);

	AVRText* RedText = Cast<AVRText>(GetWorld()->SpawnActor(AVRText::StaticClass()));
	AddWidget(RedText, 150, 70, "RedText", "CanvasPropertyWidgets");

	RedText->SetText("Red");

	//Green

	AVRSlider* GSlider = Cast<AVRSlider>(GetWorld()->SpawnActor(AVRSlider::StaticClass()));
	AddWidget(GSlider, 20, 80, "Green", "CanvasPropertyWidgets");

	GSlider->ChangeSliderLength(100);

	BindWidgetToGreen(GSlider);

	AVRText* GreenText = Cast<AVRText>(GetWorld()->SpawnActor(AVRText::StaticClass()));
	AddWidget(GreenText, 150, 100, "GreenText", "CanvasPropertyWidgets");

	GreenText->SetText("Green");

	//Blue

	AVRSlider* BSlider = Cast<AVRSlider>(GetWorld()->SpawnActor(AVRSlider::StaticClass()));
	AddWidget(BSlider, 20, 110, "Blue", "CanvasPropertyWidgets");

	BSlider->ChangeSliderLength(100);

	BindWidgetToBlue(BSlider);

	AVRText* BlueText = Cast<AVRText>(GetWorld()->SpawnActor(AVRText::StaticClass()));
	AddWidget(BlueText, 150, 130, "BlueText", "CanvasPropertyWidgets");

	BlueText->SetText("Blue");

	//Alpha

	AVRSlider* AlphaSlider = Cast<AVRSlider>(GetWorld()->SpawnActor(AVRSlider::StaticClass()));
	AddWidget(AlphaSlider, 20, 140, "Alpha", "CanvasPropertyWidgets");

	AlphaSlider->ChangeSliderLength(100);

	BindWidgetToTransparency(AlphaSlider);

	AVRText* AlphaText = Cast<AVRText>(GetWorld()->SpawnActor(AVRText::StaticClass()));
	AddWidget(AlphaText, 150, 160, "AlphaText", "CanvasPropertyWidgets");

	AlphaText->SetText("Alpha");
}

//-----------------------------------------------------//

void AVRCanvas::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCanvasProperties();
}

void AVRCanvas::UpdateCanvasProperties()
{
	//Visibility of Canvas
	if (VisibilityCheckbox)
	{
		if (VisibilityCheckbox->IsChecked())
		{
			CanvasMesh->SetVisibility(true);
		}
		else
		{
			CanvasMesh->SetVisibility(false);
		}
	}

	float Red = 0;
	float Green = 0;
	float Blue = 0;
	float Alpha = .5;

	//Transparency
	if (TransparencySlider)
		Alpha = TransparencySlider->GetPositionPercent();
	if (RedSlider)
		Red = RedSlider->GetPositionPercent();
	if (GreenSlider)
		Green = GreenSlider->GetPositionPercent();
	if (BlueSlider)
		Blue = BlueSlider->GetPositionPercent();

	SetCanvasColor(FLinearColor(Red, Green, Blue, Alpha));

	if (ToggleLayerButton)
	{
		if (ToggleLayerButton->ButtonClicked())
		{
			ToggleVisibleLayers();
		}
	}

	AVRKnob* Knob = Cast<AVRKnob>(GetWidget("Knob"));

	if (Knob)
	{
		if (Knob->ButtonClicked())
		{
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Knob Button Clicked");
		}
	}
}

FVector AVRCanvas::ConvertToWorldCoords(float X, float Y)
{
	FVector WorldLoc = GetActorLocation();
	WorldLoc.Y += X;
	WorldLoc.Z -= Y;

	return WorldLoc;
}

void AVRCanvas::CenterCanvas()
{
	FVector Loc = FVector(DistanceFromViewer, -(CanvasSizeX) / 2, (CanvasSizeY) / 2);

	SetActorRelativeLocation(Loc);
}

void AVRCanvas::ResizeCanvas(float X, float Y, float Z)
{
	CanvasSizeX = X;
	CanvasSizeY = Y;

	CanvasMesh->SetRelativeScale3D(FVector(1, CanvasSizeX / 10, CanvasSizeY / 10));

	DistanceFromViewer = Z;

	CenterCanvas();
}

void AVRCanvas::SetCanvasColor(FLinearColor NewColor)
{
	CanvasColor = NewColor;
	CanvasMat->SetVectorParameterValue("Color", CanvasColor);
}

void AVRCanvas::AddWidget(AVRWidget* AWidget, float X, float Y, FString Name)
{
	WidgetArray.Add(AWidget);

	AWidget->SetWidgetName(Name);
	AWidget->SetWidgetLocation(ConvertToWorldCoords(X, Y), X, Y);
	AWidget->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition);

	if (GetWidgetLayer("AlwaysVisible") != NULL)
		GetWidgetLayer("AlwaysVisible")->WidgetsInLayer.Add(AWidget);
}

void AVRCanvas::AddWidget(AVRWidget* AWidget, float X, float Y, FString Name, FString LayerName)
{
	WidgetArray.Add(AWidget);

	AWidget->SetWidgetName(Name);
	AWidget->SetWidgetLocation(ConvertToWorldCoords(X, Y), X, Y);
	AWidget->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition);

	if (LayerName.Compare("AlwaysVisible") == 0)
	{
		AlwaysVisibleLayer.WidgetsInLayer.Add(AWidget);
		return;
	}

	if (GetWidgetLayer(LayerName) != NULL)
	{
		FWidgetLayer* Layer = GetWidgetLayer(LayerName);
		Layer->WidgetsInLayer.Add(AWidget);
		AWidget->bWidgetVisibility = Layer->bLayerVisible;

	}
	else
		AlwaysVisibleLayer.WidgetsInLayer.Add(AWidget);
}

void AVRCanvas::MoveWidget(AVRWidget* AWidget, float X, float Y)
{
	AWidget->SetWidgetLocation(ConvertToWorldCoords(X, Y), X, Y);
}

AVRWidget* AVRCanvas::GetWidget(FString Name)
{
	AVRWidget* FoundWidget;

	for (int32 i = 0; i < WidgetArray.Num(); i++)
	{
		FoundWidget = WidgetArray[i];

		if (!WidgetArray[i])
			continue;

		if (!WidgetArray[i]->IsValidLowLevel())
			continue;

		if (FoundWidget->GetWidgetName() == Name)
			return FoundWidget;
	}

	return NULL;
}

void AVRCanvas::AddWidgetLayer(FString Name, bool bVisible)
{
	FWidgetLayer NewLayer;
	NewLayer.LayerName = Name;
	NewLayer.bLayerVisible = bVisible;

	WidgetLayers.Add(NewLayer);
}

FWidgetLayer* AVRCanvas::GetWidgetLayer(FString Name)
{
	for (auto Iter(WidgetLayers.CreateIterator()); Iter; Iter++)
	{
		if ((*Iter).LayerName.Compare(Name) == 0)
			return &(*Iter);
	}

	return NULL;
}

void AVRCanvas::SetWidgetLayerVisibility(FString LayerName, bool bValue)
{
	FWidgetLayer* Layer = GetWidgetLayer(LayerName);

	if (Layer != NULL)
	{
		for (auto Iter(Layer->WidgetsInLayer.CreateIterator()); Iter; Iter++)
		{
			if ((*Iter) != NULL)
			{
				(*Iter)->bWidgetVisibility = bValue;
			}
		}
	}

	Layer->bLayerVisible = bValue;
}

void AVRCanvas::SetWidgetLayerVisibility(FWidgetLayer &Layer, bool bValue)
{
	//bHidden doesnt work... need to hide mesh visibility :(

	for (auto Iter(Layer.WidgetsInLayer.CreateIterator()); Iter; Iter++)
	{
		if ((*Iter) != NULL)
		{
			(*Iter)->bWidgetVisibility = bValue;
		}
	}

	Layer.bLayerVisible = bValue;
}

void AVRCanvas::ToggleVisibleLayers()
{
	//To toggle through layers (view one layer at a time)
	//need to check if multiple are visible and clear them. Will need to iterate
	//twice, one to check for multiple visible layers, then again to toggle the layers.
	//not efficent, maybe should think more later. fairly
	//convoluted should probably divide into more functions

	bool bFirstVisibleFound = false;
	bool bClearLayers = false;

	for (auto Iter(WidgetLayers.CreateIterator()); Iter; Iter++)
	{
		if ((*Iter).bLayerVisible && bFirstVisibleFound)
		{
			bClearLayers = true;
			break;
		}
		else if ((*Iter).bLayerVisible)
		{
			bFirstVisibleFound = true;
		}
	}

	bool bSetNextLayerVisible = true;

	//Skips entirely if no need to clear layers
	if (bClearLayers)
	{
		for (auto Iter(WidgetLayers.CreateIterator()); Iter; Iter++)
		{
			if (bSetNextLayerVisible)
			{
				SetWidgetLayerVisibility((*Iter), true);
				bSetNextLayerVisible = false;
			}
			else
				SetWidgetLayerVisibility((*Iter), false);
		}
	}

	bSetNextLayerVisible = false;

	for (auto Iter(WidgetLayers.CreateIterator()); Iter; Iter++)
	{
		if (bSetNextLayerVisible)
		{
			SetWidgetLayerVisibility((*Iter), true);
			return;
		}
		
		if ((*Iter).bLayerVisible)
		{
			SetWidgetLayerVisibility((*Iter), false);
			bSetNextLayerVisible = true;
		}
	}

	//just using this to set first layer visible if last one was visible and got cleared
	for (auto Iter(WidgetLayers.CreateIterator()); Iter; Iter++)
	{
		SetWidgetLayerVisibility((*Iter), true);
		return;
	}
}

void AVRCanvas::ShowLayer(FWidgetLayer &Layer)
{
	HideAllLayers();

	SetWidgetLayerVisibility(Layer, true);
}

void AVRCanvas::HideAllLayers()
{
	for (auto Iter(WidgetLayers.CreateIterator()); Iter; Iter++)
	{
		SetWidgetLayerVisibility((*Iter), false);
	}
}

void AVRCanvas::BindWidgetToVisibility(AVRWidget* VisibilityWidget)
{
	if (Cast<AVRCheckbox>(VisibilityWidget))
	{
		VisibilityCheckbox = Cast<AVRCheckbox>(VisibilityWidget);
	}
}

void AVRCanvas::BindWidgetToRed(AVRWidget* RedWidget)
{
	if (Cast<AVRSlider>(RedWidget))
	{
		RedSlider = Cast<AVRSlider>(RedWidget);
	}
}

void AVRCanvas::BindWidgetToBlue(AVRWidget* BlueWidget)
{
	if (Cast<AVRSlider>(BlueWidget))
	{
		BlueSlider = Cast<AVRSlider>(BlueWidget);
	}
}

void AVRCanvas::BindWidgetToGreen(AVRWidget* GreenWidget)
{
	if (Cast<AVRSlider>(GreenWidget))
	{
		GreenSlider = Cast<AVRSlider>(GreenWidget);
	}
}

void AVRCanvas::BindWidgetToTransparency(AVRWidget* TransparencyWidget)
{
	if (Cast<AVRSlider>(TransparencyWidget))
	{
		TransparencySlider = Cast<AVRSlider>(TransparencyWidget);
	}
}

void AVRCanvas::BindWidgetToLayerToggle(AVRWidget* LayerButton)
{
	if (Cast<AVRButton>(LayerButton))
	{
		ToggleLayerButton = Cast<AVRButton>(LayerButton);
	}
}