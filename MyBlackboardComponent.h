// Copyright A.Putrino S.Chachkov.
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BehaviorTree/BlackboardComponent.h"
#include "MyBlackboardComponent.generated.h"

/**
 * 
 */
struct FMyKeyDelegate
{
	FBlackboard::FKey KeyID;
	FDelegateHandle DelegateHandle;
	FMyKeyDelegate(FBlackboard::FKey MyKeyID, FDelegateHandle MyDelegateHandle) :KeyID(MyKeyID), DelegateHandle(MyDelegateHandle){}
};
UCLASS()
class CRAZYRESTAURANT_API UMyBlackboardComponent : public UBlackboardComponent
{
	GENERATED_BODY()

public:

	bool PushBlackboard(UBlackboardData* NewAsset, TArray<struct FBlackboardKeySelector> &Params);


	UFUNCTION(BlueprintCallable, Category = BehaviorTree)
		void PopBlackboard();


	UBlackboardData* CopyBlackboardData(UBlackboardData* OldAsset);

	void CallNotifyObservers(FBlackboard::FKey KeyID);
	
private:
	TArray<UBlackboardData*> BBStack;
	TArray<struct FMyKeyDelegate> DelegateHandles;
	TArray<int32>ParamsNumStack;
};
