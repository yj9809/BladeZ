# Branching Point 변경 후 로그 요약

출처: 2026-10-28 대화에서 사용자가 직접 붙여 넣은 Seq 1135–1145 로그.

이 파일은 원문 로그가 아니라 분석에 필요한 필드를 추린 요약이다. 모든 행은 `Actor=BZPlayerCharacter_0`, `Delta=0.1000`, `InstanceId=97`, `AttackActive=1`, `Stopped=0`이었다. 이 발췌에는 Blend Out Time / Trigger Time / Auto Blend Out 설정값이 없다.

| Seq | Frame | Time | Event | Section / InstanceSection | Pos / InstancePos | HasNext | Buffered | 결과 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1135 | 37695 | 2.5137 | ComboDecision | L_1 | 0.0000 | 0 | None | Started / StartComboAttack |
| 1136 | 37701 | 3.1137 | NotifyEntered | L_1 | 1.5043 | 1 | Left | BZANComboCheck |
| 1137 | 37701 | 3.1137 | ComboDecision | LL_2 | 1.7673 | 1 | Left | JumpApplied / MappedCombo / L_1 → LL_2 / Consumed=1 |
| 1138 | 37706 | 3.6137 | NotifyEntered | LL_2 | 3.2630 | 1 | Left | BZANComboCheck |
| 1139 | 37706 | 3.6137 | ComboDecision | LLL_3 | 3.2965 | 1 | Left | JumpApplied / MappedCombo / LL_2 → LLL_3 / Consumed=1 |
| 1140 | 37715 | 4.5137 | NotifyEntered | LLL_3 | 5.5465 | 1 | Left | BZANComboCheck |
| 1141 | 37715 | 4.5137 | ComboDecision | LLLL_4 | 5.6309 | 1 | Left | JumpApplied / MappedCombo / LLL_3 → LLLL_4 / Consumed=1 |
| 1142 | 37723 | 5.3137 | NotifyEntered | LLLL_4 | 7.5205 | 1 | Left | BZANComboCheck |
| 1143 | 37723 | 5.3137 | ComboDecision | L_1_1 | 15.7238 | 1 | Left | JumpApplied / NoMapping.RandomFirstCombo / LLLL_4 → L_1_1 / Consumed=1 |
| 1144 | 37727 | 5.7137 | NotifyEntered | L_1_1 | 16.7238 | 0 | None | BZANComboCheck |
| 1145 | 37727 | 5.7137 | ComboDecision | L_1_1 | 16.7238 | 0 | None | Skipped / NoBufferedInput / Consumed=0 |

`Weight`는 시작 행인 Seq 1135에서만 0이고, 나머지 행에서는 1이었다.

이 발췌에는 `BlendOutStarted` / `AttackEnded`가 포함되지 않는다. 로그가 그 지점에서 끝나므로, 이 발췌만으로 종료 불능을 단정하지 않는다.
