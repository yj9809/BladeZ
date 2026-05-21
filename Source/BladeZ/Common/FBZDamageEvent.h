#pragma once
#include "Engine/DamageEvents.h"

class FBZDamageEvent : public FPointDamageEvent
{
public:
	static const int32 ClassID = 100;

	virtual int32 GetTypeID() const override { return ClassID; }
	virtual bool IsOfType(int32 InID) const override
	{
		return (ClassID == InID) || FPointDamageEvent::IsOfType(InID);
	}

	// 적 날릴 때 호출할 Getter/Setter.
	FORCEINLINE void SetKnockback(bool IsKnockback) { bIsKnockback = IsKnockback; }
	FORCEINLINE bool IsKnockback() const { return bIsKnockback; }
	
	// 데미지 타입 세팅 시 호출할 Getter/Setter.
	// 0: 약한 공격, 1: 아픈 공격, 2: 넘어지기.
	FORCEINLINE void SetDamageType(int32 InDamageType) { DamageType = InDamageType; }
	FORCEINLINE int32 GetDamageType() const { return DamageType; }
	
	// 넉백 강도 설정 시 호출할 Getter/Setter.
	FORCEINLINE void SetKnockbackPower(float InKnockbackPower) { KnockbackPower = InKnockbackPower; }
	FORCEINLINE float GetKnockbackPower() const { return KnockbackPower; }

private:
	bool bIsKnockback = false;
	
	int32 DamageType = 0;
	
	float KnockbackPower = 1.0f;
};