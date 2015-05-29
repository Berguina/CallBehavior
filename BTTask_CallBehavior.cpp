// Copyright A.Putrino S.Chachkov
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "CallBehaviorProj.h" //Replace this with your project
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

		//push blackboard on the stack
		MyBlackboard->PushBlackboard(OriginalBlackboard, Params);

		//temporarely replace original blackboard by the copy
		//otherwise CacheSelectedKey willl not work properly
		BehaviorAsset->BlackboardAsset = MyBlackboard->GetBlackboardAsset();

	}

	//execute RunBehavior
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	//put original BB of the behavior function back
	BehaviorAsset->BlackboardAsset = OriginalBlackboard;

	return Result;
}
