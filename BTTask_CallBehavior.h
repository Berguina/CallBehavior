// Copyright A.Putrino S.Chachkov
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_RunBehavior.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_CallBehavior.generated.h"

/**
*
*/
UCLASS()
class CRAZYRESTAURANT_API UBTTask_CallBehavior : public UBTTask_RunBehavior
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(Category = Blackboard, EditAnywhere)
		TArray<struct FBlackboardKeySelector> Params;


	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
