#include "Chat.h"
#include "Config.h"
#include "Guild.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "DatabaseEnv.h"
#include <sstream>

class npc_guild_points_leaderboard : public CreatureScript
{
public:
    npc_guild_points_leaderboard() : CreatureScript("npc_guild_points_leaderboard") { }

    struct npc_guild_points_leaderboardAI : public ScriptedAI
    {
        npc_guild_points_leaderboardAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_guild_points_leaderboardAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ClearGossipMenuFor(player);
        
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_guild_level25:24:24:-1:0|t Top Guilds (Total Points)", GOSSIP_SENDER_MAIN, 1);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_guild_level10:24:24:-1:0|t Top Guilds (Monthly Points)", GOSSIP_SENDER_MAIN, 2);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_guild_level15:24:24:-1:0|t Top Guilds (Most Records)", GOSSIP_SENDER_MAIN, 3);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\achievement_guild_level20:24:24:-1:0|t Top Guilds (First Kills)", GOSSIP_SENDER_MAIN, 4);
        
        if (player->GetGuild())
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_note_01:24:24:-1:0|t My Guild Stats", GOSSIP_SENDER_MAIN, 10);
        }
        
        SendGossipMenuFor(player, creature->GetEntry(), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        
        switch (action)
        {
            case 1:
                ShowTopGuilds(player, creature, "total_points", "Total Points");
                break;
            case 2:
                ShowTopGuilds(player, creature, "monthly_points", "Monthly Points");
                break;
            case 3:
                ShowTopGuilds(player, creature, "total_records", "Records Held");
                break;
            case 4:
                ShowTopGuilds(player, creature, "first_kills", "First Kills");
                break;
            case 10:
                ShowMyGuildStats(player, creature);
                break;
            case 99:
                OnGossipHello(player, creature);
                break;
        }
        
        return true;
    }

