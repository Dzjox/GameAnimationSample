// Fill out your copyright notice in the Description page of Project Settings.


#include "AnchorPoint.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "UObject/UnrealType.h"
#include "Engine/World.h"
#include "../CatComponents/HookComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"

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

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(Root);
	Arrow->ArrowSize = 1.2f;

	TrajectorySpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrajectorySpline"));
	TrajectorySpline->SetupAttachment(Root);
	TrajectorySpline->SetMobility(EComponentMobility::Movable);

	LaunchSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LaunchSphere"));
	LaunchSphere->SetupAttachment(Root);
	LaunchSphere->InitSphereRadius(ArrivalRadius);
	LaunchSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LaunchSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	LaunchSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	LaunchSphere->SetGenerateOverlapEvents(true);
	LaunchSphere->OnComponentBeginOverlap.AddDynamic(this, &AAnchorPoint::OnLaunchSphereBeginOverlap);
}

void AAnchorPoint::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateDetectionSphereRadius();
	UpdateLaunchSphereRadius();
#if WITH_EDITOR
	if (bShowTrajectory)
	{
		UpdateTrajectorySpline();
		UpdateArrowDirection();
	}
#endif
}

void AAnchorPoint::UpdateDetectionSphereRadius()
{
	if (DetectionSphere)
	{
		DetectionSphere->SetSphereRadius(DetectionRadius, true);
	}
}

void AAnchorPoint::UpdateLaunchSphereRadius()
{
	if (LaunchSphere)
	{
		LaunchSphere->SetSphereRadius(ArrivalRadius, true);
	}
}

void AAnchorPoint::UpdateTrajectorySpline()
{
	if (!TrajectorySpline || TrajectoryPoints < 2)
	{
		return;
	}

	FVector Start = GetActorLocation();
	FRotator StartRotator = GetActorRotation() + LaunchDirection;
	FVector Dir = StartRotator.Vector().GetSafeNormal();
	FVector InitialVel = Dir * LaunchStrength;
	float GravityZ = GetWorld() ? GetWorld()->GetGravityZ() : -980.f;

	TrajectorySpline->ClearSplinePoints(false);

	for (int32 i = 0; i < TrajectoryPoints; ++i)
	{
		float t = i * TrajectoryTimeStep;
		FVector Pos = Start + InitialVel * t + 0.5f * FVector(0.f, 0.f, GravityZ) * t * t;
		TrajectorySpline->AddSplinePoint(Pos, ESplineCoordinateSpace::World, false);
	}

	TrajectorySpline->UpdateSpline();
}

void AAnchorPoint::UpdateArrowDirection()
{
	if (Arrow)
	{
		Arrow->SetWorldRotation(LaunchDirection + GetActorRotation());
	}
}

void AAnchorPoint::OnLaunchSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor && OtherComp) OtherActor = OtherComp->GetOwner();
	if (!OtherActor) return;

	ACharacter* OverlappedChar = Cast<ACharacter>(OtherActor);
	if (!OverlappedChar) return;

	UHookComponent* Hook = OverlappedChar->FindComponentByClass<UHookComponent>();
	if (Hook)
	{
		Hook->NotifyAnchorReached(this);
	}
}

void AAnchorPoint::LaunchCharacterFromAnchor(ACharacter* Character, float PreservedSpeed)
{
	if (!Character) return;

	FVector LaunchDir = FVector::ZeroVector;

	FRotator AnchorWorldRot = GetActorRotation() + LaunchDirection;
	LaunchDir = AnchorWorldRot.Vector().GetSafeNormal();

	if (LaunchDir.IsNearlyZero())
	{
		FVector FromAnchor = GetActorLocation() - Character->GetActorLocation();
		LaunchDir = FromAnchor.GetSafeNormal();
	}

	if (LaunchDir.IsNearlyZero())
	{
		LaunchDir = Character->GetActorForwardVector();
	}

	float ForwardSpeed = (PreservedSpeed > KINDA_SMALL_NUMBER) ? PreservedSpeed : LaunchStrength;
	FVector LaunchVel = LaunchDir * ForwardSpeed + FVector::UpVector * LaunchUpwardStrength;
	Character->LaunchCharacter(LaunchVel, true, true);
}

void AAnchorPoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateDetectionSphereRadius();
	UpdateLaunchSphereRadius();
#if WITH_EDITOR
	UpdateArrowDirection();
	if (bShowTrajectory)
	{
		UpdateTrajectorySpline();
	}
#endif
}
