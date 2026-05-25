// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Item/BZInteractItemBase.h"

#include "Components/PrimitiveComponent.h"
#include "Common/FBZDamageEvent.h"

// Sets default values
ABZInteractItemBase::ABZInteractItemBase()
{
    // ★ 수정 1: 타임라인(Timeline) 컴포넌트가 매 프레임 흐르며 작동하려면 액터의 틱이 반드시 켜져 있어야 합니다!
    PrimaryActorTick.bCanEverTick = true;
    
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    RootComponent = ItemMesh;
    
    // BZWeaponActor의 구체 트레이스(Sphere Trace)가 충돌할 수 있도록 세팅
    ItemMesh->SetCollisionObjectType(ECC_WorldDynamic);
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);
}

// Called when the game starts or when spawned
void ABZInteractItemBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth; 
}

float ABZInteractItemBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator, AActor* DamageCauser)
{
    // 이미 인터랙션이 완전히 끝나서 파괴 단계에 들어간 상태라면 대미지 완전히 무시
    if (bHasInteracted)
    {
       return 0.0f;
    }
    
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    // 타격 위치 계산
    FVector ImpactPoint = GetActorLocation();
    if (DamageEvent.IsOfType(FBZDamageEvent::ClassID))
    {
       const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
       ImpactPoint = PointDamageEvent->HitInfo.ImpactPoint;
    }
    
    CurrentHealth -= ActualDamage;
    
    // BP에서 만든 타격 이벤트 실행 (매 피격 시 흔들림 등 연출용)
    BP_OnItemInteractionTriggered(DamageCauser, ImpactPoint);
    
    // 체력이 다 달았거나, 애초에 1회성 조작 아이템인 경우 (진짜 파괴 조건 만족 시)
    if (CurrentHealth <= 0.0f || !bCanInteractMultipleTimes)
    {
       // ★ 진짜 작동이 끝난 이 시점에만 true로 잠가줍니다.
       bHasInteracted = true;
       
       // 더 이상 무기에 맞지 않도록 충돌체 끄기
       ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
       
       // BP에서 만든 파괴 이벤트 실행 (블루프린트 타임라인으로 신호 전달!)
       BP_OnItemDestroyed(DamageCauser);
    }
    
    return ActualDamage;
}

void ABZInteractItemBase::ReceiveDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
    FDamageEvent DefaultDamageEvent;
    TakeDamage(DamageAmount, DefaultDamageEvent, nullptr, DamageCauser);
}

void ABZInteractItemBase::ExplodeAndDropItem()
{
    // 데이터 테이블과 행 이름의 제대로 입력되었는지 검사
    if (!DropDataTable || DropTalbeRowName.IsNone())
    {
       UE_LOG(LogTemp, Warning, TEXT("DropDataTable or DropTableRowName is not set correctly."));
       return;
    }
    
    // 데이터(엑셀) 이름으로 해당 행의 정보를 뽑아 옴
    FBZDropTableItemRow* DropData = DropDataTable->FindRow<FBZDropTableItemRow>(DropTalbeRowName, TEXT("InteractItemDropLookup"));
    
   // 데이터가 있고, 드랍 풀이 비어있는지 검사
    if (DropData && DropData->DropPool.Num() > 0)
    {
       // 드랍할 아이티메 총 개수 결정 
       int32 DropCount = FMath::RandRange(DropData->MinDropCount, DropData->MaxCount);
       
       // 총 개수만큼 반복하며 랜덤하게 생성
       for (int32 i=0; i< DropCount; i++)
       {
          // 가중치 총합 계산
          int32 TotalWeight = 0;
          for (const FBZDropItemWeightedInfo& PoolItem : DropData->DropPool)
          {
             TotalWeight += PoolItem.Weight;
          }
          
          if (TotalWeight <= 0)
          {
             continue;
          }
          
          // 랜덤 주사위 번호 뽑기
          int32 RandomWeight = FMath::RandRange(0, TotalWeight-1);
          int32 CurrentWeightSum = 0;
          TSubclassOf<AActor> SelectedItemClass = nullptr;
          
          // 주사위 번호가 어느 아이템 구간에 속하는지 판별
          for (const FBZDropItemWeightedInfo& PoolItem : DropData->DropPool)
          {
             CurrentWeightSum += PoolItem.Weight;
             
             if (RandomWeight < CurrentWeightSum)
             {
                SelectedItemClass = PoolItem.DropItemClass;
                break;
             }
          }
          
          // 선택된 아이템이 있다면 스폰
          if (SelectedItemClass)
          {
             // 생성 위치 랜덤 하게 잡기(바닥 뚫림 방지 Z +=80)
             FVector SpawnLocation = GetActorLocation();
             SpawnLocation.X += FMath::RandRange(-50.0f, 50.0f);
             SpawnLocation.Y += FMath::RandRange(-50.0f, 50.0f);
             SpawnLocation.Z += 80.0f;
          
             // 회전 무작위 적용
             FRotator SpawnRotation = FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);
          
             // 스폰 겹침 처리 
             FActorSpawnParameters SpawnParams;
             SpawnParams.Owner = this;
             SpawnParams.Instigator = GetInstigator();
             SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
          
             // 월드에 아이템 스폰
             AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SelectedItemClass, SpawnLocation, SpawnRotation, SpawnParams);
             
             // 물리 적용
             if (SpawnedActor)
             {
                UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(SpawnedActor->GetRootComponent());
             
                if (PrimitiveComp && PrimitiveComp->IsSimulatingPhysics())
                {
                   FVector Impulse = FVector(
                      FMath::RandRange(-200.0f, 200.0f),
                      FMath::RandRange(-200.0f, 200.0f),
                      FMath::RandRange(300.0f, 500.0f));
                
                   PrimitiveComp->AddImpulse(Impulse, NAME_None, true);
                }
             }             
         }          
       }
    }
    Destroy();
}