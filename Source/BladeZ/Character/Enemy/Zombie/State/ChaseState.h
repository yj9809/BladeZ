#pragma once

#include "IState.h"

// Chase.
class ChaseState : public IState
{
public:
	ChaseState(ABZZombie* Owner)
		: IState(Owner)
	{
	}

	virtual void OnEnter() override;
	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnExit() override;
};