// Copyright A.Putrino S.Chachkov
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "CallBehaviorProj.h" //Put here your project 
#include "MyBlackboardComponent.h"
#include "EngineUtils.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"


struct FBlackboardInitializationData
{
	FBlackboard::FKey KeyID;
	uint16 DataSize;

	FBlackboardInitializationData() {}
	FBlackboardInitializationData(FBlackboard::FKey InKeyID, uint16 InDataSize) : KeyID(InKeyID)
	{
		DataSize = (InDataSize <= 2) ? InDataSize : ((InDataSize + 3) & ~3);
	}

	struct FMemorySort
	{
		FORCEINLINE bool operator()(const FBlackboardInitializationData& A, const FBlackboardInitializationData& B) const
		{
			return A.DataSize > B.DataSize;
		}
	};
};


bool UMyBlackboardComponent::PushBlackboard(UBlackboardData* NewAsset, TArray<struct FBlackboardKeySelector> &Params)
{

	UAISystem* AISystem = UAISystem::GetCurrentSafe(GetWorld());
	if (AISystem == nullptr)
	{
		return false;
	}

	//makes a copy of the blackboard
	//otherwise the data in the bb can be owerwritten by other calls or other instances
	UBlackboardData* NewAssetCopy = CopyBlackboardData(NewAsset);

	//save current bb on stack
	BBStack.Add(BlackboardAsset);
	UBlackboardData* OldAsset = BlackboardAsset;

	//and replace it by new bb
	BlackboardAsset = NewAssetCopy;



	bool bSuccess = true;
	const int32 NumKeys = NewAssetCopy->Keys.Num();

	//make old bb parent of new bb
	NewAssetCopy->Parent = OldAsset;
	//update first key id accordingly
	NewAssetCopy->UpdateKeyIDs();


	TArray<FBlackboardInitializationData> InitList;

	InitList.Reserve(NumKeys);

	//grow ValueOffsets to contain new bb offsets
	ValueOffsets.AddZeroed(NumKeys);

	for (int32 KeyIndex = 0; KeyIndex < NumKeys; KeyIndex++)
	{
		if (NewAssetCopy->Keys[KeyIndex].KeyType)
		{
			InitList.Add(FBlackboardInitializationData(KeyIndex + NewAssetCopy->GetFirstKeyID(), NewAssetCopy->Keys[KeyIndex].KeyType->GetValueSize()));
		}
	}

	
	// sort key values by memory size, so they can be packed better
	// it still won't protect against structures, that are internally misaligned (-> uint8, uint32)
	// but since all Engine level keys are good... 
	InitList.Sort(FBlackboardInitializationData::FMemorySort());
	uint16 MemoryOffset = 0;
	for (int32 Index = 0; Index < InitList.Num(); Index++)
	{
		ValueOffsets[InitList[Index].KeyID] = MemoryOffset + ValueMemory.Num();
		MemoryOffset += InitList[Index].DataSize;
	}

	//grow ValueMemory to contain new bb data
	//(also allows memory for parameters, this is not really neaded, to improve...)
	ValueMemory.AddZeroed(MemoryOffset);

	// initialize memory
	for (int32 Index = 0; Index < InitList.Num(); Index++)
	{
		const FBlackboardEntry* KeyData = BlackboardAsset->GetKey(InitList[Index].KeyID);
		uint8* RawData = GetKeyRawData(InitList[Index].KeyID);

		KeyData->KeyType->Initialize(RawData);
	}

	int32 ParamIndex = 0;
	for (int32 KeyIndex = 0; KeyIndex < NumKeys && ParamIndex<Params.Num(); KeyIndex++)
	{
		//for each parameter
		if (NewAssetCopy->Keys[KeyIndex].KeyType&&NewAssetCopy->Keys[KeyIndex].EntryName.ToString().StartsWith("PARAM_"))
		{

			FBlackboard::FKey ParentKeyID = OldAsset->GetKeyID(Params[ParamIndex].SelectedKeyName);
			FBlackboard::FKey ChildKeyID = BlackboardAsset->GetKeyID(BlackboardAsset->Keys[KeyIndex].EntryName);
			if (ParentKeyID != FBlackboard::InvalidKey && ChildKeyID != FBlackboard::InvalidKey)
			{

				//redirect ValueOffset to the memory location of the actual argument
				ValueOffsets[ChildKeyID] = ValueOffsets[ParentKeyID];

				//add observer to propagate notifications to actual argument observers
				DelegateHandles.Add(FMyKeyDelegate(ChildKeyID, RegisterObserver(ChildKeyID, this, FOnBlackboardChange::CreateLambda([this, ParentKeyID](const UBlackboardComponent& BComp, FBlackboard::FKey ChangedKeyID){
					this->CallNotifyObservers(ParentKeyID);
				}))));

				ParamIndex++;
			}

		}

	}

	//saves number of parameters found
	ParamsNumStack.Add(ParamIndex);

	return bSuccess;
}

void UMyBlackboardComponent::CallNotifyObservers(FBlackboard::FKey KeyID)
{

	NotifyObservers(KeyID);

}

void UMyBlackboardComponent::PopBlackboard()
{
	//undoes PushBlackboard

	int32 ParamsNum = ParamsNumStack[ParamsNumStack.Num() - 1];
	ParamsNumStack.RemoveAt(ParamsNumStack.Num() - 1);
	if (ParamsNum > 0)
	{
		//removes observers
		for (int32 Index = DelegateHandles.Num() - ParamsNum; Index < DelegateHandles.Num(); Index++)
		{
			UnregisterObserver(DelegateHandles[Index].KeyID, DelegateHandles[Index].DelegateHandle);
		}

		DelegateHandles.RemoveAt(DelegateHandles.Num() - ParamsNum, ParamsNum);
	}

	//restores old bb
	UBlackboardData* OldAsset = BlackboardAsset;
	BlackboardAsset = BBStack[BBStack.Num() - 1];

	BBStack.RemoveAt(BBStack.Num() - 1);

	//shrinks ValueOffsets
	const int32 NumKeys = OldAsset->Keys.Num();
	ValueOffsets.RemoveAt(ValueOffsets.Num() - NumKeys, NumKeys);

	//shrinks ValueMemory
	uint16 MemoryOffset = 0;
	for (int32 KeyIndex = 0; KeyIndex < NumKeys; KeyIndex++)
	{
		if (OldAsset->Keys[KeyIndex].KeyType)
		{
			MemoryOffset += OldAsset->Keys[KeyIndex].KeyType->GetValueSize();
		}
	}
	ValueMemory.RemoveAt(ValueMemory.Num() - MemoryOffset, MemoryOffset);


}
UBlackboardData* UMyBlackboardComponent::CopyBlackboardData(UBlackboardData* OldAsset)
{
	UBlackboardData* NewAsset = NewObject<UBlackboardData>(OldAsset->GetOuter(), UBlackboardData::StaticClass());
	NewAsset->Keys.Reserve(OldAsset->Keys.Num());
	for (int32 KeyIndex = 0; KeyIndex < OldAsset->Keys.Num(); KeyIndex++)
	{
		NewAsset->Keys.Add(OldAsset->Keys[KeyIndex]);
		
	}

	NewAsset->Parent = NULL;
	return NewAsset;

}
