// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "MyBTTask_MoveTo.generated.h"

/**
 * 
 */
UCLASS()
class FUCKYEA_API UMyBTTask_MoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()

		bool M_bPathCalculated;
	UMyBTTask_MoveTo(const FObjectInitializer& ObjectInitializer);

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;


};
