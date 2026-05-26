// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Item/BZInteractItemBase.h"

#include "Components/PrimitiveComponent.h"
#include "Common/FBZDamageEvent.h"
#include "Engine/OverlapResult.h"

#include "GameFramework/Character.h"
#include "Character/Enemy/Zombie/BZZombie.h"
#include "GameFramework/CharacterMovementComponent.h"

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
   // 예외처리
   if (DamageAmount <= 0.0f)
   {
      return 0.0f;
   }
   
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
       
      if (bPullBeforeExplosion)
      {
         // 체크박스가 켜져 있으면 흡입 시퀀스 가동
         StartPullSequence(DamageCauser);
      }
      else
      {
         // 일반 드럼통이라면 기존대로 즉시 파괴 이벤트 실행(블루프린트 타임라인으로 신호 전달!)
         BP_OnItemDestroyed(DamageCauser);
      }
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

void ABZInteractItemBase::StartPullSequence(AActor* DamageCauser)
{
   // 0.05초 마다 주기적으로 주변 좀비들을 빨아들이는 타이머 작동
   GetWorld()->GetTimerManager().SetTimer(PullTimerHandle, this,&ABZInteractItemBase::PullTick, 0.05f, true);
   
   // 타이머와 동시에 블루프린트 타임라인을 출발시킨다.
   BP_OnPullStarted(DamageCauser);
   
   FTimerDelegate ExplosionDelegate;
   ExplosionDelegate.BindUObject(this, &ABZInteractItemBase::ExecuteActualExplosion, DamageCauser);
   
   // 빨아들인 뒤 최종 폭발하도록 예약
   GetWorld()->GetTimerManager().SetTimer(ExplosionDelayTimerHandle, ExplosionDelegate, PullDuration, false);
}

void ABZInteractItemBase::PullTick()
{
   FVector ItemLocation = GetActorLocation();
   
   // 바닥 간섭 원천 차단
   FCollisionObjectQueryParams ObjectQueryParams;
   ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
   
   TArray<FOverlapResult> OverlapResults;
   
   FCollisionShape PullShape = FCollisionShape::MakeSphere(PullRadius);
   FCollisionQueryParams QueryParams;
   QueryParams.AddIgnoredActor(this);// 자기 자신 제외
   
   bool bHit = GetWorld()->OverlapMultiByObjectType(OverlapResults, ItemLocation, FQuat::Identity, ObjectQueryParams, PullShape, QueryParams);
   
   if (bHit)
   {
      for (const FOverlapResult& Result : OverlapResults)
      {
         // 특정 좀비 클래스에 의존하지 않고 언리얼 전용 ACharacter로 캐스팅하여 범용성 확보
         ABZZombie* Zombie = Cast<ABZZombie>(Result.GetActor());
         if (Zombie && Zombie->GetCharacterMovement())
         {
           UCharacterMovementComponent* MoveComp = Zombie->GetCharacterMovement();
            
           // 좀비와 드럼통 사이의 진짜 남은 거리 계산
           FVector ZombieLocation = Zombie->GetActorLocation();
           float Distance = FVector::Dist(ItemLocation, ZombieLocation);
            
           // 도착 데드존 설정
           if (Distance < 45.0f)
           {
              MoveComp->Velocity = FVector(0.0f, 0.0f, MoveComp->Velocity.Z);
              continue;
           }
            
           FVector PullDirection = (ItemLocation-ZombieLocation).GetSafeNormal();
           PullDirection.Z = 0.0f;
            
           float CurrentStrength = PullStrength;
           if (Distance < 200.0f)
           {
              float Alpha = (Distance - 45.0f) / (200.0f - 45.0f);
              Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
              CurrentStrength *= Alpha;
           }
           
            FVector NewVelocity = PullDirection * CurrentStrength;
                        
            MoveComp->Velocity = FVector(NewVelocity.X, NewVelocity.Y, MoveComp->Velocity.Z);
         }
      }
   }   
}

void ABZInteractItemBase::ExecuteActualExplosion(AActor* DamageCauser)
{
   // 가동 중이던 모든 흡입 타이머 안전한게 종료
   GetWorld()->GetTimerManager().ClearTimer(PullTimerHandle);
   GetWorld()->GetTimerManager().ClearTimer(ExplosionDelayTimerHandle);
   
   bHasExploded = true;
   
   // 흡입 완료 후 원래 실행되어야 했던 블루프린트 파괴 이벤트 호출
   BP_OnItemDestroyed(DamageCauser);
   
   UE_LOG(LogTemp, Warning, TEXT("[Vortex]%s 아이템 블랙홀 흡입 완료 후 최종 폭발 작렬!"), *GetName());
}

