#pragma once

#include "IState.h"

// Attack.
class AttackState : public IState
{
public:
	AttackState(ABZZombie* Owner)
		: IState(Owner)
	{
	}

	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnExit() override;
};