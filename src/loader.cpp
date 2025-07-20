#include "ScriptMgr.h"

// External declarations
void AddBoss_AnnouncerScripts();
void AddBossRecordsLeaderboardScripts();
void AddGuildPointsLeaderboardScripts();

void AddModBossAnnouncerScripts()
{
    AddBoss_AnnouncerScripts();
    AddBossRecordsLeaderboardScripts();
    AddGuildPointsLeaderboardScripts();
}
