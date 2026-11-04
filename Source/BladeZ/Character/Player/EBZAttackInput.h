#pragma once

UENUM(BlueprintType)
enum class EBZAttackInputType : uint8
{
	None,
	Left,
	Right,
};

UENUM(BlueprintType)
enum class EComboInputState : uint8
{
	Closed UMETA(DisplayName = "입력 불가"),
	Open UMETA(DisplayName = "입력 가능"), 
	OpenBuffered UMETA(DisplayName = "입력 저장 중"),
	ClosedBuffered UMETA(DisplayName = "입력 저장 완료"),
};

UENUM(BlueprintType)
enum class EPlayerStatus : uint8
{
	Alive UMETA(DisplayName = "정상"),
	Dead UMETA(DisplayName = "사망"),
};