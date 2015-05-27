// Copyright A.Putrino S.Chachkov
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AIController.h"
#include "MyAIController.generated.h"

/**
 * AIController that should be used for bots which want to use Call Behavior node
 */

UCLASS()
class CRAZYRESTAURANT_API AMyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMyAIController(const FObjectInitializer& ObjectInitializer);
	
	/** Starts executing behavior tree. Need to override this to create my own bb component*/
  virtual bool RunBehaviorTree(UBehaviorTree* BTAsset) override;

	/** makes AI use specified BB asset. Create MyBBComponent */
	UFUNCTION(BlueprintCallable, Category = "AI")
	bool MyUseBlackboard(UBlackboardData* BlackboardAsset);
};
