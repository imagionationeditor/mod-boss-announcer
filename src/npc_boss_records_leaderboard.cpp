#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "DatabaseEnv.h"
#include <sstream>

class npc_boss_records_leaderboard : public CreatureScript
{
public:
    npc_boss_records_leaderboard() : CreatureScript("npc_boss_records_leaderboard") { }

    struct npc_boss_records_leaderboardAI : public ScriptedAI
    {
        npc_boss_records_leaderboardAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_boss_records_leaderboardAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ClearGossipMenuFor(player);
        
        // Show all available instances
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_dungeon_icecrown_ironbound:24:24:-1:0|t Icecrown Citadel", GOSSIP_SENDER_MAIN, 631);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_dungeon_ulduar77_heroic:24:24:-1:0|t Ulduar", GOSSIP_SENDER_MAIN, 603);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_dungeon_coilfang_heroic:24:24:-1:0|t Trial of the Crusader", GOSSIP_SENDER_MAIN, 649);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_dungeon_naxxramas_heroic:24:24:-1:0|t Naxxramas", GOSSIP_SENDER_MAIN, 533);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_dungeon_outland_heroic:24:24:-1:0|t The Eye", GOSSIP_SENDER_MAIN, 550);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_dungeon_outland_heroic:24:24:-1:0|t Black Temple", GOSSIP_SENDER_MAIN, 564);
        
        SendGossipMenuFor(player, creature->GetEntry(), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        
        if (action >= 1000000) // Hero details
        {
            uint32 recordId = action - 1000000;
            ShowHeroDetails(player, creature, recordId);
            return true;
        }
        else if (action >= 100000) // Record details  
        {
            uint32 bossEntry = action - 100000;
            ShowBossRecords(player, creature, bossEntry);
            return true;
        }
        else if (action >= 1000) // Back to instance
        {
            uint32 mapId = action - 1000;
            ShowInstanceBosses(player, creature, mapId);
            return true;
        }
        else if (action == 99) // Back to main
        {
            OnGossipHello(player, creature);
            return true;
        }
        else // Instance selection
        {
            ShowInstanceBosses(player, creature, action);
            return true;
        }
        
        return true;
    }

