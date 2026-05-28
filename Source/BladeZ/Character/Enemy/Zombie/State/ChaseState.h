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
	
private:
	// 좀비 개별 고유의 길찾기 누적 시간 변수
	float PathfindingElapsedTime = 0.0f;
};