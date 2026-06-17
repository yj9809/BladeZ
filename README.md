# ⚔️ BladeZ

> **Unreal Engine 5 · C++** 기반으로 컴포넌트 분리 · TMap 콤보 · DataAsset 데이터 관리 · 루트 모션 대시를 직접 구현한 **3D 무쌍 액션 플레이어 캐릭터 시스템** 프로젝트

<p align="left">
  <img src="https://img.shields.io/badge/Unreal_Engine-5.6-0E1128?logo=unrealengine&logoColor=white" />
  <img src="https://img.shields.io/badge/Language-C++-00599C?logo=cplusplus&logoColor=white" />
  <img src="https://img.shields.io/badge/Genre-3D_Hack_&_Slash-orange" />
  <img src="https://img.shields.io/badge/Platform-PC-lightgrey" />
</p>

---

## 📌 프로젝트 개요

| 항목 | 내용 |
| --- | --- |
| **게임 이름** | BladeZ |
| **플랫폼** | PC (Unreal Engine 5) |
| **장르** | 3D 무쌍 액션 |
| **제작 방식** | C++ 전용 |
| **개발 스택** | Unreal Engine 5.6 / C++ / DataAsset / AnimNotify / Niagara / StateTree |
| **개발 기간** | 2026.05.12 ~ 2026.05.28 |
| **팀 구성** | 5인 협업 |

**담당 업무** — 플레이어 캐릭터 전체(이동 · 콤보 · 대시 · 피격/죽음 · 전투 피드백) · 팀원 디버깅 지원 · 기능 협업

---

## 🎯 핵심 목표

- 컴포넌트 기반 전투 설계로 **전투 로직을 캐릭터 클래스에서 분리**
- **DataAsset(`UPrimaryDataAsset`)** 으로 공격 데이터(데미지 · 히트스톱 · 이펙트 · 카메라 쉐이크)를 에셋에서 관리
- **TMap 콤보 구조**와 **루트 모션 대시**를 C++로 직접 구현
- 이동부터 전투 피드백(HitStop · 카메라 쉐이크 · Niagara 이펙트)까지 플레이어 캐릭터 전체 구현

---

## ✨ Key Points

| 구현 | 설명 |
| --- | --- |
| **컴포넌트 분리로 전투 로직 캡슐화** | `BZPlayerCombatComponent`가 콤보 · 히트스톱 · 이펙트를 모두 담당, 캐릭터 클래스는 입력 전달만 수행 |
| **DataAsset 기반 데이터 분리** | `UPrimaryDataAsset`으로 공격별 데미지 · 히트스톱 · 이펙트 · 카메라 쉐이크를 에셋에서 독립 관리 → 로직 수정 없이 기획 반영 |
| **루트 모션 대시 버그 해결** | `FallingLateralFriction = 5.0f` 적용으로 낭떠러지 앞 대시 시 공중 이탈 현상 해결 |
| **TMap 콤보 윈도우 설계** | `입력 타입 + 섹션명` 조합 키로 다음 공격 결정, AnimNotify를 커밋 트리거로 사용 |
| **HitStop 실시간 타이머** | `SetGlobalTimeDilation` + `GetRealTimeSeconds` 기준 (DeltaTime은 TimeDilation 영향으로 사용 불가) |

---

## 🛠 핵심 구현

### 1) 콤보 시스템 (TMap + AnimNotify)

- 공격 입력(Left/Right)과 현재 섹션명을 키로 조합: `"{SectionName}_{InputType}"` → `TMap<FName, FName>`으로 다음 섹션명 조회
- `FBZAttackData`(데미지 · HitStop 등)는 `AttackDataArray`에서 `FindByPredicate`로 별도 조회
- 공격 중 입력은 `NextInputType`에 즉시 버퍼링하고 최신 입력으로 계속 덮어씀
- `BZANComboCheck` 노티파이는 입력 게이트가 아닌 **커밋 트리거**로 동작 → 도달 시 마지막 버퍼 입력 기준으로 다음 섹션 결정
- 첫 공격은 `L_1 / L_1_1` 중 랜덤 선택으로 시작 패턴 다양화

<details>
<summary><b>⚠️ PlayRate 가속 시 콤보 입력 누락 문제</b></summary>