private:
    void ShowInstanceBosses(Player* player, Creature* creature, uint32 mapId)
    {
        // Get instance name
        std::string instanceName = GetInstanceName(mapId);
        
        // Get bosses with records in this instance
        QueryResult result = WorldDatabase.Query(
            "SELECT DISTINCT boss_entry, boss_name FROM boss_kill_records WHERE map_id = {} ORDER BY boss_name", mapId
        );

        if (!result)
        {
            ChatHandler(player->GetSession()).SendSysMessage(("No records found for " + instanceName + "!").c_str());
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_arrowleft:24:24:-1:0|t Back to Instances", GOSSIP_SENDER_MAIN, 99);
            SendGossipMenuFor(player, creature->GetEntry(), creature->GetGUID());
            return;
        }

        ChatHandler(player->GetSession()).SendSysMessage(("|cffFFD700=== " + instanceName + " BOSSES ===|r").c_str());
        ChatHandler(player->GetSession()).SendSysMessage("Click on a boss to view records:");
        ChatHandler(player->GetSession()).SendSysMessage(" ");

        do
        {
            Field* fields = result->Fetch();
            uint32 bossEntry = fields[0].Get<uint32>();
            std::string bossName = fields[1].Get<std::string>();
            
            AddGossipItemFor(player, GOSSIP_ICON_BATTLE, ("|TInterface\\icons\\inv_misc_head_dragon_red:24:24:-1:0|t " + bossName).c_str(), GOSSIP_SENDER_MAIN, 100000 + bossEntry);
            
        } while (result->NextRow());

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_arrowleft:24:24:-1:0|t Back to Instances", GOSSIP_SENDER_MAIN, 99);
        SendGossipMenuFor(player, creature->GetEntry(), creature->GetGUID());
    }

    void ShowBossRecords(Player* player, Creature* creature, uint32 bossEntry)
    {
        QueryResult result = WorldDatabase.Query(
            "SELECT boss_name, kill_time, difficulty, player_name, guild_name, kill_date, "
            "CASE difficulty "
            "   WHEN 2 THEN '10 Heroic' "
            "   WHEN 3 THEN '25 Heroic' "
            "   ELSE 'Unknown' "
            "END as diff_text, map_id "
            "FROM boss_kill_records "
            "WHERE boss_entry = {} "
            "ORDER BY difficulty, kill_time ASC", bossEntry
        );

        if (!result)
        {
            ChatHandler(player->GetSession()).SendSysMessage("No records found for this boss!");
            return;
        }

        Field* firstRecord = result->Fetch();
        std::string bossName = firstRecord[0].Get<std::string>();
        uint32 mapId = firstRecord[7].Get<uint32>();
        
        ChatHandler(player->GetSession()).SendSysMessage(("|cffFFD700=== " + bossName + " RECORDS ===|r").c_str());
        ChatHandler(player->GetSession()).SendSysMessage("Click on a record to see heroes:");
        ChatHandler(player->GetSession()).SendSysMessage(" ");

        // Reset result to beginning
        result = WorldDatabase.Query(
            "SELECT boss_name, kill_time, difficulty, player_name, guild_name, kill_date, "
            "CASE difficulty "
            "   WHEN 2 THEN '10 Heroic' "
            "   WHEN 3 THEN '25 Heroic' "
            "   ELSE 'Unknown' "
            "END as diff_text, map_id "
            "FROM boss_kill_records "
            "WHERE boss_entry = {} "
            "ORDER BY difficulty, kill_time ASC", bossEntry
        );
        
        do
        {
            Field* fields = result->Fetch();
            uint32 killTime = fields[1].Get<uint32>();
            std::string playerName = fields[3].Get<std::string>();
            std::string guildName = fields[4].Get<std::string>();
            std::string diffText = fields[6].Get<std::string>();
            
            uint32 minutes = killTime / 60;
            uint32 seconds = killTime % 60;
            
            std::ostringstream recordText;
            recordText << "|TInterface\\icons\\inv_misc_stopwatch:24:24:-1:0|t " << diffText << " - " << minutes << "m " << seconds << "s";
            recordText << " by " << playerName << " <" << guildName << ">";
            
            // Create unique action ID for this record
            uint32 actionId = 1000000 + (bossEntry * 10) + fields[2].Get<uint32>(); // boss_entry * 10 + difficulty
            
            AddGossipItemFor(player, GOSSIP_ICON_DOT, recordText.str().c_str(), GOSSIP_SENDER_MAIN, actionId);
            
        } while (result->NextRow());

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_arrowleft:24:24:-1:0|t Back to Bosses", GOSSIP_SENDER_MAIN, 1000 + mapId);
        SendGossipMenuFor(player, creature->GetEntry(), creature->GetGUID());
    }

    void ShowHeroDetails(Player* player, Creature* creature, uint32 recordId)
    {
        // Decode recordId to get boss_entry and difficulty
        uint32 difficulty = recordId % 10;
        uint32 bossEntry = (recordId - difficulty) / 10;
        
        // Get record details
        QueryResult recordResult = WorldDatabase.Query(
            "SELECT boss_name, kill_time, player_name, guild_name, map_id, instance_id, "
            "CASE difficulty "
            "   WHEN 2 THEN '10 Heroic' "
            "   WHEN 3 THEN '25 Heroic' "
            "   ELSE 'Unknown' "
            "END as diff_text "
            "FROM boss_kill_records "
            "WHERE boss_entry = {} AND difficulty = {}", bossEntry, difficulty
        );

        if (!recordResult)
        {
            ChatHandler(player->GetSession()).SendSysMessage("Record not found!");
            return;
        }

        Field* recordFields = recordResult->Fetch();
        std::string bossName = recordFields[0].Get<std::string>();
        uint32 killTime = recordFields[1].Get<uint32>();
        std::string playerName = recordFields[2].Get<std::string>();
        std::string guildName = recordFields[3].Get<std::string>();
        uint32 mapId = recordFields[4].Get<uint32>();
        std::string diffText = recordFields[6].Get<std::string>();
        
        uint32 minutes = killTime / 60;
        uint32 seconds = killTime % 60;

        ChatHandler(player->GetSession()).SendSysMessage(("|cffFFD700=== " + bossName + " HEROES ===|r").c_str());
        ChatHandler(player->GetSession()).SendSysMessage(("|cffFFA500Record: " + diffText + " - " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s|r").c_str());
        ChatHandler(player->GetSession()).SendSysMessage(("|cff87CEEB" + playerName + "|r <|cff98FB98" + guildName + "|r>").c_str());
        ChatHandler(player->GetSession()).SendSysMessage(" ");
        
        // Get group composition
        QueryResult groupResult = WorldDatabase.Query(
            "SELECT player_name, player_class, player_role, is_alive "
            "FROM boss_kill_group_composition "
            "WHERE boss_entry = {} AND difficulty = {} "
            "ORDER BY player_role DESC, player_name ASC", bossEntry, difficulty
        );

        if (!groupResult)
        {
            ChatHandler(player->GetSession()).SendSysMessage("|cffC0C0C0No group composition data available for this record.|r");
        }
        else
        {
            ChatHandler(player->GetSession()).SendSysMessage("|cffFF6B6B[Group Composition]|r");
            
            // Separate by roles
            std::vector<std::string> tanks, healers, dps;
            
            do
            {
                Field* groupFields = groupResult->Fetch();
                std::string heroName = groupFields[0].Get<std::string>();
                uint8 heroClass = groupFields[1].Get<uint8>();
                std::string heroRole = groupFields[2].Get<std::string>();
                bool heroAlive = groupFields[3].Get<bool>();
                
                std::string className = GetClassName(heroClass);
                std::string aliveStatus = heroAlive ? "|cff00FF00●|r" : "|cffFF0000●|r";
                std::string heroLine = "  " + GetRoleIcon(heroRole) + " " + heroName + " - " + className + " " + aliveStatus;
                
                if (heroRole == "Tank")
                    tanks.push_back(heroLine);
                else if (heroRole == "Healer")
                    healers.push_back(heroLine);
                else
                    dps.push_back(heroLine);
                    
            } while (groupResult->NextRow());
            
            // Display by role
            if (!tanks.empty())
            {
                ChatHandler(player->GetSession()).SendSysMessage("|cff8FBC8FTanks:|r");
                for (const auto& tank : tanks)
                    ChatHandler(player->GetSession()).SendSysMessage(tank.c_str());
                ChatHandler(player->GetSession()).SendSysMessage(" ");
            }
            
            if (!healers.empty())
            {
                ChatHandler(player->GetSession()).SendSysMessage("|cff40E0D0Healers:|r");
                for (const auto& healer : healers)
                    ChatHandler(player->GetSession()).SendSysMessage(healer.c_str());
                ChatHandler(player->GetSession()).SendSysMessage(" ");
            }
            
            if (!dps.empty())
            {
                ChatHandler(player->GetSession()).SendSysMessage("|cffDDA0DDDamage Dealers:|r");
                for (const auto& damage : dps)
                    ChatHandler(player->GetSession()).SendSysMessage(damage.c_str());
                ChatHandler(player->GetSession()).SendSysMessage(" ");
            }
            
            ChatHandler(player->GetSession()).SendSysMessage("|cff00FF00●|r = Alive  |cffFF0000●|r = Dead");
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_arrowleft:24:24:-1:0|t Back to Records", GOSSIP_SENDER_MAIN, 100000 + bossEntry);
        SendGossipMenuFor(player, creature->GetEntry(), creature->GetGUID());
    }

    std::string GetClassName(uint8 classId)
    {
        switch (classId)
        {
            case 1: return "Warrior";
            case 2: return "Paladin";
            case 3: return "Hunter";
            case 4: return "Rogue";
            case 5: return "Priest";
            case 6: return "Death Knight";
            case 7: return "Shaman";
            case 8: return "Mage";
            case 9: return "Warlock";
            case 11: return "Druid";
            default: return "Unknown";
        }
    }

    std::string GetRoleIcon(const std::string& role)
    {
        if (role == "Tank")
            return "|TInterface\\icons\\inv_shield_06:16:16:-1:0|t";
        else if (role == "Healer")
            return "|TInterface\\icons\\spell_holy_heal:16:16:-1:0|t";
        else
            return "|TInterface\\icons\\inv_sword_04:16:16:-1:0|t";
    }

    std::string GetInstanceName(uint32 mapId)
    {
        switch (mapId)
        {
            case 631: return "Icecrown Citadel";
            case 603: return "Ulduar";
            case 649: return "Trial of the Crusader";
            case 533: return "Naxxramas";
            case 550: return "The Eye";
            case 564: return "Black Temple";
            default: return "Unknown Instance";
        }
    }
};

void AddBossRecordsLeaderboardScripts()
{
    new npc_boss_records_leaderboard();
}
