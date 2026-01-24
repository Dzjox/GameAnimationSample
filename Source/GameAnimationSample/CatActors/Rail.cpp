// Fill out your copyright notice in the Description page of Project Settings.


#include "Rail.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "../CatComponents/RailRidingComponent.h"

ARail::ARail()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(Root);
	Spline->SetMobility(EComponentMobility::Movable);

	StaticMesh_Example = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh_Example"));
	StaticMesh_Example->SetupAttachment(Root);

	BoxCollision_Example = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision_Example"));
	BoxCollision_Example->SetupAttachment(Root);
}

void ARail::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RebuildRailFromExamples();
}

void ARail::ClearGeneratedExamples()
{
	TArray<UActorComponent*> Components = GetComponents().Array();
	for (UActorComponent* Comp : Components)
	{
		if (!Comp)
		{
			continue;
		}
		if (Comp->ComponentTags.Contains(GeneratedTag))
		{
			Comp->DestroyComponent();
		}
	}
}

void ARail::RebuildRailFromExamples()
{
	if (!Spline || SpawnPeriod <= 0.f)
	{
		return;
	}

	ClearGeneratedExamples();

	float SplineLength = Spline->GetSplineLength();
	if (SplineLength <= 0.f)
	{
		return;
	}

	bool bHasMeshTemplate = (StaticMesh_Example && StaticMesh_Example->GetStaticMesh());
	bool bHasBoxTemplate = (BoxCollision_Example != nullptr);

	for (float Dist = 0.f; Dist <= SplineLength + KINDA_SMALL_NUMBER; Dist += SpawnPeriod)
	{
		FVector Location = Spline->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);
		FRotator Rotation = Spline->GetRotationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::World);

		if (bHasMeshTemplate)
		{
			UStaticMeshComponent* NewSM = NewObject<UStaticMeshComponent>(this);
			NewSM->SetStaticMesh(StaticMesh_Example->GetStaticMesh());
			NewSM->SetWorldLocation(Location);
			NewSM->SetWorldRotation(Rotation);
			NewSM->SetMobility(StaticMesh_Example->Mobility);
			NewSM->SetRelativeScale3D(StaticMesh_Example->GetRelativeScale3D());
			NewSM->SetCollisionEnabled(StaticMesh_Example->GetCollisionEnabled());
			NewSM->SetCollisionProfileName(StaticMesh_Example->GetCollisionProfileName());
			NewSM->ComponentTags = StaticMesh_Example->ComponentTags;
			NewSM->ComponentTags.Add(GeneratedTag);
			NewSM->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
			NewSM->RegisterComponent();
		}

		if (bHasBoxTemplate)
		{
			UBoxComponent* NewBox = NewObject<UBoxComponent>(this);
			FVector BoxExtent = BoxCollision_Example->GetUnscaledBoxExtent();
			NewBox->SetBoxExtent(BoxExtent);
			NewBox->SetWorldLocation(Location);
			NewBox->SetWorldRotation(Rotation);
			NewBox->SetMobility(BoxCollision_Example->Mobility);
			NewBox->SetCollisionEnabled(BoxCollision_Example->GetCollisionEnabled());
			NewBox->SetCollisionProfileName(BoxCollision_Example->GetCollisionProfileName());
			NewBox->ComponentTags = BoxCollision_Example->ComponentTags;
			NewBox->ComponentTags.Add(GeneratedTag);
			NewBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
			NewBox->RegisterComponent();
			NewBox->OnComponentBeginOverlap.AddDynamic(this, &ARail::OnTriggerBeginOverlap);
		}
	}
}

void ARail::AddRider(ACharacter* Character, float StartDistance, float Speed)
{
	if (!Character) return;

	TArray<URailRidingComponent*> FoundComps;
	Character->GetComponents<URailRidingComponent>(FoundComps);

	for (URailRidingComponent* Comp : FoundComps)
	{
		if (Comp && Comp->IsRegistered())
		{
			Comp->StopRide();
		}
	}

	URailRidingComponent* NewComp = NewObject<URailRidingComponent>(Character);
	if (!NewComp) return;

	NewComp->RegisterComponent();
	NewComp->StartRide(this, StartDistance, Speed > 0.f ? Speed : DefaultSpeed);
}

void ARail::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                  bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	ACharacter* Char = Cast<ACharacter>(OtherActor);
	if (!Char)
	{
		return;
	}

	if (!Spline)
	{
		return;
	}

	const FVector CharLoc = Char->GetActorLocation();
	const float InputKey = Spline->FindInputKeyClosestToWorldLocation(CharLoc);
	const float StartDistance = Spline->GetDistanceAlongSplineAtSplineInputKey(InputKey);

	AddRider(Char, StartDistance, DefaultSpeed);
}
