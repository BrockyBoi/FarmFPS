// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated
#include "TutorialShineActor.generated.h"

class USpotLightComponent;

UENUM(BlueprintType)
enum class EShineSize : uint8
{
	Small, 
	Medium,
	Large
};

class UArrowComponent;

UCLASS()
class ATutorialShineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATutorialShineActor();

	void Show(const FVector& location, EShineSize shineSize);
	void FollowActor(AActor* actorToFollow, EShineSize shineSize);
	void Hide();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	float _heightOverObject = 1000.f;

	TWeakObjectPtr<AActor> _actorToFollow;

	USpotLightComponent* _spotLight = nullptr;
	UArrowComponent* _arrowComponent = nullptr;

	UPROPERTY(EditAnywhere)
	float _smallAngle = 15.f;

	UPROPERTY(EditAnywhere)
	float _mediumAngle = 35.f;

	UPROPERTY(EditAnywhere)
	float _largeAngle = 60.f;
};
