-- Boss Kill Records Table (Heroic Only)
CREATE TABLE IF NOT EXISTS `boss_kill_records` (
  `boss_entry` int(10) unsigned NOT NULL COMMENT 'Boss creature entry',
  `boss_name` varchar(100) NOT NULL COMMENT 'Boss name',
  `map_id` int(10) unsigned NOT NULL COMMENT 'Map ID',
  `instance_id` int(10) unsigned NOT NULL COMMENT 'Instance ID',
  `difficulty` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '2=10heroic, 3=25heroic (only heroic modes tracked)',
  `kill_time` int(10) unsigned NOT NULL COMMENT 'Kill time in seconds from instance start',
  `player_name` varchar(50) NOT NULL COMMENT 'Player or group leader name',
  `guild_name` varchar(100) DEFAULT NULL COMMENT 'Guild name',
  `group_size` tinyint(3) unsigned NOT NULL COMMENT 'Number of players in group',
  `alive_players` tinyint(3) unsigned NOT NULL COMMENT 'Number of alive players at kill',
  `kill_date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'When the kill happened',
  PRIMARY KEY (`boss_entry`, `difficulty`),
  UNIQUE KEY `idx_boss_difficulty` (`boss_entry`, `difficulty`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Fast kill records for bosses (Heroic only)';

-- Boss Kill Group Composition Table
CREATE TABLE IF NOT EXISTS `boss_kill_group_composition` (
  `boss_entry` int(10) unsigned NOT NULL COMMENT 'Boss creature entry from boss_kill_records',
  `difficulty` tinyint(3) unsigned NOT NULL COMMENT 'Difficulty from boss_kill_records',
  `player_name` varchar(50) NOT NULL COMMENT 'Player name',
  `player_class` tinyint(3) unsigned NOT NULL COMMENT 'Player class (1=Warrior, 2=Paladin, etc.)',
  `player_role` varchar(20) NOT NULL COMMENT 'Tank, Healer, or DPS',
  `is_alive` tinyint(1) NOT NULL DEFAULT '1' COMMENT 'Was player alive at boss kill',
  PRIMARY KEY (`boss_entry`, `difficulty`, `player_name`),
  FOREIGN KEY (`boss_entry`, `difficulty`) REFERENCES `boss_kill_records`(`boss_entry`, `difficulty`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Group composition for boss kill records';

-- Guild Boss Points Table
CREATE TABLE IF NOT EXISTS `guild_boss_points` (
  `guild_id` int(10) unsigned NOT NULL COMMENT 'Guild ID',
  `guild_name` varchar(100) NOT NULL COMMENT 'Guild name',
  `total_points` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Total points earned',
  `total_records` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Number of records held',
  `first_kills` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Number of first kills',
  `monthly_points` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Points earned this month',
  `last_updated` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`guild_id`),
  UNIQUE KEY `idx_guild_name` (`guild_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Guild points for boss achievements';

-- Guild Points History Table
CREATE TABLE IF NOT EXISTS `guild_points_history` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `guild_id` int(10) unsigned NOT NULL COMMENT 'Guild ID',
  `boss_entry` int(10) unsigned NOT NULL COMMENT 'Boss creature entry',
  `difficulty` tinyint(3) unsigned NOT NULL COMMENT 'Difficulty mode',
  `points_awarded` int(10) unsigned NOT NULL COMMENT 'Points awarded',
  `reason` varchar(50) NOT NULL COMMENT 'First Kill, New Record, Big Improvement',
  `award_date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `idx_guild_id` (`guild_id`),
  KEY `idx_boss_difficulty` (`boss_entry`, `difficulty`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='History of guild point awards';
