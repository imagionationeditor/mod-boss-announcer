# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore
## mod-boss-announcer
- Latest build status with azerothcore: [![Build Status](https://github.com/azerothcore/mod-boss-announcer/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-boss-announcer)

# 🏆 Boss Announcer - Advanced Raid Competition Module

A comprehensive competitive raiding system for AzerothCore that transforms boss announcements into an engaging guild competition with speed records, point systems, and detailed tracking.

## 📋 **Original Foundation**
This module builds upon the basic boss announcer functionality and has been **extensively enhanced and redesigned** to create a complete competitive raiding ecosystem.

**Original Script**: Basic boss kill announcements by talamortis  
**Major Enhancement**: Fast Kill Record System, Guild Points, and Role Detection by **Mojispectre**

---

## 🚀 **Core Features**

### **1. Enhanced Boss Kill Announcements**
- **Rich Formatting**: Color-coded announcements with guild, player, and boss information
- **Advanced Role Detection**: Sophisticated talent-based system for accurate Tank/Healer/DPS classification
- **Group Composition**: Real-time counting of alive players and role distribution
- **Difficulty Indicators**: Clear Normal/Heroic and 10/25-man identification

### **2. Fast Kill Record System (Heroic Only)**
- **Heroic Exclusivity**: Only tracks speed records for Heroic difficulty content
- **Automatic Tracking**: Real-time kill time measurement and comparison
- **Record Announcements**: Special "NEW FASTEST KILL RECORD!" notifications
- **Detailed Storage**: Complete group composition saved with each record
- **Anti-Cheat**: Multiple validation systems to ensure legitimate records

### **3. Competitive Guild Points System**
- **Dynamic Scoring**: Points awarded based on difficulty and achievement type
- **Multiple Achievement Categories**:
  - 🥇 **First Kill** (200 pts): Server-first boss defeats
  - ⚡ **New Record** (100 pts): Setting new speed records
  - 🗡️ **Heroic 25-man** (100 pts): Most challenging content
  - ⚔️ **Heroic 10-man** (50 pts): Heroic difficulty small group
  - 🛡️ **Normal 25-man** (20 pts): Large group raids
  - ⚡ **Normal 10-man** (10 pts): Standard small group content
- **Strict Validation**: Anti-abuse systems ensuring legitimate achievements
- **Public Recognition**: Point awards announced server-wide

### **4. Interactive Leaderboard System**
- **Hierarchical Navigation**: Instance → Boss → Records → Heroes
- **Dual NPCs**: Separate interfaces for records and guild rankings
- **Comprehensive Data**: Kill times, group composition, achievement dates
- **Strategic Locations**: NPCs placed in all major cities

---

## 🎯 **Advanced Role Detection System**

### **Death Knight Tank Detection**
Analyzes specific tanking talents and abilities:
```
✅ Toughness               ✅ Vampiric Blood
✅ Will of the Necropolis  ✅ Improved Rune Tap
✅ Anticipation            ✅ Blood of the North
✅ Acclimation             ✅ Improved Frost Presence
✅ Spell Deflection
```

**Logic**: 3+ tank talents = Tank, 2+ talents + Blood abilities = Tank

### **Multi-Role Class Support**
- **Warriors**: Protection talent analysis with Shield Slam, Taunt detection
- **Paladins**: Holy vs Protection specialization with tanking ability checks
- **Druids**: Bear vs Cat form distinction using specific abilities
- **Shamans**: Enhancement/Elemental vs Restoration talent distribution
- **Priests**: Shadow vs Holy/Discipline specialization

---

## 📊 **Database Architecture**

### **Boss Records Storage**
```sql
boss_kill_records:
- Complete kill information (time, difficulty, group size)
- Guild and player attribution
- Instance and boss identification

boss_kill_group_composition:
- Individual player details
- Class and role information  
- Alive/dead status at kill time
```

### **Guild Competition System**
```sql
guild_boss_points:
- Total accumulated points
- Monthly point tracking
- Record count and first kill statistics

guild_points_history:
- Complete audit trail
- Point award reasons and dates
- Boss-specific achievement tracking
```

---

## ⚙️ **Configuration System**

### **Core Settings**
```ini
# Module Control
Boss.Announcer.Enable = 1
Boss.Announcer.Announce = 1
Boss.Announcer.RemoveAuraUponKill = 0

# Fast Kill Records (Heroic Only)
Boss.Announcer.FastKillRecord.Enable = 1
Boss.Announcer.FastKillRecord.Announce = 1

# Interactive Leaderboards
Boss.Announcer.Leaderboard.Enable = 1
```

### **Guild Points Configuration**
```ini
# System Control
Boss.Announcer.GuildPoints.Enable = 1
Boss.Announcer.GuildPoints.Announce = 1

# Achievement Values
Boss.Announcer.GuildPoints.FirstKill = 200
Boss.Announcer.GuildPoints.NewRecord = 100
Boss.Announcer.GuildPoints.Heroic25 = 100
Boss.Announcer.GuildPoints.Heroic10 = 50
Boss.Announcer.GuildPoints.Normal25 = 20
Boss.Announcer.GuildPoints.Normal10 = 10

# Validation Rules
Boss.Announcer.GuildPoints.RequireSameGuild = 1
Boss.Announcer.GuildPoints.RequireOnline = 1
Boss.Announcer.GuildPoints.MaxDistance = 100
```

---

## 🎮 **Example Scenarios**

### **Scenario 1: Server First Heroic Lich King**
```
🏆 Arthas's Guild <Legends> has slain [The Lich King] with remaining 23/25 players alive on Heroic mode, possible group Tank: 2 Healers: 6 DPS: 17 [+300 Guild Points - First Kill]
```
*Awards: 200 (First Kill) + 100 (Heroic 25) = 300 points*

### **Scenario 2: New Speed Record**
```
⚡ SpeedRunner's Guild <Velocity> has slain [Sindragosa] with remaining 25/25 players alive on Heroic mode, possible group Tank: 2 Healers: 5 DPS: 18 *** NEW FASTEST KILL RECORD! *** [+200 Guild Points - New Record]
```
*Awards: 100 (New Record) + 100 (Heroic 25) = 200 points*

### **Scenario 3: Regular Progression**
```
🛡️ Player1's Guild <Progression> has slain [Lord Marrowgar] with remaining 22/25 players alive on Normal mode, possible group Tank: 2 Healers: 6 DPS: 17 [+20 Guild Points - Boss Kill (Normal 25)]
```

---

## 🛠️ **Installation Guide**

### **1. File Setup**
```bash
# Copy module to AzerothCore modules directory
cp -r mod-boss-announcer/ /path/to/azerothcore/modules/

# Execute SQL files
mysql -u username -p world_database < data/sql/db-world/base/boss_kill_records.sql
mysql -u username -p world_database < data/sql/db-world/base/boss_records_npc.sql  
mysql -u username -p world_database < data/sql/db-world/base/guild_points_npc.sql
```

### **2. Compilation**
```bash
# Recompile AzerothCore with module
cd /path/to/azerothcore/build
make -j$(nproc)
```

### **3. Configuration**
```bash
# Copy and edit configuration file
cp mod_boss_announcer_example.conf worldserver.conf.d/mod_boss_announcer.conf
# Edit settings as needed
```

---

## 🏛️ **NPC Locations**

### **Boss Records Keeper** (ID: 190010)
- **Stormwind City**: Cathedral Square
- **Orgrimmar**: Valley of Strength  
- **Dalaran**: Krasus' Landing
- **Ironforge**: The Commons
- **Shattrath City**: Lower City

### **Guild Points Keeper** (ID: 190020)
- Same locations as Boss Records Keeper
- Provides guild rankings and personal statistics

---

## 🔧 **Technical Implementation**

### **Role Detection Algorithm**
- **Talent Analysis**: Checks talent point distribution across specialization trees
- **Spell Verification**: Validates class-specific abilities and talents
- **Multi-Factor Decision**: Combines multiple indicators for accurate classification
- **Fallback Systems**: Graceful degradation when information is incomplete

### **Record Validation**
- **Time Verification**: Multiple timestamp checks for accuracy
- **Group Validation**: Ensures legitimate group composition
- **Distance Checking**: Anti-exploit proximity requirements
- **Guild Verification**: Same-guild requirements for competitive integrity

---

## 📈 **Competitive Features**

### **Guild Competition Elements**
- **Monthly Leaderboards**: Regular competition cycles
- **Achievement Diversity**: Multiple paths to guild prominence
- **Public Recognition**: Server-wide announcements for major achievements
- **Historical Tracking**: Complete audit trail of guild accomplishments

### **Anti-Abuse Systems**
- **Same Guild Requirement**: Prevents point farming across guilds
- **Online Validation**: Ensures active participation
- **Proximity Checking**: Prevents remote assistance
- **Database Integrity**: Multiple validation layers

---

## 🎊 **What's New in This Enhanced Version**

### **Major Additions**
1. **Complete Guild Points System**: Comprehensive competitive scoring
2. **Heroic-Only Speed Records**: Focus on most challenging content  
3. **Advanced Role Detection**: Talent and spell-based classification
4. **Interactive NPCs**: User-friendly leaderboard interfaces
5. **Anti-Abuse Validation**: Multiple security layers
6. **Hierarchical Navigation**: Intuitive record browsing system

### **Code Quality Improvements**
- **Modular Design**: Clean separation of concerns
- **Database Optimization**: Efficient query patterns
- **Configuration Flexibility**: Extensive customization options
- **Error Handling**: Robust validation and fallback systems

---

## 📄 **Credits**

**Original Script**: talamortis (Basic boss announcer functionality)  
**Major Enhancement & Redesign**: **Mojispectre**
- Fast Kill Record System (Heroic Only)
- Comprehensive Guild Points System  
- Advanced Role Detection
- Interactive Leaderboard System
- Anti-Abuse Validation
- Database Architecture
- Configuration System

This module represents a complete transformation from basic announcements to a comprehensive competitive raiding ecosystem! 🏆


