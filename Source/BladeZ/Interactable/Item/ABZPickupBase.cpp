// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Item/ABZPickupBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/Player/BZPlayerCharacter.h"
#include "TimerManager.h"

// Sets default values
AABZPickupBase::AABZPickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// 컴포넌트 생성 및 루트 설정
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->SetSphereRadius(50.0f);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	
	// 물리 및 충돌 세팅
	SphereComponent->SetSimulatePhysics(true);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	
	// 프리셋 커스텀 조율(플레이어는 겹치게 설정)
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	// 드랍되자마자 플레이어 무시	
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	// 물리 시뮬레이션 중 바닥에 안착(바닥은 무조건 Block)
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}

// Called when the game starts or when spawned
void AABZPickupBase::BeginPlay()
{
	Super::BeginPlay();
	
	// 오버랩 이벤트 바인딩 
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AABZPickupBase::OnOverlapBegin);
	
	// 아이템 드랍 후 'PickupDelay(1초)' 뒤에 EnablePickup 함수를 실행시키는 타이머 작동
	GetWorld()->GetTimerManager().SetTimer(PickupActivationTimer, this, &AABZPickupBase::EnablePickup, PickupDelay, false);
}

void AABZPickupBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 플레이어가 충돌했는지 검사
	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(OtherActor);
	if (Player)
	{
		// 아이템 효과 발동
		ApplyItemEffect(Player);
		
		// 획득했으므로 파괴
		Destroy();
	}
}

void AABZPickupBase::ApplyItemEffect_Implementation(AActor* TargetActor)
{
	// 아이템 효과 적용 로직을 여기에 작성하세요.
}

void AABZPickupBase::EnablePickup()
{
	// 플레이어와 닿으면 Overlap 이벤트를 발생시키도록 콜리전을 변경.
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}
