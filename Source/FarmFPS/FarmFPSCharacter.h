// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// Brock
#include "Managers/ModifiedValueData.h"

// UE
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"

// Generated
#include "FarmFPSCharacter.generated.h"

class AConstantCropAffectorArea;
class AResourcePickupActor;

class UBoxComponent;
class UCameraComponent;
class UInputAction;
class UInputComponent;
class UPlayerInventoryItemSelector;
class UResourceInventory;
class USkeletalMeshComponent;
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
	
public:
	AFarmFPSCharacter();

	UFUNCTION(BlueprintPure)
	bool GetIsMeleeing() const { return _isMeleeing; }

	UFUNCTION(Exec)
	void AddResourcesBrock(FString resourceName, int amount);

	UFUNCTION(Exec)
	void StartDayBrock();

	UFUNCTION(Exec)
	void EndDayBrock();

	UFUNCTION(Exec)
	void StartStormBrock(FString resourceTag, bool isPermanent);

	UFUNCTION(Exec)
	void SpawnStormCloudBrock();

	UFUNCTION(Exec)
	void ResetToSpawnLocationBrock();

	UFUNCTION(Exec)
	void SpawnCustomerBrock();

	UFUNCTION(Exec)
	void SpawnGiantCustomerBrock();

	bool IsPickupInRangeOfPlayer(AResourcePickupActor* pickup) const;

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintPure)
	UResourceInventory* GetResourceInventory() const { return _inventory; }

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCropHealthShownEvent, bool showHealth);

	FOnCropHealthShownEvent OnShowCropHealth;

	bool IsShowingCropHealth() const { return _isShowingCropHealth; }

	void AddForeignMovement(const FVector2D& foreignMovementVector);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPlayerLanded(const FHitResult& HitResult);

	void TriggerFallSlam(const float velocityAtFall);

	UFUNCTION()
	void OnPerkLevelDataChanged(const FGameplayTag& perkType, const FPerkData& perkData);

	UFUNCTION()
	void OnGroundSlamComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnMeleeComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnResourcePickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoShowCropHealth();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoHideCropHealth();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoSelectInventoryItem(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnThrowStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnThrowStop();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void ThrowInventoryItem();

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

	void DoMeleeStart();
	void DoMeleeEnd();
	void DoGroundSlamStart();
	void DoGroundSlamEnd();

	UFUNCTION()
	void OnResourcePickupBonusTimeEnd();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void Cosmetic_OnItemSelectorIndexChanged(int index, FGameplayTag resourceType);

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	void OnScrollTimerEnd() { _canScroll = true; }

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ThrowInventoryItemAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SelectInventoryItemAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ShowCropHealth;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MeleeAction;

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
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UPerkManager* _perkManager = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UResourceInventory* _inventory = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UPlayerInventoryItemSelector* _itemSelector = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup Collider")
	FModifiedFloatValue _defaultPickupColliderRadius = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Ground Slam")
	FModifiedFloatValue _groundSlamDistanceThreshold;

	UPROPERTY(EditDefaultsOnly, Category = "Ground Slam")
	FModifiedFloatValue _groundSlamExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Ground Slam")
	float _groundSlamDashForce = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Ground Slam")
	float _timeToReachGround = .75f;

	UPROPERTY(EditDefaultsOnly, Category = "Fall Slam")
	float _velocityNeededToTriggerFallSlam = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fall Slam")
	float _maxFallSlamVelocity = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fall Slam")
	float _minFallSlamColliderMultipler = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fall Slam")
	float _maxFallSlamColliderMultipler = 15.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fall Slam")
	float _timeOfFallSlamColliderBonus = 1.f;

	UPROPERTY(EditAnywhere, Category = "Scroll")
	float _scrollRate = .1f;

	bool _canScroll = true;

	int _startingJumpCount = 0;
	float _startingJumpHeight = 0;

	bool _isMeleeing = false;

	FTimerHandle _meleeTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	FModifiedIntValue _meleeDamage = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	FVector _meleeColliderBounds = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	FModifiedFloatValue _meleeScale = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	FModifiedFloatValue _meleeDuration = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Ground Slam")
	FModifiedIntValue _groundSlamDamage = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Jump")
	FModifiedIntValue _extraJumpCount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Jump")
	FModifiedFloatValue _extraJumpHeight = 10.f;

	bool _isThrowingItems = false;

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	FModifiedFloatValue _throwForce = 1000.f;

	float _throwInterval = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	FModifiedFloatValue _throwIntervalSpeedUpPerThrow = 0.01;

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	float _maxThrowSpeedInterval = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	float _minThrowSpeedInterval = 0.01;

	FTimerHandle _throwTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Affectors")
	TSubclassOf<AConstantCropAffectorArea> _waterAffectorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Affectors")
	TSubclassOf<AConstantCropAffectorArea> _lightAffectorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FModifiedFloatValue _movementSpeedMultiplier = 1.f;

	float _startingMovementSpeed = 0;

	UPROPERTY(EditAnywhere)
	USphereComponent* _groundSlamSphereCollider;

	UPROPERTY(EditAnywhere)
	USphereComponent* _resourcePickupCollider;

	UPROPERTY(EditAnywhere)
	UBoxComponent* _meleeCollider;

	bool _startedGroundSlam = false;
	bool _isShowingCropHealth = false;

	FVector _spawnLocation = FVector::ZeroVector;
};

