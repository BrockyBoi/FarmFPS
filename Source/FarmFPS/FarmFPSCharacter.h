// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// Brock
#include "ModifiedValueData.h"

// UE
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"

// Generated
#include "FarmFPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class AConstantCropAffectorArea;
class UInputAction;
class USphereComponent;
struct FInputActionValue;
struct FPerkData;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AFarmFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Ground Slam Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* GroundSlamAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SpawnWaterAffectorAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SpawnLightAffectorAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;

	class UPerkManager* _perkManager = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Ground Slam")
	FModifiedFloatValue _groundSlamDistanceThreshold;

	UPROPERTY(EditDefaultsOnly, Category = "Ground Slam")
	FModifiedFloatValue _groundSlamExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Ground Slam")
	float _groundSlamDashForce = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Ground Slam")
	float _timeToReachGround = .75f;

	int _startingJumpCount = 0;
	float _startingJumpHeight = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Jump")
	FModifiedIntValue _extraJumpCount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Jump")
	FModifiedFloatValue _extraJumpHeight = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Affectors")
	TSubclassOf<AConstantCropAffectorArea> _waterAffectorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Affectors")
	TSubclassOf<AConstantCropAffectorArea> _lightAffectorClass;

	UPROPERTY(EditAnywhere)
	USphereComponent* _groundSlamSphereCollider;

	bool _startedGroundSlam = false;
	
public:
	AFarmFPSCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPerkLevelDataChanged(const FGameplayTag& perkType, const FPerkData& perkData);

	UFUNCTION()
	void OnGroundSlamComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	void OnPressSpawnWaterAffector();
	void OnPressSpawnLightAffector();

	void DoGroundSlamStart();
	void DoGroundSlamEnd();

protected:
	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

public:
	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

