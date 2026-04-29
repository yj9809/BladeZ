#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogBZBoss, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogBZPlayer, Log, All);

#define BOSS_LOG(Verbosity, Format, ...) UE_LOG(LogBZBoss, Verbosity, TEXT(Format), ##__VA_ARGS__)
#define PLAYER_LOG(Verbosity, Format, ...) UE_LOG(LogBZPlayer, Verbosity, TEXT(Format), ##__VA_ARGS__)
