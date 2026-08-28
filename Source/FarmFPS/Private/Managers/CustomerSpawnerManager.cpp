// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomerSpawnerManager.h"

// Brock
#include "Characters/Customer.h"
#include "Characters/CustomerTypeTag.h"
#include "Managers/DayNightCycleManager.h"
#include "Managers/FarmFPSUtilities.h"
#include "Resources/ResourceTypeTag.h"

UCustomerSpawnerManager::UCustomerSpawnerManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	_allowedBreadTypesDesired.AddTag(ResourceTypeTag::Bread);
}

bool UCustomerSpawnerManager::IsRoomForNewCustomer() const
{
	return _currentCustomersOnScreen < _totalCustomersAllowedOnScreenAtOnce.GetModifiedValue(this);
}

bool UCustomerSpawnerManager::IsSpawnTimerActive() const
{
	return ensure(IsValid(GetWorld())) ? GetWorld()->GetTimerManager().IsTimerActive(_spawnTimer) : false;
}

void UCustomerSpawnerManager::SpawnCustomer_CHEAT()
{
	TSubclassOf<ACustomer> customerClass = GetNextCustomerSpawnClass(CustomerTypeTag::StandardCustomer);
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

void UCustomerSpawnerManager::SpawnGiantCustomer_CHEAT()
{
	TSubclassOf<ACustomer> customerClass = GetNextCustomerSpawnClass(CustomerTypeTag::GiantCustomer);
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

	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (ensure(IsValid(dayNightCycle)))
	{
		dayNightCycle->OnDayBegin.AddUObject(this, &UCustomerSpawnerManager::OnDayBegin);
		dayNightCycle->OnDayEnd.AddUObject(this, &UCustomerSpawnerManager::OnDayEnd);
	}
}

void UCustomerSpawnerManager::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	UDayNightCycleManager* dayNightCycle = UFarmFPSUtilities::GetDayNightCycleManager(this);
	if (IsValid(dayNightCycle))
	{
		dayNightCycle->OnDayBegin.RemoveAll(this);
		dayNightCycle->OnDayEnd.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

const FGameplayTag UCustomerSpawnerManager::GetNextCustomerTypeToSpawn() const
{
	float randomValue = FMath::RandRange(0.f, 1.f);

	if (_giantCustomerSpawnData.SpawnChance.GetModifiedValue(this) >= randomValue)
	{
		return CustomerTypeTag::GiantCustomer;
	}

	return CustomerTypeTag::StandardCustomer;
}

const TSubclassOf<ACustomer> UCustomerSpawnerManager::GetNextCustomerSpawnClass(const FGameplayTag& customerType)
{
	if (customerType.MatchesTag(CustomerTypeTag::GiantCustomer))
	{
		return _giantCustomerSpawnData.CustomerClass;
	}

	if (customerType.MatchesTag(CustomerTypeTag::StandardCustomer))
	{
		return _defaultCustomerSpawnData.CustomerClass;
	}

	return TSubclassOf<ACustomer>();
}

void UCustomerSpawnerManager::OnDayBegin()
{
	if (ensure(IsValid(GetWorld())) && !IsSpawnTimerActive())
	{
		GetWorld()->GetTimerManager().SetTimer(_spawnTimer, this, &UCustomerSpawnerManager::AttemptSpawnCustomer, _spawnRate.GetModifiedValue(this), true);
	}
}

void UCustomerSpawnerManager::OnDayEnd()
{
	if (ensure(IsValid(GetWorld())))
	{
		GetWorld()->GetTimerManager().ClearTimer(_spawnTimer);
	}
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
