// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerWallet.h"

UPlayerWallet::UPlayerWallet()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerWallet::AddMoney(uint16 moneyToAdd)
{
	SetMoney(_currentMoney + moneyToAdd);
}

void UPlayerWallet::RemoveMoney(uint16 moneyToRemove)
{
	SetMoney(_currentMoney - moneyToRemove);
}

void UPlayerWallet::SetMoney(uint16 newMoneyAmount)
{
	_currentMoney = newMoneyAmount;
}

void UPlayerWallet::BeginPlay()
{
	Super::BeginPlay();
}