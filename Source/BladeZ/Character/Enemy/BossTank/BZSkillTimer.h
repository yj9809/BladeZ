#pragma once

#include "CoreMinimal.h"
#include "BZSkillTimer.generated.h"

// 쿨타임 관리 객체

USTRUCT(BlueprintType)
struct FSkillCooldown
{
	GENERATED_BODY()

	float CurrentTime;
	float SetTime;

	FSkillCooldown() : CurrentTime(0.0f), SetTime(0.0f) {}

	// 인자를 받는 생성자
	FSkillCooldown(float InSetTime) 
		: CurrentTime(0.0f)
		, SetTime(InSetTime) // 입력받은 값을 SetTime에 대입
	{}
	
	bool IsTimeout() const { return CurrentTime >= SetTime; }
	void Reset() { CurrentTime = 0.0f; }
};
