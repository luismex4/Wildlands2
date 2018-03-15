// Fill out your copyright notice in the Description page of Project Settings.

#include "FuckYea.h"
#include "MyBTTask_MoveTo.h"
#include "AISystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveVector.h"
#include "Net/UnrealNetwork.h"

UMyBTTask_MoveTo::UMyBTTask_MoveTo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	M_bPathCalculated = false;
}


void UMyBTTask_MoveTo::OnGameplayTaskActivated(UGameplayTask& Task)
{
	//M_bPathCalculated = false;
}

void UMyBTTask_MoveTo::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	
}

void UMyBTTask_MoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTMoveToTaskMemory* MyMemory = (FBTMoveToTaskMemory*)NodeMemory;
	if (!M_bPathCalculated && !OwnerComp.IsPaused())
	{
		AAIController* MyController = OwnerComp.GetAIOwner();
		if (MyController && !MyController->ShouldPostponePathUpdates())
		{
			MyMemory->bWaitingForPath = false;

			const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
			FBTMoveToTaskMemory* MyMemory = reinterpret_cast<FBTMoveToTaskMemory*>(NodeMemory);
			AAIController* MyController = OwnerComp.GetAIOwner();

			EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;
			if (MyController && MyBlackboard)
			{
				FAIMoveRequest MoveReq;
				MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : MyController->GetDefaultNavigationFilterClass());
				MoveReq.SetAllowPartialPath(bAllowPartialPath);
				MoveReq.SetAcceptanceRadius(AcceptableRadius);
				MoveReq.SetCanStrafe(bAllowStrafe);
				MoveReq.SetReachTestIncludesAgentRadius(bStopOnOverlap);
				MoveReq.SetProjectGoalLocation(bProjectGoalLocation);
				MoveReq.SetUsePathfinding(bUsePathfinding);

				if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
				{
					UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
					AActor* TargetActor = Cast<AActor>(KeyValue);
					if (TargetActor)
					{
						if (bTrackMovingGoal)
						{
							MoveReq.SetGoalActor(TargetActor);
						}
						else
						{
							MoveReq.SetGoalLocation(TargetActor->GetActorLocation());
						}
					}

				}
				else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
				{
					const FVector TargetLocation = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
					MoveReq.SetGoalLocation(TargetLocation);

					MyMemory->PreviousGoalLocation = TargetLocation;
				}

				if (MoveReq.IsValid())
				{

						FPathFollowingRequestResult RequestResult = MyController->MoveTo(MoveReq);
						if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
						{
							MyMemory->MoveRequestID = RequestResult.MoveId;
							WaitForMessage(OwnerComp, UBrainComponent::AIMessage_MoveFinished, RequestResult.MoveId);
							WaitForMessage(OwnerComp, UBrainComponent::AIMessage_RepathFailed);

							NodeResult = EBTNodeResult::InProgress;
						}
						else if (RequestResult.Code == EPathFollowingRequestResult::AlreadyAtGoal)
						{
							NodeResult = EBTNodeResult::Succeeded;
						}
						else if (RequestResult.Code == EPathFollowingRequestResult::Failed)
						{
							M_bPathCalculated = false;
						}
				}
			}

			FinishLatentTask(OwnerComp, NodeResult);	
		}
	}
}

