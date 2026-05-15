#pragma once

#include "IState.h"

// Idle.
class IdleState : public IState
{
public:
	IdleState(ABZZombie* Owner)
		: IState(Owner)
	{
	}

	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnExit() override;
};