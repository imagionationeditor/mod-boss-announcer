-- Guild Points Leaderboard NPC
DELETE FROM `creature_template` WHERE `entry` = 190020;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `speed_swim`, `speed_flight`, `detection_range`, `scale`, `rank`, `dmgschool`, `DamageModifier`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `ctm1`, `ctm2`, `ctm3`, `ctm4`, `ctm5`, `ctm6`, `ctm7`, `ctm8`, `InhabitType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(190020, 0, 0, 0, 0, 0, 25902, 0, 0, 0, 'Guild Points Keeper', 'Boss Achievement Rankings', 'Speak', 0, 80, 80, 2, 35, 1, 1, 1.14286, 1, 1, 20, 1, 0, 0, 1, 2000, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 'npc_guild_points_leaderboard', 12340);

-- Example spawn locations (you can modify coordinates as needed)
DELETE FROM `creature` WHERE `id1` = 190020;
INSERT INTO `creature` (`guid`, `id1`, `id2`, `id3`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`, `ScriptName`, `VerifiedBuild`) VALUES
-- Stormwind City
(800020, 190020, 0, 0, 0, 0, 0, 1, 1, 0, -8842.09, 626.358, 94.0066, 3.61661, 300, 0, 0, 8982, 0, 0, 0, 0, 0, '', 0),
-- Orgrimmar  
(800021, 190020, 0, 0, 1, 0, 0, 1, 1, 0, 1633.75, -4439.5, 15.7588, 2.42251, 300, 0, 0, 8982, 0, 0, 0, 0, 0, '', 0),
-- Dalaran
(800022, 190020, 0, 0, 571, 0, 0, 1, 1, 0, 5804.15, 624.771, 647.767, 1.27409, 300, 0, 0, 8982, 0, 0, 0, 0, 0, '', 0);
