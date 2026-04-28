// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated
#include "CustomerQueue.generated.h"

class ACustomer;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCustomerQueue : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCustomerQueue();

	void AddCustomerToQueue(ACustomer* Customer);
	void RemoveCustomerFromQueue(ACustomer* Customer);
	void RemoveCustomerFromFrontOfQueue();
	const FVector GetCustomerQueuePosition(const ACustomer* searchingCustomer) const;
	const FVector GetQueuePositionAtIndex(int index) const;
	const FVector GetEndQueuePosition() const;
	bool IsAtFrontOfQueue(const ACustomer* Customer) const;

	ACustomer* GetFrontOfQueue() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TArray<TWeakObjectPtr<ACustomer>> _customerQueue;

	UPROPERTY(BlueprintReadWrite)
	USceneComponent* _queueStartPoint;

	UPROPERTY(EditDefaultsOnly)
	float _customerSpacing = 50.f;

	UPROPERTY(EditDefaultsOnly)
	FVector _queueDirection = FVector(1.f, 0.f, 0.f);
};
