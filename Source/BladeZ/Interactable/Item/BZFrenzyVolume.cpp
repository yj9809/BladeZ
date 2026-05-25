// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Item/BZFrenzyVolume.h"
#include "Components/BoxComponent.h"
#include "Character/Enemy/Zombie/BZZombie.h"

// Sets default values
ABZFrenzyVolume::ABZFrenzyVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// 에디터에서 배치할 수 있도록 기준점이 되는 기본 루트 컴포넌트를 생성합니다.
	USceneComponent* DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DummyRoot;
	
	// 좀비를 감지할 가상의 박스 콜리전(TriggerBox)을 실제로 메모리에 생성하고 루트에 붙입니다.
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	
	// 다른 불필요한 채널(카메라, 무기 등)은 완전히 무시하고, 
	// 오직 좀비나 플레이어가 사용하는 ECC_Pawn 채널에만 오버랩(겹침) 반응을 하도록 잠금 장치를 겁니다.
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

// Called when the game starts or when spawned
void ABZFrenzyVolume::BeginPlay()
{
	Super::BeginPlay();
	
	// 박스 콜리전에 무언가 겹치기 시작하는 순간(OnComponentBeginOverlap) 
	// 우리가 하단에 만든 OnZombieOverlapBegin 함수가 자동으로 실행되도록 센서를 연결(바인딩)합니다.
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABZFrenzyVolume::OnZombieOverlapBegin);
}

void ABZFrenzyVolume::OnZombieOverlapBegin(UPrimitiveComponent* OverlappedCop, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 2. 구역에 겹친 액터가 'ABZZombie' 클래스인지 확인합니다. (플레이어나 다른 오브젝트는 무시)
	if (ABZZombie* OverlappedZombie = Cast<ABZZombie>(OtherActor))
	{
		// 3. 겹친 좀비가 아직 광폭화 상태가 아니라면?
		if (!OverlappedZombie->IsFrenzied())
		{
			// 4. 좀비 본체에 만들어둔 광폭화 스위치를 켭니다!
			OverlappedZombie->SetFrenzyMode(true);
			
			UE_LOG(LogTemp, Warning, TEXT("광폭화 구역 발동! %s 가 거대해지고 강력해집니다!"), *OverlappedZombie->GetName());
		}
	}
}



