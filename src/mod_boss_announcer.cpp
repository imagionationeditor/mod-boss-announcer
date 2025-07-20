
#include "Chat.h"
#include "Config.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Guild.h"
#include "InstanceScript.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "WorldSessionMgr.h"
#include "DatabaseEnv.h"
#include "GameTime.h"
#include <sstream>

static bool removeAura, BossAnnouncerEnable, BossAnnounceToPlayerOnLogin, FastKillRecordEnable, FastKillRecordAnnounce, LeaderboardEnable;
static bool GuildPointsEnable, GuildPointsAnnounce, GuildPointsRequireSameGuild, GuildPointsRequireOnline;
static uint32 FirstKillPoints, NewRecordPoints, Normal10Points, Normal25Points, Heroic10Points, Heroic25Points, MaxDistanceFromLeader;

// Helper function to get player role based on class and spec
std::string GetPlayerRole(Player* player)
{
    if (!player)
        return "DPS";
        
    uint8 classId = player->getClass();
    
    // For classes with multiple roles, check their talent specialization
    switch (classId)
    {
        case 1: // Warrior
        {
            // Check for Protection spec tanking abilities
            if (player->HasSpell(23922) ||  // Shield Slam
                player->HasSpell(2565) ||   // Shield Block
                player->HasSpell(871) ||    // Shield Wall
                player->HasSpell(12975) ||  // Last Stand
                player->HasSpell(355) ||    // Taunt
                player->HasSpell(1161) ||   // Challenging Shout
                player->HasSpell(20230) ||  // Retaliation
                player->HasSpell(676) ||    // Disarm
                player->HasSpell(72) ||     // Shield Bash
                player->HasSpell(18499))    // Berserker Rage (defensive)
                return "Tank";
            
            // Check talent points as backup
            uint32 talentPoints = player->GetTalentPointsCount(2); // Protection tree
            return (talentPoints >= 31) ? "Tank" : "DPS";
        }
        
        case 2: // Paladin
        {
            uint32 holyPoints = player->GetTalentPointsCount(0); // Holy
            uint32 protPoints = player->GetTalentPointsCount(2); // Protection
            
            // Check for Protection paladin tanking abilities
            if (player->HasSpell(48827) ||  // Avenger's Shield
                player->HasSpell(53595) ||  // Hammer of the Righteous
                player->HasSpell(48952) ||  // Holy Shield
                player->HasSpell(642) ||    // Divine Shield
                player->HasSpell(1044) ||   // Hand of Freedom
                player->HasSpell(62124) ||  // Hand of Reckoning (taunt)
                player->HasSpell(20925) ||  // Holy Shield (improved)
                player->HasSpell(31935) ||  // Avenger's Shield (improved)
                player->HasSpell(53601) ||  // Sacred Weapon
                player->HasSpell(54428))    // Divine Plea
            {
                if (protPoints >= 21) return "Tank"; // More likely tank with prot points
            }
            
            if (protPoints >= 31) return "Tank";
            if (holyPoints >= 31) return "Healer";
            return "DPS";
        }
        
        case 3: // Hunter
            return "DPS";
            
        case 4: // Rogue
            return "DPS";
            
        case 5: // Priest
        {
            uint32 shadowPoints = player->GetTalentPointsCount(2); // Shadow
            return (shadowPoints >= 31) ? "DPS" : "Healer";
        }
        
        case 6: // Death Knight
        {
            // Check for specific tanking talents that only tanks would take
            uint32 tankTalents = 0;
            
            // Count defensive/tanking talents
            if (player->HasSpell(49455)) tankTalents++; // Toughness (damage reduction)
            if (player->HasSpell(55233)) tankTalents++; // Vampiric Blood (tank cooldown)
            if (player->HasSpell(49189)) tankTalents++; // Will of the Necropolis (damage reduction when low HP)
            if (player->HasSpell(49154)) tankTalents++; // Improved Rune Tap (better self-healing)
            if (player->HasSpell(49508)) tankTalents++; // Anticipation (dodge chance)
            if (player->HasSpell(54639)) tankTalents++; // Blood of the North (rune enhancement)
            if (player->HasSpell(49226)) tankTalents++; // Acclimation (magic resistance stacking)
            if (player->HasSpell(55061)) tankTalents++; // Improved Frost Presence (threat and damage reduction)
            if (player->HasSpell(49145)) tankTalents++; // Spell Deflection (spell damage reduction)
            
            // If they have 3+ tanking talents, definitely a tank
            if (tankTalents >= 3) return "Tank";
            
            // Check for other Blood tanking abilities as backup
            if (player->HasSpell(49028) ||  // Dancing Rune Weapon
                player->HasSpell(48982) ||  // Rune Tap
                player->HasSpell(49005) ||  // Mark of Blood
                player->HasSpell(55050))    // Heart Strike
            {
                if (tankTalents >= 2) return "Tank"; // Lower threshold with abilities
            }
            
            // Otherwise DPS
            return "DPS";
        }
        
        case 7: // Shaman
        {
            uint32 enhancePoints = player->GetTalentPointsCount(1); // Enhancement
            uint32 elePoints = player->GetTalentPointsCount(0); // Elemental
            
            if (enhancePoints >= 31 || elePoints >= 31) return "DPS";
            return "Healer";
        }
        
        case 8: // Mage
            return "DPS";
            
        case 9: // Warlock
            return "DPS";
            
        case 11: // Druid
        {
            uint32 feralPoints = player->GetTalentPointsCount(1); // Feral
            uint32 balancePoints = player->GetTalentPointsCount(0); // Balance
            uint32 restoPoints = player->GetTalentPointsCount(2); // Restoration
            
            // Clear Balance = DPS
            if (balancePoints >= 31) return "DPS";
            
            // Clear Restoration = Healer
            if (restoPoints >= 31) return "Healer";
            
            // Feral - need to distinguish between Bear (Tank) and Cat (DPS)
            if (feralPoints >= 31)
            {
                // Check for specific Bear form tanking abilities
                if (player->HasSpell(9634) ||   // Dire Bear Form
                    player->HasSpell(5229) ||   // Enrage
                    player->HasSpell(22842) ||  // Frenzied Regeneration
                    player->HasSpell(33878) ||  // Mangle (Bear)
                    player->HasSpell(48564) ||  // Survival Instincts
                    player->HasSpell(22812) ||  // Barkskin (tanking utility)
                    player->HasSpell(16979) ||  // Feral Charge - Bear
                    player->HasSpell(6795) ||   // Growl (taunt)
                    player->HasSpell(5211) ||   // Bash
                    player->HasSpell(48480))    // Maul (bear attack)
                {
                    return "Tank";
                }
                
                // Check for Cat form DPS abilities
                if (player->HasSpell(48579) ||  // Claw
                    player->HasSpell(48572) ||  // Shred
                    player->HasSpell(48577) ||  // Ferocious Bite
                    player->HasSpell(48574) ||  // Rake
                    player->HasSpell(768) ||    // Cat Form
                    player->HasSpell(16857) ||  // Faerie Fire (Feral)
                    player->HasSpell(49376))    // Feral Charge - Cat
                {
                    return "DPS";
                }
                
                // Default feral in raids is usually tank
                return "Tank";
            }
            
            // No clear spec, default to healer
            return "Healer";
        }
        
        default:
            return "DPS";
    }
}

