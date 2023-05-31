// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AEquipment::AEquipment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEquipment::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AWeapon::AWeapon()
{
	//Type = EEquipmentType::Weapon;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bTraceHitEnabled)
	{
		FVector BoxLocation;
		FRotator BoxOrientation;
		GetBoxTransform(BoxLocation, BoxOrientation);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn),
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Destructible),
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic),
			UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic),
		};

		TArray<AActor*> ActorsToIgnore = {
			Cast<AActor>(this),
			Cast<AActor>(OwnerChar)
		};

		TArray<FHitResult> OutHits;

		EDrawDebugTrace::Type DebugTrace = (bEnableDebugDraw ?
			EDrawDebugTrace::ForDuration : EDrawDebugTrace::None);
		UKismetSystemLibrary::BoxTraceMultiForObjects(
			this,
			BoxPrevLocation,
			BoxLocation,
			BoxCollision.HalfSize,
			BoxOrientation,
			ObjectTypes,
			false,
			ActorsToIgnore,
			DebugTrace,
			OutHits,
			true
		);

		OnTraceHit.Broadcast(this, OutHits);
	}

#if ENABLE_DRAW_DEBUG
	if (bEnableDebugDraw)
	{
		FVector BoxLocation;
		FRotator BoxOrientation;
		GetBoxTransform(BoxLocation, BoxOrientation);

		DrawDebugBox(
			GetWorld(),
			BoxLocation,
			BoxCollision.HalfSize,
			BoxOrientation.Quaternion(),
			FColor::Red,
			false,
			-1.0f,
			0,
			1.2f);
	}
#endif
}

void AWeapon::EnableTraceHit(bool bEnable)
{
	bTraceHitEnabled = bEnable;

	if (bEnable)
	{
		const FTransform& Transform = GetTransform();
		BoxPrevLocation = Transform.TransformPosition((BoxCollision.Offset));
	}
}

void AWeapon::SetDebugDrawEnable(bool bEnable)
{
#if ENABLE_DRAW_DEBUG
	bEnableDebugDraw = bEnable;
#endif
}

void AWeapon::GetBoxTransform(FVector& OutLocation, FRotator& OutOrientation)
{
	const FTransform& Transform = GetTransform();
	OutLocation = Transform.TransformPosition((BoxCollision.Offset));
	OutOrientation = Transform.Rotator() + BoxCollision.Orientation;
}