- **문제** — 무쌍 장르의 속도감을 위해 PlayRate를 올렸더니 콤보 윈도우(노티파이 구간)가 짧아져 정확한 타이밍에 입력하지 않으면 콤보가 끊김
- **원인** — 기존 설계는 `BZANComboCheck` 노티파이 수신 구간에서만 입력을 허용 → PlayRate가 빨라질수록 입력 가능 창이 비례해 좁아짐
- **해결** — 입력 수락 시점과 콤보 전환 시점을 분리. `SetAttackInput()`에서 `bIsAttacking` 상태면 언제든 입력을 버퍼링하고, 노티파이(`CheckCombo()`)는 저장된 마지막 입력을 읽어 다음 섹션으로 점프하는 커밋 트리거 역할만 수행

</details>

### 2) Montage Curve 기반 동적 PlayRate

- `AttackMontage`에 내장된 `AttackRate Float Curve`를 `TickComponent`에서 매 프레임 Evaluate
- `Montage_GetPosition`으로 현재 재생 위치를 구한 뒤 `GetCurveData()`로 커브 값 샘플링
- `BasePlayRate(2.5f) × CurveValue`로 PlayRate 실시간 갱신 → 몽타주 에셋에서 직접 속도 프로파일 조정 가능

### 3) 루트 모션 대시

- 8방향 입력 → `GetDashSectionName()`으로 대응 몽타주 섹션 결정 후 루트 모션 적용
- 대시 중 캡슐 충돌을 `ECR_Overlap`으로 전환해 적을 관통, `OnCapsuleOverlap`에서 적의 좌우 방향을 계산해 `LaunchCharacter`로 밀어냄
- 쿨다운 타이머로 연속 입력 방지, 추락 중 대시 불가 처리

<details>
<summary><b>⚠️ 낭떠러지 앞 대시 시 공중 이탈 문제</b></summary>

- **문제** — 낭떠러지 앞에서 대시 시 공중에서 이동량이 유지된 채 멀리 날아감
- **원인** — 공중에서는 지면 마찰이 없어 루트 모션 이동량이 감소하지 않고 누적됨
- **시도** — Gravity 값 조정 → 너무 빠르게 떨어져 착지감이 부자연스러워 실패
- **해결** — 대시 시작 시 `FallingLateralFriction = 5.0f` 적용, `OnLandMontageEnded` 콜백에서 `0.0f`로 복구하여 공중 수평 마찰로 이동량이 자연스럽게 감소

</details>

### 4) 무기 SphereTrace 판정

프레임 사이 빈틈 없이 타격을 감지하기 위해 **다중 Sphere 보간** 방식을 채택:

- Step 보간(계산량 과다), 단일 Sphere 연결(크기 과대), 8점 직선 Trace(감지 사각 발생) 안을 기각
- 무기 트레이스 영역의 포인트마다 `이전 위치 → 현재 위치`를 Sphere Trace로 연결 → 불필요한 감지 영역 최소화 + 빈틈 보완

<details>
<summary><b>⚠️ HitResults 배열 재사용으로 밀착 시 충돌 미감지</b></summary>

- **문제** — 트레이스 보간 적용 후 적과 밀착 시 충돌 처리가 발생하지 않음
- **원인** — `SphereTraceMulti`에 동일한 `HitResults` 배열을 재사용하면 for 루프마다 배열이 덮어써져, 충돌이 없는 포인트가 이전 결과를 지움
- **해결** — 루프 내부에 `PointHitResults` 별도 배열 생성 후 `Append`로 통합, 이미 히트한 Actor 필터링으로 중복 타격 방지

```cpp
for (int i = 0; i < NumCount; i++)
{
    float Alpha = static_cast<float>(i) / (NumCount - 1);
    FVector PrevPoint = FMath::Lerp(PrevStart, PrevEnd, Alpha);
    FVector CurrPoint = FMath::Lerp(StartLocation, EndLocation, Alpha);

    TArray<FHitResult> PointHitResults;  // 포인트별 개별 배열

    UKismetSystemLibrary::SphereTraceMulti(
        this, PrevPoint, CurrPoint, Radius,
        UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2),
        false, ActorsToIgnore, EDrawDebugTrace::ForDuration,
        PointHitResults,
        true, FLinearColor::Red, FLinearColor::Green, 1.0f
    );
    HitResults.Append(PointHitResults);  // 전체 배열에 통합
}
```

