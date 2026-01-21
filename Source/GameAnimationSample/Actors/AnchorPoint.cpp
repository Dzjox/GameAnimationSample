// Fill out your copyright notice in the Description page of Project Settings.


#include "AnchorPoint.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "../Components/HookComponent.h"

AAnchorPoint::AAnchorPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(Root);
	DetectionSphere->InitSphereRadius(DetectionRadius);
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAnchorPoint::OnDetectionBeginOverlap);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AAnchorPoint::OnDetectionEndOverlap);
}

void AAnchorPoint::OnDetectionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!OtherActor) return;
	UHookComponent* HookComp = OtherActor->FindComponentByClass<UHookComponent>();
	if (HookComp)
	{
		HookComp->RegisterNearbyAnchor(this);
		UE_LOG(LogTemp, Display, TEXT("AnchorPoint::OnDetectionBeginOverlap"));
	}
}

void AAnchorPoint::OnDetectionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;
	UHookComponent* HookComp = OtherActor->FindComponentByClass<UHookComponent>();
	if (HookComp)
	{
		HookComp->UnregisterNearbyAnchor(this);
		UE_LOG(LogTemp, Display, TEXT("AnchorPoint::OnDetectionEndOverlap"));
	}
}

void AAnchorPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DetectionSphere)
	{
		TArray<AActor*> Overlapping;
		DetectionSphere->GetOverlappingActors(Overlapping);
		for (AActor* Actor : Overlapping)
		{
			if (!Actor) continue;
			UHookComponent* HookComp = Actor->FindComponentByClass<UHookComponent>();
			if (HookComp)
			{
				HookComp->UnregisterNearbyAnchor(this);
			}
		}
	}
	Super::EndPlay(EndPlayReason);
}