private:
    void ShowTopGuilds(Player* player, Creature* creature, const std::string& orderBy, const std::string& title)
    {
        QueryResult result = WorldDatabase.Query(
            "SELECT guild_name, total_points, monthly_points, total_records, first_kills "
            "FROM guild_boss_points "
            "ORDER BY {} DESC "
            "LIMIT 10", orderBy.c_str()
        );

        if (!result)
        {
            ChatHandler(player->GetSession()).SendSysMessage("No guild data found!");
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_arrowleft:24:24:-1:0|t Back", GOSSIP_SENDER_MAIN, 99);
            SendGossipMenuFor(player, creature->GetEntry(), creature->GetGUID());
            return;
        }

        ChatHandler(player->GetSession()).SendSysMessage(("|cffFFD700=== TOP GUILDS: " + title + " ===|r").c_str());
        ChatHandler(player->GetSession()).SendSysMessage(" ");

        uint32 rank = 1;
        do
        {
            Field* fields = result->Fetch();
            std::string guildName = fields[0].Get<std::string>();
            uint32 totalPoints = fields[1].Get<uint32>();
            uint32 monthlyPoints = fields[2].Get<uint32>();
            uint32 totalRecords = fields[3].Get<uint32>();
            uint32 firstKills = fields[4].Get<uint32>();
            
            std::string rankIcon = GetRankIcon(rank);
            std::ostringstream guildInfo;
            
            if (orderBy == "total_points")
                guildInfo << rankIcon << " |cff98FB98" << guildName << "|r - |cffFFD700" << totalPoints << " points|r";
            else if (orderBy == "monthly_points")
                guildInfo << rankIcon << " |cff98FB98" << guildName << "|r - |cff87CEEB" << monthlyPoints << " monthly|r";
            else if (orderBy == "total_records")
                guildInfo << rankIcon << " |cff98FB98" << guildName << "|r - |cffFFA500" << totalRecords << " records|r";
            else if (orderBy == "first_kills")
                guildInfo << rankIcon << " |cff98FB98" << guildName << "|r - |cffFF6B6B" << firstKills << " first kills|r";
            
            ChatHandler(player->GetSession()).SendSysMessage(guildInfo.str().c_str());
            
            // Show additional stats for top 3
            if (rank <= 3)
            {
                std::ostringstream extraInfo;
                extraInfo << "    |cff90EE90Total: " << totalPoints << " | Monthly: " << monthlyPoints 
                         << " | Records: " << totalRecords << " | First Kills: " << firstKills << "|r";
                ChatHandler(player->GetSession()).SendSysMessage(extraInfo.str().c_str());
            }
            
            rank++;
            
        } while (result->NextRow());

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_arrowleft:24:24:-1:0|t Back to Menu", GOSSIP_SENDER_MAIN, 99);
        SendGossipMenuFor(player, creature->GetEntry(), creature->GetGUID());
    }

    void ShowMyGuildStats(Player* player, Creature* creature)
    {
        Guild* guild = player->GetGuild();
        if (!guild)
        {
            ChatHandler(player->GetSession()).SendSysMessage("You are not in a guild!");
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_arrowleft:24:24:-1:0|t Back", GOSSIP_SENDER_MAIN, 99);
            SendGossipMenuFor(player, creature->GetEntry(), creature->GetGUID());
            return;
        }

        uint32 guildId = guild->GetId();
        std::string guildName = guild->GetName();

        // Get guild stats
        QueryResult guildResult = WorldDatabase.Query(
            "SELECT total_points, monthly_points, total_records, first_kills "
            "FROM guild_boss_points "
            "WHERE guild_id = {}", guildId
        );

        ChatHandler(player->GetSession()).SendSysMessage(("|cffFFD700=== " + guildName + " STATISTICS ===|r").c_str());
        ChatHandler(player->GetSession()).SendSysMessage(" ");

        if (!guildResult)
        {
            ChatHandler(player->GetSession()).SendSysMessage("|cffC0C0C0No boss kill data found for your guild.|r");
        }
        else
        {
            Field* fields = guildResult->Fetch();
            uint32 totalPoints = fields[0].Get<uint32>();
            uint32 monthlyPoints = fields[1].Get<uint32>();
            uint32 totalRecords = fields[2].Get<uint32>();
            uint32 firstKills = fields[3].Get<uint32>();

            ChatHandler(player->GetSession()).SendSysMessage(("|cffFFD700Total Points: |cff98FB98" + std::to_string(totalPoints) + "|r").c_str());
            ChatHandler(player->GetSession()).SendSysMessage(("|cff87CEEBMonthly Points: |cff98FB98" + std::to_string(monthlyPoints) + "|r").c_str());
            ChatHandler(player->GetSession()).SendSysMessage(("|cffFFA500Records Held: |cff98FB98" + std::to_string(totalRecords) + "|r").c_str());
            ChatHandler(player->GetSession()).SendSysMessage(("|cffFF6B6BFirst Kills: |cff98FB98" + std::to_string(firstKills) + "|r").c_str());
            ChatHandler(player->GetSession()).SendSysMessage(" ");

            // Get rank among all guilds
            QueryResult rankResult = WorldDatabase.Query(
                "SELECT COUNT(*) + 1 FROM guild_boss_points WHERE total_points > "
                "(SELECT total_points FROM guild_boss_points WHERE guild_id = {})", guildId
            );

            if (rankResult)
            {
                uint32 rank = rankResult->Fetch()->Get<uint32>();
                ChatHandler(player->GetSession()).SendSysMessage(("|cffFFD700Server Rank: |cff98FB98#" + std::to_string(rank) + "|r").c_str());
            }
        }

        // Show recent achievements
        QueryResult historyResult = WorldDatabase.Query(
            "SELECT bkr.boss_name, gph.points_awarded, gph.reason, gph.award_date "
            "FROM guild_points_history gph "
            "LEFT JOIN boss_kill_records bkr ON gph.boss_entry = bkr.boss_entry AND gph.difficulty = bkr.difficulty "
            "WHERE gph.guild_id = {} "
            "ORDER BY gph.award_date DESC "
            "LIMIT 5", guildId
        );

        if (historyResult)
        {
            ChatHandler(player->GetSession()).SendSysMessage(" ");
            ChatHandler(player->GetSession()).SendSysMessage("|cffFFA500Recent Achievements:|r");

            do
            {
                Field* historyFields = historyResult->Fetch();
                std::string bossName = historyFields[0].Get<std::string>();
                uint32 points = historyFields[1].Get<uint32>();
                std::string reason = historyFields[2].Get<std::string>();

                std::ostringstream achievementInfo;
                achievementInfo << "  |cff98FB98+" << points << " pts|r - " << reason << " (" << bossName << ")";
                ChatHandler(player->GetSession()).SendSysMessage(achievementInfo.str().c_str());

            } while (historyResult->NextRow());
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_arrowleft:24:24:-1:0|t Back to Menu", GOSSIP_SENDER_MAIN, 99);
        SendGossipMenuFor(player, creature->GetEntry(), creature->GetGUID());
    }

    std::string GetRankIcon(uint32 rank)
    {
        switch (rank)
        {
            case 1: return "|TInterface\\icons\\inv_misc_ribbon_01:16:16:-1:0|t|cffFFD700#1|r";
            case 2: return "|TInterface\\icons\\inv_jewelry_talisman_12:16:16:-1:0|t|cffC0C0C0#2|r";
            case 3: return "|TInterface\\icons\\inv_jewelry_talisman_11:16:16:-1:0|t|cffCD7F32#3|r";
            default: return "|cffFFFFFF#" + std::to_string(rank) + "|r";
        }
    }
};

void AddGuildPointsLeaderboardScripts()
{
    new npc_guild_points_leaderboard();
}
