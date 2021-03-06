// Copyright A.Putrino S.Chachkov
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "CallBehaviorProj.h" // Replace with your project .h, idem for logs
#include "MyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyBlackboardComponent.h"


AMyAIController::AMyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


bool AMyAIController::RunBehaviorTree(UBehaviorTree* BTAsset)
{

	if (BTAsset == NULL)
	{
		UE_LOG(LogCallBehavior, Warning, TEXT("RunBehaviorTree: Unable to run NULL behavior tree"));
		return false;
	}

	bool bSuccess = true;
	bool bShouldInitializeBlackboard = false;

	// see if need a blackboard component at all
	UBlackboardComponent* BlackboardComp = NULL;
	if (BTAsset->BlackboardAsset)
	{
		//this is the only change wrt the overriden method
		bSuccess = MyUseBlackboard(BTAsset->BlackboardAsset);
		BlackboardComp = FindComponentByClass<UBlackboardComponent>();
	}

	if (bSuccess)
	{
		UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(BrainComponent);
		if (BTComp == NULL)
		{

			BrainComponent = BTComp = ConstructObject<UBehaviorTreeComponent>(UBehaviorTreeComponent::StaticClass(), this, TEXT("BTComponent"));
			BrainComponent->RegisterComponent();
		}

		check(BTComp != NULL);
		BTComp->StartTree(*BTAsset, EBTExecutionMode::Looped);
	}

	return bSuccess;
}

bool AMyAIController::MyUseBlackboard(UBlackboardData* BlackboardAsset)
{
	if (BlackboardAsset == NULL)
	{
		UE_LOG(LogCallBehavior, Log, TEXT("UseBlackboard: trying to use NULL Blackboard asset. Ignoring"));
		return false;
	}

	bool bSuccess = true;
	UBlackboardComponent* BlackboardComp = FindComponentByClass<UBlackboardComponent>();

	if (BlackboardComp == NULL)
	{
		//this is the only change wrt the original UseBlackboard method
		BlackboardComp = ConstructObject<UMyBlackboardComponent>(UMyBlackboardComponent::StaticClass(), this, TEXT("BlackboardComponent"));
		if (BlackboardComp != NULL)
		{
			InitializeBlackboard(*BlackboardComp, *BlackboardAsset);
			BlackboardComp->RegisterComponent();
		}

	}
	else if (BlackboardComp->GetBlackboardAsset() == NULL)
	{
		InitializeBlackboard(*BlackboardComp, *BlackboardAsset);
	}
	else if (BlackboardComp->GetBlackboardAsset() != BlackboardAsset)
	{
		UE_LOG(LogCallBehavior, Log, TEXT("UseBlackboard: requested blackboard %s while already has %s instantiated. Forcing new BB.")
			, *GetNameSafe(BlackboardAsset), *GetNameSafe(BlackboardComp->GetBlackboardAsset()));
		InitializeBlackboard(*BlackboardComp, *BlackboardAsset);
	}

	return bSuccess;
}