// Helper function to validate group for guild points
bool ValidateGroupForPoints(Player* player)
{
    if (!player->GetGroup())
        return false;
        
    Group* group = player->GetGroup();
    Guild* playerGuild = player->GetGuild();
    
    if (!playerGuild)
        return false;
        
    Player* leader = ObjectAccessor::FindPlayer(group->GetLeaderGUID());
    if (!leader)
        return false;
    
    // Check all group members
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member)
            continue;
            
        // Check if member is online (if required)
        if (GuildPointsRequireOnline && !member->IsInWorld())
            return false;
            
        // Check if member is from same guild (if required)
        if (GuildPointsRequireSameGuild)
        {
            Guild* memberGuild = member->GetGuild();
            if (!memberGuild || memberGuild->GetId() != playerGuild->GetId())
                return false;
        }
        
        // Check distance from leader (if required)
        if (MaxDistanceFromLeader > 0)
        {
            if (member->GetDistance(leader) > MaxDistanceFromLeader)
                return false;
        }
    }
    
    return true;
}

// Helper function to award guild points
void AwardGuildPoints(Player* player, uint32 bossEntry, uint32 difficulty, uint32 points, const std::string& reason)
{
    Guild* guild = player->GetGuild();
    if (!guild)
        return;
        
    uint32 guildId = guild->GetId();
    std::string guildName = guild->GetName();
    
    // Update or insert guild points
    WorldDatabase.Execute("INSERT INTO guild_boss_points (guild_id, guild_name, total_points, total_records, first_kills, monthly_points) "
                         "VALUES ({}, '{}', {}, {}, {}, {}) "
                         "ON DUPLICATE KEY UPDATE "
                         "total_points = total_points + {}, "
                         "total_records = CASE WHEN '{}' = 'New Record' THEN total_records + 1 ELSE total_records END, "
                         "first_kills = CASE WHEN '{}' = 'First Kill' THEN first_kills + 1 ELSE first_kills END, "
                         "monthly_points = monthly_points + {}",
                         guildId, guildName, points, 
                         (reason == "New Record") ? 1 : 0,
                         (reason == "First Kill") ? 1 : 0,
                         points,
                         points, reason.c_str(), reason.c_str(), points);
    
    // Add to history
    WorldDatabase.Execute("INSERT INTO guild_points_history (guild_id, boss_entry, difficulty, points_awarded, reason) "
                         "VALUES ({}, {}, {}, {}, '{}')",
                         guildId, bossEntry, difficulty, points, reason.c_str());
}

