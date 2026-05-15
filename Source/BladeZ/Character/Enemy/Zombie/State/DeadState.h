#pragma once

#include "IState.h"

// Dead.
class DeadState : public IState
{
public:
	DeadState(ABZZombie* Owner)
		: IState(Owner)
	{
	}

	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnExit() override;
};