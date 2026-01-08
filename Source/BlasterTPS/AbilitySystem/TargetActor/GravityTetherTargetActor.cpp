#include "GravityTetherTargetActor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "BlasterTPS/Character/BlasterCharacter.h"

AGravityTetherTargetActor::AGravityTetherTargetActor()
{
	PrimaryActorTick.bCanEverTick = true;
	ShouldProduceTargetDataOnServer = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	SetRootComponent(SplineComponent);
}

void AGravityTetherTargetActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();

	if (ABlasterCharacter* BlasterChar = Cast<ABlasterCharacter>(SourceActor))
	{
		BlasterChar->AddTargetingMappingContext();
	}
}

void AGravityTetherTargetActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (SourceActor)
	{
		if (ABlasterCharacter* BlasterChar = Cast<ABlasterCharacter>(SourceActor))
		{
			BlasterChar->RemoveTargetingMappingContext();
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AGravityTetherTargetActor::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	
	// 这里可以简单的再次 Perform Trace 获取确切的 HitResult
	// 或者直接复用 Tick 中计算的结果
	// 为了简单，做一次最后的 Trace
	
	FHitResult HitResult;
	FVector StartLoc = GetStartLocation();
	FVector ViewLoc;
	FRotator ViewRot;
	PrimaryPC->GetPlayerViewPoint(ViewLoc, ViewRot);
	
	// 1. Raycast from Camera to find aim point
	FVector LookDir = ViewRot.Vector();
	FVector CameraTraceEnd = ViewLoc + LookDir * 50000.f; // Long range
	
	FHitResult CameraHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(SourceActor);
	Params.AddIgnoredActor(this);
	
	GetWorld()->LineTraceSingleByChannel(CameraHit, ViewLoc, CameraTraceEnd, ECC_Visibility, Params);
	
	FVector AimTargetObj = CameraHit.bBlockingHit ? CameraHit.Location : CameraTraceEnd;
	
	// 2. Calculate Launch Config
	FVector LaunchVelocity;
	float OverrideGravityZ = GetWorld()->GetGravityZ() * ProjectileGravityScale;
	if (!UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, LaunchVelocity, StartLoc, AimTargetObj, OverrideGravityZ, 0.5f))
	{
		FVector LaunchDir = (AimTargetObj - StartLoc).GetSafeNormal();
		LaunchVelocity = LaunchDir * ProjectileInitialSpeed;
	}
	UE_LOG(LogTemp, Warning, TEXT("targetactor suggest:vel(%f %f %f) spawn(%f %f %f), target(%f %f %f),gz(%f)"),
		LaunchVelocity.X,LaunchVelocity.Y, LaunchVelocity.Z, StartLoc.X,StartLoc.Y, StartLoc.Z,
		AimTargetObj.X,AimTargetObj.Y, AimTargetObj.Z, OverrideGravityZ
	);
	FPredictProjectilePathParams PathParams;
	PathParams.StartLocation = StartLoc;
	PathParams.LaunchVelocity = LaunchVelocity;
	PathParams.bTraceWithCollision = true;
	PathParams.ProjectileRadius = ProjectileRadius;
	PathParams.MaxSimTime = 5.0f;
	PathParams.bTraceComplex = false;
	PathParams.TraceChannel = ECC_Visibility;
	PathParams.ActorsToIgnore.Add(SourceActor);
	PathParams.DrawDebugType = EDrawDebugTrace::None; // No debug needed for final confirm
	PathParams.OverrideGravityZ = GetWorld()->GetGravityZ() * ProjectileGravityScale;
	
	FPredictProjectilePathResult PathResult;
	
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
	
	if (PathResult.HitResult.bBlockingHit)
	{
		HitResult = PathResult.HitResult;
	}
	else
	{
		HitResult.Location = PathResult.LastTraceDestination.Location;
	}
	// Explicitly set TraceStart to pass the StartLocation to the Ability
	HitResult.TraceStart = StartLoc;

	FGameplayAbilityTargetDataHandle Handle = StartLocation.MakeTargetDataHandleFromHitResult(OwningAbility, HitResult);
	TargetDataReadyDelegate.Broadcast(Handle);
}

void AGravityTetherTargetActor::CancelTargeting()
{
	CanceledDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
}

void AGravityTetherTargetActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!SourceActor || !PrimaryPC) return;

	FVector StartLoc = GetStartLocation();
	
	// 获取玩家视点
	FVector ViewLoc;
	FRotator ViewRot;
	PrimaryPC->GetPlayerViewPoint(ViewLoc, ViewRot);
	
	// 1. 摄像机射线检测获取瞄准点
	FVector LookDir = ViewRot.Vector();
	FVector CameraTraceEnd = ViewLoc + LookDir * 10000.f; 
	
	FHitResult CameraHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(SourceActor);
	Params.AddIgnoredActor(this);
	
	GetWorld()->LineTraceSingleByChannel(CameraHit, ViewLoc, CameraTraceEnd, ECC_Visibility, Params);
	
	FVector AimTargetObj = CameraHit.bBlockingHit ? CameraHit.Location : CameraTraceEnd;
	
	// 2. 计算发射方向和速度
	FVector LaunchVelocity;
	float OverrideGravityZ = GetWorld()->GetGravityZ() * ProjectileGravityScale;
	if (!UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, LaunchVelocity, StartLoc, AimTargetObj, OverrideGravityZ, 0.5f))
	{
		FVector LaunchDir = (AimTargetObj - StartLoc).GetSafeNormal();
		LaunchVelocity = LaunchDir * ProjectileInitialSpeed;
	}

	// 3. 预测弹道
	FPredictProjectilePathParams PathParams;
	PathParams.StartLocation = StartLoc;
	PathParams.LaunchVelocity = LaunchVelocity;
	PathParams.bTraceWithCollision = true;
	PathParams.ProjectileRadius = ProjectileRadius;
	PathParams.MaxSimTime = 2.0f; // 缩短一些也可以
	PathParams.bTraceComplex = false;
	PathParams.TraceChannel = ECC_Visibility; // 自定义
	PathParams.ActorsToIgnore.Add(SourceActor);
	PathParams.DrawDebugType = EDrawDebugTrace::None; // Use Spline or Debug
	// PathParams.DrawDebugType = EDS_Persistent; // Debug line for test
	PathParams.OverrideGravityZ = GetWorld()->GetGravityZ() * ProjectileGravityScale;
	PathParams.SimFrequency = 15.f; // 频率不用太高

	FPredictProjectilePathResult PathResult;
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);

	// 4. 更新样条线
	TArray<FVector> PathPoints;
	for (const auto& PointData : PathResult.PathData)
	{
		PathPoints.Add(PointData.Location);
	}
	UpdateSpline(PathPoints);
}

FVector AGravityTetherTargetActor::GetStartLocation() const
{
	if (!SourceActor) return FVector::ZeroVector;

	// 这里尽量与 Ability 中 SpawnActor 的位置保持一致
	FTransform SpawnTransform = SourceActor->GetActorTransform();
	FVector SpawnLocation = SpawnTransform.GetLocation() + (SpawnTransform.GetRotation().GetForwardVector() * 100.f);
	return SpawnLocation;
}

void AGravityTetherTargetActor::UpdateSpline(const TArray<FVector>& PathPoints)
{
	if (!SplineComponent) return;

	SplineComponent->ClearSplinePoints(false);
	for (const FVector& Point : PathPoints)
	{
		SplineComponent->AddSplinePoint(Point, ESplineCoordinateSpace::World, false);
	}
	SplineComponent->UpdateSpline();

	if (!SplineMesh) return;

	int32 NumPoints = PathPoints.Num();
	int32 NumSegments = NumPoints - 1;

	// Ensure pool size and visibility
	for (int32 i = 0; i < SplineMeshes.Num(); i++)
	{
		if (i < NumSegments)
		{
			SplineMeshes[i]->SetVisibility(true);
		}
		else
		{
			SplineMeshes[i]->SetVisibility(false);
		}
	}

	// Create new meshes if needed
	while (SplineMeshes.Num() < NumSegments)
	{
		USplineMeshComponent* NewMesh = NewObject<USplineMeshComponent>(this);
		NewMesh->SetStaticMesh(SplineMesh);

		// Register component early so GetNumMaterials works reliably
		NewMesh->SetMobility(EComponentMobility::Movable);
		NewMesh->RegisterComponent();
		NewMesh->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);
		NewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// IMPORTANT: Set forward axis if your mesh is aligned along X, Y or Z. usually X for beams.
		NewMesh->SetForwardAxis(ESplineMeshAxis::Z);
		if (SplineMeshMaterial)
		{
			NewMesh->SetMaterial(0, SplineMeshMaterial);
		}


		SplineMeshes.Add(NewMesh);
	}

	// Update positions and tangents for continuous curve
	for (int32 i = 0; i < NumSegments; i++)
	{
		USplineMeshComponent* Mesh = SplineMeshes[i];
		if (Mesh)
		{
			FVector StartPos, StartTan, EndPos, EndTan;
			SplineComponent->GetLocationAndTangentAtSplinePoint(i, StartPos, StartTan, ESplineCoordinateSpace::Local);
			SplineComponent->GetLocationAndTangentAtSplinePoint(i + 1, EndPos, EndTan, ESplineCoordinateSpace::Local);
			
			Mesh->SetStartAndEnd(StartPos, StartTan, EndPos, EndTan, true);
			
		}
	}
}

