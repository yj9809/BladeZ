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

	FORCEINLINE void SetKnockback(bool IsKnockback) { bIsKnockback = IsKnockback; }
	FORCEINLINE bool IsKnockback() const { return bIsKnockback; }

private:
	bool bIsKnockback = false;
};