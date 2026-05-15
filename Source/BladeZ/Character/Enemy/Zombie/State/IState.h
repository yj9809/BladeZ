#pragma once

// Finite State Machine에서 사용할 State 클래스.
class ABZZombie;
class IState
{
public:
	IState(ABZZombie* InOwner) : Owner(InOwner)
	{
	}

	virtual ~IState() = default;

	// 스테이트를 처음 진입할 때 한 번 호출할 함수 (액터의 BeginPlay와 유사).
	virtual void OnEnter() = 0;

	// 스테이트를 업데이트할 때 프레임마다 호출할 함수 (일종의 Tick).
	virtual void OnUpdate(float DeltaTime) = 0;

	// 스테이트를 빠져나갈 때 한 번 호출할 함수.
	virtual void OnExit() = 0;

protected:
	// 이 스테이트를 소유하는 캐릭터 참조 변수.
	ABZZombie* Owner = nullptr;
};
