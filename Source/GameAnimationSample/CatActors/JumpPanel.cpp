// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPanel.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

AJumpPanel::AJumpPanel()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// use mesh as trigger
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionObjectType(ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AJumpPanel::OnMeshBeginOverlap);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);
	Arrow->ArrowSize = 1.5f;

	TrajectorySpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrajectorySpline"));
	TrajectorySpline->SetupAttachment(RootComponent);
	TrajectorySpline->SetMobility(EComponentMobility::Movable);
}

void AJumpPanel::BeginPlay()
{
	Super::BeginPlay();
}

void AJumpPanel::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
#if WITH_EDITOR
	if (bShowTrajectory)
	{
		UpdateTrajectorySpline();
		UpdateArrowDirection();
	}
#endif
}

void AJumpPanel::UpdateTrajectorySpline()
{
	if (!TrajectorySpline || TrajectoryPoints < 2)
	{
		return;
	}

	FVector Start = GetActorLocation();
	FRotator StartRotator = GetActorRotation()+LaunchDirection;
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

void AJumpPanel::UpdateArrowDirection()
{
	Arrow->SetWorldRotation(LaunchDirection+GetActorRotation());
}

void AJumpPanel::OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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

	FRotator StartRotator = GetActorRotation()+LaunchDirection;
	FVector Dir = StartRotator.Vector().GetSafeNormal();
	FVector LaunchVel = Dir * LaunchStrength;
	LaunchVel.Z *= LaunchZMultiplier;

	Char->LaunchCharacter(LaunchVel, true, true);
}