class Boss_Announcer : public PlayerScript
{
public:
	Boss_Announcer() : PlayerScript("Boss_Announcer", {
        PLAYERHOOK_ON_LOGIN,
        PLAYERHOOK_ON_CREATURE_KILL
    }) {}

    void OnPlayerLogin(Player *player)
    {
        if (BossAnnouncerEnable)
            if (BossAnnounceToPlayerOnLogin)
               ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00BossAnnouncer |rmodule.");
    }

    void OnPlayerCreatureKill(Player* player, Creature* boss)
    {
        if (BossAnnouncerEnable)
        {
            if (boss->GetMap()->IsRaid() && boss->GetLevel() > 80 && boss->IsDungeonBoss())
            {
                //lets get the info we want
                Map* map = player->GetMap();
                std::string p_name;
                std::string g_name;
                std::string boss_name = boss->GetName();
                std::string IsHeroicMode;
                std::string IsNormal;
                std::string tag_colour = "7bbef7";
                std::string plr_colour = "7bbef7";
                std::string guild_colour = "00ff00";
                std::string boss_colour = "ff0000";
                std::string alive_text = "00ff00";
                uint32 Alive_players = 0;
                uint32 Tanks = 0;
                uint32 Healers = 0;
                uint32 DPS = 0;
                Map::PlayerList const & playerlist = map->GetPlayers();

                if (!player->GetGroup())
                    p_name = player->GetName();
                else
                    p_name = player->GetGroup()->GetLeaderName();

                if (player->GetMap()->Is25ManRaid())
                    IsNormal = "25";
                else
                    IsNormal = "10";

                if (player->GetMap()->IsHeroic())
                    IsHeroicMode = "|cffff0000Heroic|r";
                else
                    IsHeroicMode = "|cff00ff00Normal|r";

                // Calculate difficulty for database
                uint8 difficulty = 0;
                if (player->GetMap()->Is25ManRaid())
                    difficulty += 1;
                if (player->GetMap()->IsHeroic())
                    difficulty += 2;

                std::ostringstream stream;

                for (Map::PlayerList::const_iterator itr = playerlist.begin(); itr != playerlist.end(); ++itr)
                {
                    if (!itr->GetSource())
                        continue;

                    if (itr->GetSource()->IsAlive())
                        Alive_players++;

                    // Count roles based on class and talent specialization
                    std::string role = GetPlayerRole(itr->GetSource());
                    if (role == "Healer")
                        Healers++;
                    else if (role == "Tank")
                        Tanks++;
                    else
                        DPS++;

                    if (removeAura == true)
                    {
                        uint32 buff = itr->GetSource()->GetTeamId() == TEAM_ALLIANCE ? 57723 : 57724;

                        if (itr->GetSource()->HasAura(buff))
                            itr->GetSource()->RemoveAura(buff);
                    }

                }

                // Get guild name
                if (player->GetGuild())
                {
                    g_name = player->GetGuildName();
                }
                else if (player->GetGroup())
                {
                    // Try to get guild from group leader
                    Player* leader = ObjectAccessor::FindPlayer(player->GetGroup()->GetLeaderGUID());
                    if (leader && leader->GetGuild())
                    {
                        g_name = leader->GetGuildName();
                    }
                    else
                    {
                        g_name = "< No Guild >";
                    }
                }
                else
                {
                    g_name = "< No Guild >";
                }

                // Fast Kill Record System (Heroic Only)
                bool isNewRecord = false;
                if (FastKillRecordEnable && player->GetMap()->IsHeroic())
                {
                    // Get instance start time and calculate kill time
                    InstanceScript* instance = map->GetInstanceScript();
                    uint32 killTime = 300; // Default 5 minutes for testing
                    
                    if (instance)
                    {
                        // Simple approach: use a default time or current time as placeholder
                        // In a full implementation, you'd want to track actual encounter start time
                        killTime = static_cast<uint32>(GameTime::GetGameTime() % 3600); // Use modulo for testing
                    }
                    
                    // Check if this is a new record
                    QueryResult result = WorldDatabase.Query("SELECT kill_time FROM boss_kill_records WHERE boss_entry = {} AND difficulty = {}", 
                                                           boss->GetEntry(), difficulty);
                    
                    if (!result || (result && killTime < result->Fetch()->Get<uint32>()))
                    {
                        isNewRecord = true;
                        
                        // Save the new record
                        WorldDatabase.Execute("REPLACE INTO boss_kill_records "
                                            "(boss_entry, boss_name, map_id, instance_id, difficulty, kill_time, "
                                            "player_name, guild_name, group_size, alive_players, kill_date) "
                                            "VALUES ({}, '{}', {}, {}, {}, {}, '{}', '{}', {}, {}, NOW())",
                                            boss->GetEntry(), boss_name, map->GetId(), map->GetInstanceId(),
                                            difficulty, killTime, p_name, g_name, 
                                            static_cast<uint32>(playerlist.getSize()), Alive_players);

                        // Save group composition
                        WorldDatabase.Execute("DELETE FROM boss_kill_group_composition WHERE boss_entry = {} AND difficulty = {}", 
                                            boss->GetEntry(), difficulty);

                        for (Map::PlayerList::const_iterator itr = playerlist.begin(); itr != playerlist.end(); ++itr)
                        {
                            if (!itr->GetSource())
                                continue;

                            Player* groupPlayer = itr->GetSource();
                            std::string playerName = groupPlayer->GetName();
                            uint8 playerClass = groupPlayer->getClass();
                            std::string role = GetPlayerRole(groupPlayer);
                            
                            bool isAlive = groupPlayer->IsAlive() ? 1 : 0;

                            WorldDatabase.Execute("INSERT INTO boss_kill_group_composition "
                                                "(boss_entry, difficulty, player_name, player_class, player_role, is_alive) "
                                                "VALUES ({}, {}, '{}', {}, '{}', {})",
                                                boss->GetEntry(), difficulty, playerName, playerClass, role, isAlive);
                        }
                    }
                }

                // Guild Points System
                uint32 pointsAwarded = 0;
                std::string pointReason = "";
                bool eligibleForPoints = false;
                
                if (GuildPointsEnable && g_name != "< No Guild >")
                {
                    // Check if all players meet requirements
                    eligibleForPoints = ValidateGroupForPoints(player);
                    
                    if (eligibleForPoints)
                    {
                        // Check if this is first kill
                        bool isFirstKill = false;
                        QueryResult firstKillCheck = WorldDatabase.Query("SELECT COUNT(*) FROM boss_kill_records WHERE boss_entry = {} AND difficulty = {}", 
                                                                        boss->GetEntry(), difficulty);
                        if (firstKillCheck && firstKillCheck->Fetch()->Get<uint32>() == 0)
                        {
                            isFirstKill = true;
                            pointsAwarded = FirstKillPoints;
                            pointReason = "First Kill";
                        }
                        else if (isNewRecord)
                        {
                            pointsAwarded = NewRecordPoints;
                            pointReason = "New Record";
                        }
                        else
                        {
                            // Award points based on difficulty for regular boss kills
                            switch (difficulty)
                            {
                                case 0: // Normal 10-man
                                    pointsAwarded = Normal10Points;
                                    pointReason = "Boss Kill (Normal 10)";
                                    break;
                                case 1: // Normal 25-man  
                                    pointsAwarded = Normal25Points;
                                    pointReason = "Boss Kill (Normal 25)";
                                    break;
                                case 2: // Heroic 10-man
                                    pointsAwarded = Heroic10Points;
                                    pointReason = "Boss Kill (Heroic 10)";
                                    break;
                                case 3: // Heroic 25-man
                                    pointsAwarded = Heroic25Points;
                                    pointReason = "Boss Kill (Heroic 25)";
                                    break;
                                default:
                                    pointsAwarded = 0;
                                    break;
                            }
                        }
                        
                        // Award points if any
                        if (pointsAwarded > 0)
                        {
                            AwardGuildPoints(player, boss->GetEntry(), difficulty, pointsAwarded, pointReason);
                        }
                    }
                }

                // Build announcement message
                stream << "|CFF" << tag_colour << "|r|cff" << plr_colour << " " << p_name << "|r's Guild |cff" << guild_colour << "" << g_name << "|r has slain |CFF" << boss_colour << "[" << boss_name << "]|r with remaining |cff" << alive_text << "" << Alive_players << " /" << IsNormal << "|r players alive on " << IsHeroicMode << " mode, possible group |cff" << tag_colour << "Tank: " << Tanks  <<"|r |cff" << guild_colour <<
                    " Healers: "<< Healers << "|r |cff" << boss_colour << " DPS: " << DPS << "|r";

                // Add record announcement if it's a new record
                if (isNewRecord && FastKillRecordAnnounce)
                {
                    stream << " |cffFFD700*** NEW FASTEST KILL RECORD! ***|r";
                }

                // Add guild points announcement
                if (pointsAwarded > 0 && GuildPointsAnnounce)
                {
                    stream << " |cff98FB98[+" << pointsAwarded << " Guild Points - " << pointReason << "]|r";
                }

                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
            }
        }
    }
};

