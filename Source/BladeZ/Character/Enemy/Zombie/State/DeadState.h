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
	
	
	/*
	 * 작성자: 윤제영.
	 * 작성일: 26.05.22
	 * 작성 사유: Dissolve 처리를 위해 작성.
	 */
	// 디졸브 처리를 위한 private영역.
private:
	bool bIsDissolving = false;
	
	float DissolveValue = 0.0f;
	
	float DissolveSpeed = 0.5f;
	
	TArray<class UMaterialInstanceDynamic*> DynamicMaterials;
	
	void StartDissolve();
};