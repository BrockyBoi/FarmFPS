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

const FGameplayTag& UCustomerSpawnerManager::GetNextCustomerTypeToSpawn() const
{
	float randomValue = FMath::RandRange(0.f, 1.f);

	if (_giantCustomerSpawnData.SpawnChance.GetModifiedValue(this) >= randomValue)
	{
		return CustomerTypeTags::GiantCustomer;
	}

	return CustomerTypeTags::StandardCustomer;
}

const TSubclassOf<ACustomer> UCustomerSpawnerManager::GetNextCustomerSpawnClass(const FGameplayTag& customerType)
{
	if (customerType.MatchesTag(CustomerTypeTags::GiantCustomer))
	{
		return _giantCustomerSpawnData.CustomerClass;
	}

	if (customerType.MatchesTag(CustomerTypeTags::StandardCustomer))
	{
		return _defaultCustomerSpawnData.CustomerClass;
	}

	return TSubclassOf<ACustomer>();
}

void UCustomerSpawnerManager::AttemptSpawnCustomer()
{
	if (IsRoomForNewCustomer())
	{
		TSubclassOf<ACustomer> customerClass = GetNextCustomerSpawnClass(GetNextCustomerTypeToSpawn());
		if (ensure(IsValid(customerClass)))
		{
			FActorSpawnParameters spawnParams;
			ACustomer* customer = GetWorld()->SpawnActor<ACustomer>(customerClass, _customerSpawnPoint, FRotator::ZeroRotator, spawnParams);
			if (ensure(IsValid(customer)))
			{
				_currentCustomersOnScreen++;
			}
		}
	}

	if (ensure(IsValid(GetWorld())))
	{
		GetWorld()->GetTimerManager().ClearTimer(_spawnTimer);
		GetWorld()->GetTimerManager().SetTimer(_spawnTimer, this, &UCustomerSpawnerManager::AttemptSpawnCustomer, _spawnRate.GetModifiedValue(this), true);
	}
}