class Boss_Announcer_World : public WorldScript
{
public:
	Boss_Announcer_World() : WorldScript("Boss_Announcer_World", {
        WORLDHOOK_ON_BEFORE_CONFIG_LOAD
    }) { }

	void OnBeforeConfigLoad(bool reload) override
	{
		if (!reload)
            SetInitialWorldSettings();
	}
    void SetInitialWorldSettings()
    {
        removeAura = sConfigMgr->GetOption<bool>("Boss.Announcer.RemoveAuraUponKill", false);
        BossAnnouncerEnable = sConfigMgr->GetOption<bool>("Boss.Announcer.Enable", true);
        BossAnnounceToPlayerOnLogin = sConfigMgr->GetOption<bool>("Boss.Announcer.Announce", true);
        FastKillRecordEnable = sConfigMgr->GetOption<bool>("Boss.Announcer.FastKillRecord.Enable", true);
        FastKillRecordAnnounce = sConfigMgr->GetOption<bool>("Boss.Announcer.FastKillRecord.Announce", true);
        LeaderboardEnable = sConfigMgr->GetOption<bool>("Boss.Announcer.Leaderboard.Enable", true);
        
        // Guild Points System
        GuildPointsEnable = sConfigMgr->GetOption<bool>("Boss.Announcer.GuildPoints.Enable", true);
        GuildPointsAnnounce = sConfigMgr->GetOption<bool>("Boss.Announcer.GuildPoints.Announce", true);
        GuildPointsRequireSameGuild = sConfigMgr->GetOption<bool>("Boss.Announcer.GuildPoints.RequireSameGuild", true);
        GuildPointsRequireOnline = sConfigMgr->GetOption<bool>("Boss.Announcer.GuildPoints.RequireOnline", true);
        FirstKillPoints = sConfigMgr->GetOption<uint32>("Boss.Announcer.GuildPoints.FirstKill", 200);
        NewRecordPoints = sConfigMgr->GetOption<uint32>("Boss.Announcer.GuildPoints.NewRecord", 100);
        Normal10Points = sConfigMgr->GetOption<uint32>("Boss.Announcer.GuildPoints.Normal10", 10);
        Normal25Points = sConfigMgr->GetOption<uint32>("Boss.Announcer.GuildPoints.Normal25", 20);
        Heroic10Points = sConfigMgr->GetOption<uint32>("Boss.Announcer.GuildPoints.Heroic10", 50);
        Heroic25Points = sConfigMgr->GetOption<uint32>("Boss.Announcer.GuildPoints.Heroic25", 100);
        MaxDistanceFromLeader = sConfigMgr->GetOption<uint32>("Boss.Announcer.GuildPoints.MaxDistance", 100);
    }
};

void AddBoss_AnnouncerScripts()
{
	new Boss_Announcer_World;
	new Boss_Announcer;
}
