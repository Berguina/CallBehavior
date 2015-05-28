// Copyright A.Putrino S.Chachkov
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "MyProject.h" //Replace this with your project
#include "BTTask_CallBehavior.h"
#include "MyBlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeManager.h"



UBTTask_CallBehavior::UBTTask_CallBehavior(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Call Behavior";
}

void UBTTask_CallBehavior::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	for (int32 Index = 0; Index < Params.Num(); Index++)
	{
		Params[Index].CacheSelectedKey(GetBlackboardAsset());
	}

}

EBTNodeResult::Type UBTTask_CallBehavior::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardData* OriginalBlackboard = BehaviorAsset->BlackboardAsset;


	UMyBlackboardComponent * MyBlackboard = Cast<UMyBlackboardComponent>(OwnerComp.GetBlackboardComponent());
	if (MyBlackboard != NULL)
	{
		MyBlackboard->PushBlackboard(OriginalBlackboard, Params);
		BehaviorAsset->BlackboardAsset = MyBlackboard->GetBlackboardAsset();
	}

	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	BehaviorAsset->BlackboardAsset = OriginalBlackboard;
	
	return Result;
}