</details>

### 5) 전투 피드백 (HitStop · 카메라 쉐이크 · Niagara)

- **HitStop** — `SetGlobalTimeDilation`으로 시간 감속, 지속 시간은 `GetWorld()->GetRealTimeSeconds()` 기준 (DeltaTime은 TimeDilation 영향으로 사용 불가). 다수의 적을 상대하는 게임 특성상 모든 공격이 아닌 특정 콤보에서만 적용
- **카메라 쉐이크** — `FOnCameraShake` 델리게이트 체인 → `UBZCameraShakeComponent` 수신, DataAsset Scale로 공격별 강도 독립 조절
- **Niagara 이펙트** — `FBZAttackData.HitEffect` 배열(최대 2개)로 공격별 파티클 독립 지정
- **처리 순서** — 카메라 쉐이크 → 데미지 → HitStop → Niagara 이펙트

### 6) 피격 / 죽음 처리

- `FBZDamageEvent`(커스텀 데미지 이벤트)로 DamageType 전달: `Light / Heavy / Knockdown` → 수신 측에서 몽타주 섹션 분기
- `SetDead()` — 충돌 무시 설정 → DeadMontage 재생 → 입력 차단 → `OpenLevel`

<details>
<summary><b>⚠️ PIE 레벨 전환 시 레벨명 접두사 문제</b></summary>

- **문제** — PIE 실행 시 `OpenLevel` 호출이 실패하거나 잘못된 레벨로 전환됨
- **원인** — PIE 환경에서 레벨명에 `"UEDPIE_0_"` 접두사가 자동으로 붙어 기존 레벨명으로 찾을 수 없음
- **해결** — `RemoveFromStart("UEDPIE_0_")`로 접두사 제거 후 레벨 전환

</details>

---

## 📂 프로젝트 구조

```
Source/BladeZ/
├─ Character/
│  ├─ Player/                 # 플레이어 캐릭터 (담당 영역)
│  │  ├─ Animation/           # AnimInstance, 콤보/트레이스/패링 노티파이
│  │  ├─ BZPlayerCharacter    # 플레이어 캐릭터 클래스 (입력 전달)
│  │  └─ BZPlayerAttackData   # 공격 DataAsset
│  └─ Enemy/
│     ├─ BossTank/            # 보스(탱크) - StateMachine 기반 행동
│     └─ Zombie/              # 좀비 - 오브젝트 풀 · Niagara 스왑 · State
└─ LoadingScreenModule/       # 로딩 스크린 모듈

Source/LoadingScreenModule/   # 별도 런타임 모듈 (PreLoadingScreen)
```

> `Variant_Combat` · `Variant_Platforming` · `Variant_SideScrolling`은 언리얼 템플릿 기본 제공 코드입니다.

---

## ▶️ 빌드 / 실행

1. **Unreal Engine 5.6** 설치
2. 리포지토리 클론 후 `BladeZ.uproject` 우클릭 → **Generate Visual Studio project files**
3. `BladeZ.sln`을 Visual Studio로 열어 **Development Editor** 구성으로 빌드
4. 빌드 완료 후 `BladeZ.uproject` 실행

---

## 🔎 회고

- 컴포넌트 분리와 DataAsset 구조를 직접 설계하며 **"기획 변경에 코드가 흔들리지 않는 구조"** 가 어떤 형태인지 체감
- 루트 모션 · Niagara SIMD · PIE 접두사까지 엔진 내부 동작을 **가설 → 검증**으로 파고드는 디버깅 습관 형성
- 팀원이 내 시스템을 쓸 수 있도록 문서화하면서 **"구현보다 전달이 어렵다"** 는 점을 체감

---

## 🤝 협업 기여

- 팀 코딩 표준 문서 수립 → 팀 전체 코드 일관성 확보
- `BZSoundManager` 구현 후 BP 연동 가이드 작성 → UI 담당 팀원의 독립 연동 지원
- Niagara ↔ C++ 파티클 변환 시스템 디버깅 (SIMD 배치 처리 문제 → 독립 동작 구조로 재설계 후 팀 공유)
