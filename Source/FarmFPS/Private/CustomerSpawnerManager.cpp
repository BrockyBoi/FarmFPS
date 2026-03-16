// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomerSpawnerManager.h"

// Brock
#include "Customer.h"

UCustomerSpawnerManager::UCustomerSpawnerManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UCustomerSpawnerManager::IsRoomForNewCustomer() const
{
	return _currentCustomersOnScreen < _totalCustomersAllowedOnScreenAtOnce.GetModifiedValue(this);
}

void UCustomerSpawnerManager::OnCustomerLeaveMap()
{
	bool wasRoomForNewCustomer = IsRoomForNewCustomer();
	_currentCustomersOnScreen--;

	//if (!wasRoomForNewCustomer)
	//{
	//	AttemptSpawnCustomer();
	//}
}

void UCustomerSpawnerManager::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(GetWorld())))
	{
		GetWorld()->GetTimerManager().SetTimer(_spawnTimer, this, &UCustomerSpawnerManager::AttemptSpawnCustomer, _spawnRate.GetModifiedValue(this), true);
	}
}

void UCustomerSpawnerManager::AttemptSpawnCustomer()
{
	if (IsRoomForNewCustomer())
	{
		_currentCustomersOnScreen++;

		FActorSpawnParameters spawnParams;
		//spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ACustomer* customer = GetWorld()->SpawnActor<ACustomer>(_customerToSpawn, _customerSpawnPoint, FRotator::ZeroRotator, spawnParams);
		if (ensure(IsValid(customer)))
		{
		}
	}
}
