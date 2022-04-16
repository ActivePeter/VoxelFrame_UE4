// Copyright Epic Games, Inc. All Rights Reserved.

//#include "VoxelFrameUE4Character.h"
#include "main_player_Character.h"

#include "VoxelFrameUE4Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId


#include "./game/GameContext.h"
#include "main_player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameContext.h"
//#include "./game/ecs/Physic.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AVoxelFrameUE4Character

void AVoxelFrameUE4Character::first_position_set(FVector&& pos)
{
	this->SetActorLocation(
		pos
	);
	this->GetCharacterMovement()->GravityScale = 1;
	VF_Log("main player first_position_set");
}

AVoxelFrameUE4Character::AVoxelFrameUE4Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.


	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AVoxelFrameUE4Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	///////////////////////////////////////////////////
	/// 关闭重力
	///////////////////////////////////////////////////
	this->GetCharacterMovement()->GravityScale = 0;
	///////////////////////////////////////////////////
	/// 获取world
	///////////////////////////////////////////////////
	auto classToFind = AWorldActor::StaticClass();
	TArray<AActor*> foundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundEnemies);
	if (foundEnemies.Num() == 0)
	{
		VF_LogWarning("weird main character isnt loaded");
		return;
	}
	else
	{
		VF_LogWarning("player worldactor get");
	}
	world = Cast<AWorldActor>(foundEnemies[0]);
	//world = //Cast<AWorldActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AWorldActor::StaticClass()));

	///////////////////////////////////////////////////
	/// ecs
	///////////////////////////////////////////////////
	VF::_main_player::init(*world->context, this);


	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
}


void AVoxelFrameUE4Character::checkPlayerMoved()
{
	auto vfpos = FVector(GetActorLocation().X, GetActorLocation().Z, GetActorLocation().Y) / VF_WorldScale;
	if ((vfpos - lastPos).Size() > 0.001)
	{
		//VF_LogWarning("player moved");

		//最后记录上一次坐标
		lastPos = vfpos;
	}
	else
	{
		//VF_LogWarning("last distance %f", (vfpos - lastPos).Size());
	}
}

void AVoxelFrameUE4Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (world)
	{
		this->checkPlayerMoved();
		//world->context->chunkManager->checkPlayerChunkPosChange(
			//FVector(GetActorLocation().X, GetActorLocation().Z, GetActorLocation().Y) / VF_WorldScale);
		//VF::_Physic::sys_checkPlayerRay2Chunk(this);

	}
	else
	{
		VF_LogWarning("WorldActor not loaded for player");
		world = Cast<AWorldActor>(GetParentActor());
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AVoxelFrameUE4Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AVoxelFrameUE4Character::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AVoxelFrameUE4Character::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AVoxelFrameUE4Character::OnFire);
	PlayerInputComponent->BindAction("MouseRight", IE_Pressed, this, &AVoxelFrameUE4Character::OnRight);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AVoxelFrameUE4Character::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AVoxelFrameUE4Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVoxelFrameUE4Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AVoxelFrameUE4Character::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &AVoxelFrameUE4Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AVoxelFrameUE4Character::LookUpAtRate);
}
void AVoxelFrameUE4Character::OnRight()
{
	if (this->world->context->type == VF::ClientType::Player)
	{
		this->world->context->block_preview_manager->putBlock();
	}
}

void AVoxelFrameUE4Character::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AVoxelFrameUE4Projectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AVoxelFrameUE4Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

				this->world->context->block_preview_manager->destroyBlock();
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AVoxelFrameUE4Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AVoxelFrameUE4Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AVoxelFrameUE4Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AVoxelFrameUE4Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}
//struct ABC {};
void AVoxelFrameUE4Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);

		VF::_main_player::NetSyncData* net_sync_data;
		auto ok = this->world->context->ecs.scene->randomAccessEntity(this->ecsId, net_sync_data);
		assert(ok);
		if (ok)
		{
			net_sync_data->move_forward = Value;
		}
	}
}

void AVoxelFrameUE4Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);

		VF::_main_player::NetSyncData* net_sync_data;
		auto ok = this->world->context->ecs.scene->randomAccessEntity(this->ecsId, net_sync_data);
		assert(ok);
		if (ok)
		{
			net_sync_data->move_right = Value;
		}
	}
}

void AVoxelFrameUE4Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	//VF_LogWarning("player turnning");
	/*if (world->context.get()->type == VF::ClientType::Player)
	{
		auto a = VF::_main_player::get_ControlMovement(this->world->context.get());
		if (a)
		{
			a->rotate = true;
		}
		else
		{
			VF_LogWarning("fatal error, no player comp get");
		}
	}*/
}

void AVoxelFrameUE4Character::Turn(float val)
{
	if (fabsf(val) > VF_RoughEpsilon)
	{
		AddControllerYawInput(val);

		VF::_main_player::NetSyncData* net_sync_data;
		auto ok = this->world->context->ecs.scene->randomAccessEntity(this->ecsId, net_sync_data);
		assert(ok);
		if (ok)
		{
			net_sync_data->rotated = true;
		}
	}
}

void AVoxelFrameUE4Character::Jump()
{
	using VF::_main_player::NetSyncData;
	Super::Jump();
	NetSyncData* net_sync_data;
	auto ok = this->world->context->ecs.scene->randomAccessEntity(this->ecsId, net_sync_data);
	assert(ok);
	if (ok)
	{
		net_sync_data->jump_cmd = NetSyncData::JumpCmd::Jump;
	}
}
void AVoxelFrameUE4Character::StopJumping()
{
	using VF::_main_player::NetSyncData;
	Super::StopJumping();
	NetSyncData* net_sync_data;
	auto ok = this->world->context->ecs.scene->randomAccessEntity(this->ecsId, net_sync_data);
	assert(ok);
	if (ok)
	{
		net_sync_data->jump_cmd = NetSyncData::JumpCmd::StopJump;
	}
}

void AVoxelFrameUE4Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AVoxelFrameUE4Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AVoxelFrameUE4Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AVoxelFrameUE4Character::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AVoxelFrameUE4Character::TouchUpdate);
		return true;
	}

	return false;
}
