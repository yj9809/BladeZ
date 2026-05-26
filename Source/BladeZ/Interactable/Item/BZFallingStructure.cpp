// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Item/BZFallingStructure.h"
#include "Components/BoxComponent.h"
#include "Character/Player/BZPlayerCharacter.h"
#include "Character/Enemy/Zombie/BZZombie.h"
#include "Engine/OverlapResult.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABZFallingStructure::ABZFallingStructure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponents"));
	RootComponent = DummyRoot;
	
	// 플레이어 캐릭터 감지용 트리거 볼룸 컴포넌트 생성 및 조립
	ActivationTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationTrigger"));
	ActivationTrigger->SetupAttachment(RootComponent);
	
	// 플레이어 캐릭터 겹침 허용
	ActivationTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ActivationTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	ActivationTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

// Called when the game starts or when spawned
void ABZFallingStructure::BeginPlay()
{
	Super::BeginPlay();
	
	// 활성화 트릭 박스에 오버랩 시작 이벤트 센서 연결
	ActivationTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABZFallingStructure::OnActivationOverlapBegin);
}

void ABZFallingStructure::TriggerCollapse()
{
	bIsCollapsed = true;
}

void ABZFallingStructure::ExecuteRadialCrush(FVector CrushLocation)
{
	
	TArray<FOverlapResult> OverlapResults;
	UE_LOG(LogTemp, Warning, TEXT("[ExecuteRadialCrush] 함수 시작! 중심 위치: %s, 대미지 반경: %f"), *CrushLocation.ToString(), CrushRadius);
		
	FCollisionShape CrushShape = FCollisionShape::MakeSphere(CrushRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	FCollisionObjectQueryParams ObjectQueryParamsParams;
	ObjectQueryParamsParams.AddObjectTypesToQuery(ECC_Pawn);
	
	bool bHit = GetWorld()->OverlapMultiByChannel(OverlapResults, CrushLocation, FQuat::Identity, ECC_Pawn, CrushShape, QueryParams);
	
	UE_LOG(LogTemp, Warning, TEXT("[ExecuteRadialCrush] 범위 검사 결과: %s, 감지된 Pawn 개수: %d"), bHit ? TEXT("TRUE") : TEXT("FALSE"), OverlapResults.Num());
	
	if (bHit)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (!OverlappedActor)
			{
				continue;
			}
			
			UE_LOG(LogTemp, Log, TEXT("[ExecuteRadialCrush] 감지된 액터 이름: %s"), *OverlappedActor->GetName());
				
			ABZZombie* Zobmbie = Cast<ABZZombie>(OverlappedActor);
			if (Zobmbie)
			{
				// 대미지 전달
				FDamageEvent PurCrushDamageEvent;
				Zobmbie->TakeDamage(CrushDamage, PurCrushDamageEvent, nullptr, this);
				
				UE_LOG(LogTemp, Warning, TEXT("Zombie %s is crushed by %s"), *Zobmbie->GetName(), *GetName())
				
				// 생존한 좀비 체크 및 디버프 
				if (Zobmbie->GetCharacterMovement() && !Zobmbie->IsFrenzied())
				{
					Zobmbie->GetCharacterMovement()->MaxWalkSpeed *= SurvivorSpeedMultiplier;
					Zobmbie->CustomTimeDilation = 0.5f;
				}
			}
		}
	}
	
}

void ABZFallingStructure::OnActivationOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	ABZPlayerCharacter* PlayerCharacter = Cast<ABZPlayerCharacter>(OtherActor);
	
	if (PlayerCharacter && !bIsCollapsed)
	{
		TriggerCollapse();
	}
}

