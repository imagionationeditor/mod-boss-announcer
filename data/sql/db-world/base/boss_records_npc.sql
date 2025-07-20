-- Boss Records Leaderboard NPC
DELETE FROM `creature_template` WHERE `entry` = 190010;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `speed_swim`, `speed_flight`, `detection_range`, `scale`, `rank`, `dmgschool`, `DamageModifier`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `ctm1`, `ctm2`, `ctm3`, `ctm4`, `ctm5`, `ctm6`, `ctm7`, `ctm8`, `InhabitType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(190010, 0, 0, 0, 0, 0, 28731, 0, 0, 0, 'Boss Records Keeper', 'Heroic Kill Records & Leaderboards', 'Speak', 0, 80, 80, 2, 35, 1, 1, 1.14286, 1, 1, 20, 1, 0, 0, 1, 2000, 2000, 1, 1, 1, 33536, 2048, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'SmartAI', 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 2, 'npc_boss_records_leaderboard', 12340);

-- Spawn Boss Records Keeper NPCs in major cities
DELETE FROM `creature` WHERE `id1` = 190010;
INSERT INTO `creature` (`guid`, `id1`, `id2`, `id3`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`, `ScriptName`, `VerifiedBuild`) VALUES
-- Stormwind City - Cathedral Square
(800011, 190010, 0, 0, 0, 1519, 1519, 1, 1, 0, -8853.11, 664.031, 96.9956, 1.06465, 300, 0, 0, 8982, 0, 0, 0, 0, 0, '', 0),
-- Orgrimmar - Valley of Strength  
(800012, 190010, 0, 0, 1, 1637, 1637, 1, 1, 0, 1569.59, -4397.12, 16.0472, 1.69297, 300, 0, 0, 8982, 0, 0, 0, 0, 0, '', 0),
-- Dalaran - Krasus' Landing
(800013, 190010, 0, 0, 571, 4395, 4395, 1, 1, 0, 5813.64, 450.062, 658.75, 0.001747, 300, 0, 0, 8982, 0, 0, 0, 0, 0, '', 0),
-- Shattrath City - Lower City
(800014, 190010, 0, 0, 530, 3703, 3703, 1, 1, 0, -1822.85, 5299.43, -12.4281, 5.42797, 300, 0, 0, 8982, 0, 0, 0, 0, 0, '', 0),
-- Ironforge - The Commons
(800015, 190010, 0, 0, 0, 1537, 1537, 1, 1, 0, -4924.97, -951.472, 501.526, 5.44543, 300, 0, 0, 8982, 0, 0, 0, 0, 0, '', 0);

-- Set NPC as gossip creature
UPDATE `creature_template` SET `npcflag` = 1 WHERE `entry` = 190010;
