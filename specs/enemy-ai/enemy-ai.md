# Spec: Enemy AI, Loot Pickup, Enemy Progression & Veterans

## Overview
Enemies, Bandits, Beastmen, raiders, and town NPCs run server-side StateTree behaviour: they patrol near home, investigate what they half-notice, fight whatever their group is hostile to, flee, return home, and raid. Enemies pick up shared items from the ground, humanoid enemies wear and wield what they pick up, and enemies gain XP and levels from fights. An enemy that gains enough levels becomes a named Veteran that is saved with the world and can lead raids. Shared terms are defined in `specs/game-foundation/game-foundation.md`.

Existing enemy content stays where it is: enemy rows, stats, spawning, bosses, damage, perception, threat, executions, and loot rolling in `specs/combat-loot/combat-loot.md`; camps, town NPCs, reputation, and raids in `specs/factions-kingdoms/factions-kingdoms.md`; StateTree, Smart Objects, Mutable, and the benchmark in `specs/engine-tech/engine-tech.md`. This spec owns AI behaviour, faction hostility, loot pickup, enemy equipment, enemy XP and levels, and Veterans, and cross-references the rest.

## Goals
- The world feels contested: enemy groups fight each other, and items left on the ground are not safe.
- An enemy that downs players or survives a long time becomes visibly stronger, and a Veteran becomes a named, persistent threat worth hunting.
- Gear a humanoid enemy picks up is visible on it, changes how it fights, and is recovered when it dies.
- AI cost stays inside the frame rate targets of `specs/game-foundation/game-foundation.md` Requirement 13, measured by the benchmark milestone (`specs/engine-tech/engine-tech.md` Requirement 11).

## Non-Goals
- Enemy row definitions, spawning, bosses and their phases, player damage formulas, executions, and loot rolling (`specs/combat-loot/combat-loot.md`).
- Camp placement, reputation, town NPC roles, and raid rolls, sources, sizes, and snapshots (`specs/factions-kingdoms/factions-kingdoms.md`).
- Enemies using consumables, tools, torches, bows, jewelry, cloaks, bags, or ammo, opening containers, or opening doors (`specs/voxel-world/voxel-world.md` Requirement 45).
- Enemies that are not raiders damaging building pieces (`specs/voxel-world/voxel-world.md` Requirement 26).
- XP, levels, looting, or equipment for `Boss` rows, `Wildlife` rows, and town NPCs.
- Saving non-Veteran enemies.
- Mass-based AI (Mass stays on the Evaluate list, `specs/engine-tech/engine-tech.md` Requirement 5).
- Changes to player-count scaling (`specs/combat-loot/combat-loot.md` Requirement 21).

## Requirements

### AI Architecture
1. Every enemy (a `DT_Combat_Enemies` row, `specs/combat-loot/combat-loot.md` Requirement 19: `Hostile` rows including Bandits, Beastmen, and kingdom raiders, `Wildlife` rows, and `Boss` rows) and every town NPC, including escort NPCs (`specs/factions-kingdoms/factions-kingdoms.md` Requirements 20 and 40), runs the StateTree asset named by its row, per the StateTree and Smart Objects row of `specs/engine-tech/engine-tech.md` Requirement 3. `UNamecEnemyAIComponent` on `ANamecEnemyBase` hosts the StateTree, the home position (Requirement 4), the current AI state (Requirement 3), perception, and the threat table. AI interactions with world objects use Smart Objects: shared world pickups (Requirement 21), NPC posts, and raider attack positions on building pieces.
2. AI runs only on the server. StateTree evaluation, perception, threat, target selection, loot and equip decisions, XP, level-ups, and Veteran promotion run on the host and nowhere else. Clients run no StateTree and no perception, and receive replicated state only: movement, montages, the current AI state (Requirement 3), level (Requirement 38), Veteran name and title (Requirement 45), equipped item visuals with dye colors (Requirement 31), and the level-up event (Requirement 40).
3. Each AI is in exactly one of these states at a time, and `UNamecEnemyAIComponent` replicates the current state:
   - **Idle/Patrol** — stays inside its home radius (Requirement 5).
   - **Investigate** — moves to a perceived but unconfirmed stimulus (Requirement 6).
   - **Combat** — fights the targets in its threat table (Requirement 7).
   - **Loot** — walks to a lootable pickup and picks it up (Requirement 23).
   - **Flee** — runs from danger (Requirement 8).
   - **Return** — walks back home after exceeding its leash distance (Requirement 9).
   - **Raid** — follows the raid rules (Requirement 10).

   Which states each kind of AI uses:

   | AI | States used |
   |----|-------------|
   | `Hostile` enemy (region monster, Bandit, Beastman, Veteran) | Idle/Patrol, Investigate, Combat, Loot, Flee (only when its row sets a flee threshold, Requirement 8), Return, and Raid while leading a raid (Requirement 52) |
   | Raider (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 56) | Raid and Loot |
   | `Wildlife` enemy | Idle/Patrol, Flee, Return |
   | `Boss` enemy | Idle/Patrol and Combat, inside its arena (`specs/combat-loot/combat-loot.md` Requirements 24–25) |
   | Guard | Idle/Patrol (standing at its post), Investigate, Combat, Return |
   | Citizen | Idle/Patrol (standing at its post), Flee (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 24), Return |
   | Vendor, Quest Giver, Guard Captain | Idle/Patrol (standing at its post), with the cower reaction of `specs/factions-kingdoms/factions-kingdoms.md` Requirement 24 |
   | Escort NPC | Idle/Patrol (following the accepting player, `specs/factions-kingdoms/factions-kingdoms.md` Requirement 66) |
4. Every AI except raiders has a home position:
   - a region monster or `Wildlife` enemy: its spawn position;
   - a camp enemy: its camp's center (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 17);
   - a town NPC: its post (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 21);
   - a Veteran: its saved home position (Requirements 46 and 53);
   - a `Boss`: its arena center.

   The home radius is 15 m (tuning value) and the leash distance is 40 m (tuning value), both measured horizontally from the home position. A Veteran's home radius and leash distance are each multiplied by VeteranRoamMultiplier, 3 (tuning value). A Guard uses its town's protected radius in place of the leash distance (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 23). A `Boss` has no leash distance and never leaves its arena.
5. **Idle/Patrol.** A `Hostile` or `Wildlife` enemy walks to a random navigable point inside its home radius, waits a random 8–20 s (tuning values), and repeats. A town NPC stands at its post. An escort NPC follows the accepting player instead. An AI leaves Idle/Patrol for Investigate on a stimulus (Requirement 6), for Combat on detecting a hostile target (Requirement 17), for Flee (`Wildlife` and Citizens, Requirement 8), or for Loot (Requirement 23).
6. **Investigate.** A stimulus is either a hostile target (Requirement 15) inside the AI's perception radius (Requirement 17) but out of its line of sight, or a hostile target in its line of sight at a distance between the perception radius and InvestigateRadiusMultiplier, 1.5 (tuning value), times the perception radius. The AI moves to the stimulus's last perceived position. Detecting a hostile target (Requirement 17) switches it to Combat. When it detects nothing within 10 s (tuning value) of reaching that position, it switches to Idle/Patrol when it is inside its home radius, otherwise to Return. An investigating AI has not detected or aggroed the target it is investigating (`specs/character-progression/character-progression.md` Requirement 21, Stealth).
7. **Combat.** An AI enters Combat when it detects a hostile target (Requirement 17) or takes damage from one. Detection adds the target to the AI's threat table (`specs/combat-loot/combat-loot.md` Requirement 22) with a detection threat of 1 (tuning value), and damage adds threat as that requirement defines. The AI attacks the target with the highest threat, and taunts force the target as usual. A target leaves the threat table when it dies, despawns, disconnects, or has been outside the AI's perception radius or line of sight for 5 s (tuning value). When the threat table is empty, the AI switches to Idle/Patrol when it is inside its home radius, otherwise to Return.
8. **Flee.** `DT_Combat_Enemies` has an optional FleeHealthThreshold column, a fraction of MaxHealth (empty means the row never flees). A `Hostile` enemy in Combat whose Health drops below its row's FleeHealthThreshold switches to Flee: it stops attacking and moves away from its highest-threat target. A `Wildlife` enemy switches to Flee whenever it detects any player, enemy, or town NPC (Requirement 17) or takes damage, whatever its health. A Citizen flees per `specs/factions-kingdoms/factions-kingdoms.md` Requirement 24, and an escort NPC never flees. An AI in Flee neither attacks nor loots, and keeps fleeing when damaged. Flee ends when the AI has detected no hostile target (for `Wildlife`, no player, enemy, or town NPC) for 8 s (tuning value), and the AI then switches to Return. Raiders ignore FleeHealthThreshold.
9. **Return.** An AI switches to Return when its horizontal distance from its home position exceeds its leash distance (Requirement 4) while in Idle/Patrol, Investigate, Combat, or Loot, when Investigate or Combat ends outside its home radius (Requirements 6 and 7), or when Flee ends (Requirement 8). On entering Return, the AI clears its threat table and releases any Smart Object claim. During Return it walks home, ignores stimuli, detection, and pickups, and still takes damage. When it reaches its home radius, it switches to Idle/Patrol, and a non-Veteran restores Health to MaxHealth and Poise to MaxPoise. A Veteran's Health is not restored (Requirement 49). When no navmesh path home exists, or the AI has not reached its home radius within 30 s (tuning value) of entering Return:
   - a non-Veteran enemy despawns where it stands and drops its picked-up items (Requirement 33). A camp enemy despawned this way is not dead, so its spawn point spawns it again the next time the camp's chunk streams in (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 18);
   - a Veteran despawns, keeps its record (Requirement 46), and spawns again at its home position (Requirement 47);
   - a town NPC is moved to its post.
10. **Raid.** A raider (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 56) and a Veteran raid leader (Requirement 52) run Raid instead of Idle/Patrol, Investigate, Combat, Flee, and Return. Raid follows `specs/combat-loot/combat-loot.md` Requirement 43 and `specs/factions-kingdoms/factions-kingdoms.md` Requirements 56–58, with two extensions: a hostile enemy or Guard (Requirement 15) that damages the raider is added to its threat table exactly as a damaging player is, and a raider or Veteran raid leader with no navigation path to its current target attacks the first building piece blocking its path, including a piece outside the raid's snapshot, without ending the raid or changing the snapshot (`specs/combat-loot/combat-loot.md` Requirement 43). A raider in Raid may enter Loot (Requirement 23) while its threat table is empty and returns to Raid when Loot ends. A retreating raider (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 58) neither attacks nor loots.
11. AI level of detail (AI LOD):
    - StateTrees use scheduled ticking (`specs/engine-tech/engine-tech.md` Requirement 3, StateTree and Smart Objects row).
    - An AI within 50 m (tuning value) of at least one player runs perception (detection, stimuli, and pickup perception) every 0.2 s (tuning value). An AI farther than that from every player runs it every 1.0 s (tuning value).
    - No AI is simulated in a chunk that is not streamed in (`specs/voxel-world/voxel-world.md` Requirement 4). AI pathing uses only streamed-in chunks. When a chunk streams out, every non-Veteran enemy in it despawns and drops its picked-up items (Requirement 33), camp states stay as saved (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 19), and every Veteran in it stops simulating (Requirement 47).

### Faction Hostility
12. Every player and AI belongs to exactly one hostility group, derived from existing data with no new column:

    | Group | Members |
    |-------|---------|
    | Player | Every player character |
    | Region Monster | `Hostile` rows with Faction `None` (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 2), including region monster raiders |
    | Wildlife | `Wildlife` rows |
    | Boss | `Boss` rows |
    | Bandits | `Hostile` rows with Faction `Bandits`, including Bandit raiders |
    | Beastmen | `Hostile` rows with Faction `Beastmen`, including Beastmen raiders |
    | Kingdom Raider | `Hostile` rows with a kingdom Faction (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 3) |
    | Guard | Town NPCs with role Guard |
    | Town NPC | Town NPCs with role Citizen, Vendor, Quest Giver, or Guard Captain, and escort NPCs |

    A Veteran stays in its row's group.
13. `DT_EnemyAI_Hostility` holds the hostility matrix: one row per AI group (every group except Player) and one column per group. Each cell is `Hostile`, `Neutral`, `Flee`, or `Reputation`, and says how an AI of the row's group treats a member of the column's group. Starting values:

    | AI group (row) → target (column) | Player | Region Monster | Wildlife | Boss | Bandits | Beastmen | Kingdom Raider | Guard | Town NPC |
    |------|------|------|------|------|------|------|------|------|------|
    | **Region Monster** | Hostile | Neutral | Neutral | Neutral | Hostile | Hostile | Neutral | Hostile | Hostile |
    | **Wildlife** | Flee | Flee | Neutral | Flee | Flee | Flee | Flee | Flee | Flee |
    | **Boss** | Hostile | Neutral | Neutral | Neutral | Neutral | Neutral | Neutral | Neutral | Neutral |
    | **Bandits** | Hostile | Hostile | Neutral | Neutral | Neutral | Hostile | Hostile | Hostile | Hostile |
    | **Beastmen** | Hostile | Hostile | Neutral | Neutral | Hostile | Neutral | Hostile | Hostile | Hostile |
    | **Kingdom Raider** | Hostile | Neutral | Neutral | Neutral | Hostile | Hostile | Neutral | Hostile | Neutral |
    | **Guard** | Reputation | Hostile | Neutral | Neutral | Hostile | Hostile | Hostile | Neutral | Neutral |
    | **Town NPC** | Neutral | Neutral | Neutral | Neutral | Neutral | Neutral | Neutral | Neutral | Neutral |

    So: Bandits and Beastmen are hostile to each other; region monsters are hostile to Bandits, Beastmen, town NPCs (Guards and the Town NPC group), and players; Guards are hostile to every `Hostile`-category group, which includes raiders; `Wildlife` is attacked only by players and flees from everyone; kingdom raiders are hostile to players, Guards, Bandits, and Beastmen.
14. A `Reputation` cell means the Guard targets players by `specs/factions-kingdoms/factions-kingdoms.md` Requirement 23 (Hostile reputation tier, damaging that Guard, and the post-fine truce). A Guard engages a raider it detects, engages any other `Hostile`-category enemy only inside its town's protected radius, and never pursues any target beyond that radius (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 23).
15. A target is **hostile** to an AI when the cell for the AI's group and the target's group is `Hostile`, or `Reputation` and the Guard rule of Requirement 14 selects that player. An AI detects, investigates, adds to its threat table, and attacks only hostile targets. A `Flee` cell makes the AI flee from that group (Requirement 8) and never attack it. A `Neutral` cell means the AI ignores that group.
16. Enemies fight hostile enemies and town NPCs with the same rules they use for players:
    - AI attacks, including area-of-effect attacks, damage only targets hostile to the attacker (Requirement 15). An AI never damages a member of its own group, so no member of a faction damages another member of the same faction. Player attacks are unchanged (`specs/combat-loot/combat-loot.md` Requirements 12 and 42), and building piece damage is unchanged (`specs/voxel-world/voxel-world.md` Requirement 26).
    - Damage an AI deals to a non-player target is the `specs/combat-loot/combat-loot.md` Requirement 38 IncomingDamage formula with BlockPercent 0 and TargetResistance from the target's `UNamecEnemyAttributeSet` values (`specs/combat-loot/combat-loot.md` Requirement 9). Poise damage and stagger follow `specs/combat-loot/combat-loot.md` Requirements 7 and 38.
    - Player-count scaling is unchanged: `specs/combat-loot/combat-loot.md` Requirement 21 applies to enemy health and damage whatever the target is, and never to town NPCs.
    - Threat from a non-player target comes from the damage it deals to the AI. Only player abilities taunt.
17. **Detection.** An AI detects a hostile target (or, for `Wildlife`, a member of a `Flee` group) that is inside its perception radius and in its line of sight (`specs/combat-loot/combat-loot.md` Requirement 41). The perception radius is the AI's row value in `DT_Combat_Enemies` or `DT_Factions_NPCs`. The crouch, Stealth, and racial multipliers of `specs/combat-loot/combat-loot.md` Requirement 41 apply only when the target is a player.
18. Kill rules do not depend on the killer. When an enemy dies to another enemy, a Guard, or the environment, loot eligibility, per-player loot and gold, KillXP, Hunt credit, camp clearing, and Clear Camp credit follow `specs/combat-loot/combat-loot.md` Requirements 28–29 and `specs/factions-kingdoms/factions-kingdoms.md` Requirements 19 and 44 unchanged, so a player within 50 m of the death or who damaged the enemy is eligible, and with no eligible player no per-player loot is rolled. A Guard or Citizen killed by an enemy respawns per `specs/factions-kingdoms/factions-kingdoms.md` Requirement 25, and no reputation changes, because only player harm changes reputation (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 8).

### Loot Pickup
19. A **lootable pickup** is a shared world pickup (`ANamecLootPickup` in shared world pickup mode, `specs/combat-loot/combat-loot.md` Requirement 34) with the lootable flag set. The server sets the flag on:
    - items dropped by a player (`specs/combat-loot/combat-loot.md` Requirement 34);
    - gold dropped with Drop Gold (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 30);
    - storage container contents dropped when the container is destroyed or deconstructed (`specs/inventory/inventory.md` Edge Case 7);
    - items and gold dropped by a dead or despawned enemy or Veteran (Requirements 32–33), and pickups written by Requirement 34;
    - shared world pickups converted from per-player loot (Requirement 20), including boss materials, trophies, and offering items.

    Every other world item is not lootable: per-player loot actors before conversion, Loose Stick and Loose Stone pickups (`specs/voxel-world/voxel-world.md` Requirement 30), forage nodes, Wood from fallen logs and removed stumps (`specs/voxel-world/voxel-world.md` Requirement 20), voxel yield drops (`specs/voxel-world/voxel-world.md` Requirement 14), and materials dropped by destroyed building pieces (`specs/voxel-world/voxel-world.md` Requirement 26). Quest items never become world pickups (`specs/inventory/inventory.md` Requirement 24). Players pick up lootable and non-lootable shared world pickups alike.
20. A per-player loot actor (`specs/combat-loot/combat-loot.md` Requirement 28) left unclaimed for 2 real-time minutes (tuning value) after it spawns converts into shared world pickups at its location: one lootable pickup per item it holds, plus one lootable gold pickup when it holds gold. The conversion time counts only while the world is not paused (`specs/multiplayer/multiplayer.md` Edge Cases 1 and 2). Converted pickups are visible to every player in every viewport and can be picked up by any player or enemy. Each converted pickup despawns 10 minutes (the `specs/combat-loot/combat-loot.md` Edge Case 2 despawn time, tuning value) after the original per-player loot actor spawned. Its remaining despawn time is saved with it (`specs/game-foundation/game-foundation.md` Requirement 6) and counts down only while the world is loaded and not paused. A converted pickup that is picked up and dropped again by a player is a new player-dropped pickup and does not despawn.
21. Every lootable pickup carries a Smart Object with one slot. An enemy claims the slot when it enters Loot for that pickup and releases the claim when Loot ends. A claim never blocks a player's pickup.
22. Only `Hostile`-category enemies loot: region monsters, Bandits, Beastmen, raiders, and Veterans. `Wildlife`, `Boss` rows, and town NPCs never enter Loot.
23. **Loot.** An enemy in Idle/Patrol, or a raider in Raid with an empty threat table (Requirement 10), enters Loot when its perception (Requirement 11) finds a lootable pickup that is within 12 m (tuning value) and in its line of sight, has a navmesh path, has an unclaimed Smart Object slot, and, for an enemy with a home position, lies within its leash distance of home. It picks the nearest such pickup, claims it (Requirement 21), and walks to within 1.5 m (tuning value). It then plays a 1 s pickup montage (tuning value), and at the montage's pickup notify the server processes the enemy's pickup request (Requirement 24). The pickup is interrupted, the claim is released, and the pickup stays on the ground when the enemy detects a hostile target or takes damage (it switches to Combat, or back to Raid for a raider), staggers, is chosen as an execution victim (`specs/combat-loot/combat-loot.md` Requirement 48), exceeds its leash distance (Return), or the pickup disappears. After a successful pickup, the enemy makes its equip decision (Requirement 29), then looks for the next lootable pickup, and otherwise returns to Idle/Patrol when inside its home radius, to Return when outside it, or to Raid for a raider.
24. Pickup contention: the server processes pickup requests in the order it receives them, whether a player's `ServerPickup` (`specs/inventory/inventory.md` Data Flow 1) or an enemy's pickup notify. The first request processed takes the pickup, and every later request for it fails. A failed enemy request ends its Loot state.
25. Enemies carry an unlimited number of items and unlimited gold, with no weight, encumbrance, or equip load rules. Each picked-up item keeps its full item instance data (`specs/inventory/inventory.md` Data Flow 4: rarity, affixes, durability, item level, dye colors, potency, rune tier, remaining drinks, provenance recipe row ID, and weapon coating). Picked-up gold adds to the enemy's gold counter, clamped to 2,147,483,647 like a character's gold (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 28). Players cannot see an enemy's carried items or gold.

### Equipping
26. `DT_EnemyAI_SkeletonFamilies` has one row per skeleton family with: family ID, Humanoid flag, and, for rows with the Humanoid flag set, a body race ID and sex naming one of the 12 race × sex body combinations (`specs/character-creation/character-creation.md` Requirement 18), an appearance preset (`FNamecAppearance` values for that race and sex, `specs/character-creation/character-creation.md` Requirement 16), and an IK Retargeter asset that retargets the family's animation set to that body. The skeleton family column of `DT_Combat_Enemies` and `DT_Factions_NPCs` names a row of this table (`specs/combat-loot/combat-loot.md` Requirement 19). Only an enemy whose family has the Humanoid flag equips items (Requirements 27–31). Every other enemy only carries what it picks up.
27. A humanoid enemy has seven equipment slots: Right Hand, Left Hand, Head, Chest, Hands, Legs, and Feet. What each slot accepts:
    - Right Hand: a weapon of any weapon category except Bow and Shield (`specs/character-progression/character-progression.md` Requirement 29). A two-handed weapon (Two-Handed Sword, Two-Handed Axe, or Two-Handed Hammer) occupies the Right Hand and the Left Hand.
    - Left Hand: a shield, or a one-handed weapon of any weapon category except Staff.
    - Head, Chest, Hands, Legs, Feet: armor and clothing for that slot (`specs/inventory/inventory.md` Requirements 12 and 13).

    Enemies never equip jewelry, cloaks, bags, or ammo, and also never equip bows, tools, torches, or any item at 0 durability. Those items, and every other picked-up item, are carried.
28. **Item Score** is a single number that ranks an item for enemy equipping: `ItemScore = WeaponBase × WeaponBaseWeight + Armor × ArmorWeight + TotalResistance × ResistanceWeight + Σ (AffixValue × AffixWeight) + LegendaryPowerScore`. WeaponBase and Armor are the item definition's values (0 when the item has none). TotalResistance is the sum of the item definition's resistance values across every damage type. The Σ term sums every affix on the item instance, each value multiplied by the AffixWeight of its affix type. LegendaryPowerScore applies only to an item with a Legendary unique power. The weights live in `DT_EnemyAI_ItemScore`, with starting values WeaponBaseWeight 1.0, ArmorWeight 1.0, ResistanceWeight 100, AffixWeight 1.0 for every affix type, and LegendaryPowerScore 25 (tuning values). Stat requirements and category penalties do not change Item Score.
29. **Equip decision.** After each successful pickup (Requirement 23), and only then, a humanoid enemy evaluates the picked-up item when Requirement 27 lets it be equipped:
    - Armor or clothing: it equips into its slot when that slot is empty or the item's Item Score is greater than the equipped item's.
    - Shield: it equips into the Left Hand when the Left Hand is empty or the shield's Item Score is greater than the Left Hand item's, and never while a two-handed weapon is equipped.
    - Two-handed weapon: it equips when both hand slots are empty or its Item Score is greater than the sum of the Right Hand and Left Hand items' Item Scores.
    - One-handed weapon: it is first compared with the Right Hand (with a two-handed weapon's Item Score when one is equipped) and equips there when the Right Hand is empty or its Item Score is greater. Otherwise, unless it is a Staff or a two-handed weapon is equipped, it is compared with the Left Hand in the same way.

    Every item an equip displaces becomes carried. Equipping is instant after the pickup montage. Enemies never equip or unequip at any other time.
30. Equipped items change the enemy's combat values:
    - Each equipped item's Armor adds to the enemy's Armor attribute on `UNamecEnemyAttributeSet` (0 with nothing equipped), and each equipped item's resistance values and resistance affixes add to the matching resistance attributes, so player damage to the enemy uses the full `specs/combat-loot/combat-loot.md` Requirement 9 TargetResistance.
    - Each equipped armor piece's `Poise` adds to the enemy's MaxPoise (`specs/combat-loot/combat-loot.md` Requirement 38).
    - While a Right Hand weapon is equipped, every attack in the enemy's attack set whose `UsesWeapon` flag is set (a per-attack column in `DT_Combat_Enemies`) adds the weapon's WeaponBase to its damage (`specs/combat-loot/combat-loot.md` Requirement 38, EquippedWeaponBase) and deals the weapon's damage type instead of the attack's own. Such an attack also applies the weapon item definition's own status buildup, when it has one, in addition to the attack's own buildup (`specs/combat-loot/combat-loot.md` Requirements 10 and 38). Left Hand items add no damage and no buildup, and a shield gives an enemy no blocking.
    - A weapon's coating (`specs/combat-loot/combat-loot.md` Requirement 61) has no effect on the enemy: its attacks do not apply the coating and do not lower its remaining hits.
    - Every other affix and every Legendary unique power has no effect on the enemy.
    - Enemies ignore stat requirements and category penalties: no `specs/combat-loot/combat-loot.md` Requirement 36 penalty applies to an enemy's equipped items.
    - Enemy use never reduces durability: attacking, being hit, and dying leave every picked-up item's durability unchanged.
31. Equipped items are visible on every client. A humanoid enemy's body is built through Mutable from its family's body race and sex and appearance preset (Requirement 26) plus its equipped wearables, selecting each wearable's body variant for that race × sex combination and applying each item's visibility settings (`specs/character-creation/character-creation.md` Requirements 18–19), by the same path characters use (`specs/character-creation/character-creation.md` Data Flow 5). Held weapons and shields attach to the hand sockets. Each item's dye colors are set as runtime Substrate material parameters (`specs/crafting-jobs/crafting-jobs.md` Requirement 20), and an item's affixes and dye colors are never changed by enemy pickup, equipping, carrying, or dropping. The family's animation set is retargeted to that body with its IK Retargeter asset.
    - The research file supports this path for the 12 player bodies: Mutable is Production-Ready in 5.8 and generates the 12 race × sex armor variants, and the IK Retargeter shares one animation set across differently proportioned bodies (`specs/engine-tech/engine-tech.md` Requirement 3). So every humanoid family uses one of the 12 player bodies, and a skeleton family whose body is not one of them cannot set the Humanoid flag (`specs/character-creation/character-creation.md` Requirement 20).
    - The research file does not cover the CPU and memory cost of runtime Mutable rebuilds for many humanoid enemies changing equipment in 1–4 viewports. When enemy Mutable rebuilds run, whether generated meshes are cached, and any limit on simultaneous rebuilds is a Researcher decision, recorded in `specs/engine-tech/engine-tech-roadmap.md` (created by the Researcher) and measured by the benchmark milestone (`specs/engine-tech/engine-tech.md` Requirement 11).

### Drops on Death and Despawn
32. When an enemy dies from any cause (a player, an execution, another enemy, a Guard, fall damage, or a falling tree log), the server drops every picked-up item, equipped and carried, as lootable shared world pickups at its death location (one pickup per item instance), plus one lootable gold pickup when its gold counter is above 0. This is in addition to its normal per-player loot (`specs/combat-loot/combat-loot.md` Requirement 28). Dropped item instances keep their affixes, dye colors, durability, and every other instance value.
33. A non-Veteran enemy that despawns without dying drops its picked-up items and gold the same way at the position where it despawns. This covers a failed Return (Requirement 9), a retreating raider's despawn and a raid's immediate despawn (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 58), and chunk stream-out (Requirement 11). A Veteran that despawns without dying keeps its picked-up items and gold (Requirement 47).
34. Every `UNamecWorldSave` write (autosave and host exit, `specs/game-foundation/game-foundation.md` Requirement 8) writes each live non-Veteran enemy's picked-up items and gold as lootable shared world pickups at that enemy's current position, without removing them from the enemy in the running session. Non-Veteran enemies are not saved, so after a load those items are on the ground where the enemies stood.

### Enemy XP and Levels
35. `Hostile`-category enemies gain XP: region monsters, Bandits, Beastmen, raiders, and Veterans. `Boss` rows, `Wildlife` rows, and town NPCs never gain XP.
36. XP sources, with starting values in `DT_EnemyAI_XP`:

    | Source | XP |
    |--------|----|
    | Downing a player: the enemy's hit brings a player to 0 Health and the player becomes Downed (`specs/combat-loot/combat-loot.md` Requirement 13) | 50 |
    | Killing a player: the enemy's hit kills a player outright (`specs/combat-loot/combat-loot.md` Requirement 18) or ends a Downed player's bleed-out timer (`specs/combat-loot/combat-loot.md` Requirement 13) | 100 |
    | Killing a town NPC: the enemy's hit kills a Guard or Citizen | 40 |
    | Killing an enemy hostile to it (Requirement 15): the enemy's hit kills that enemy | the victim's KillXP (Requirement 39) × 0.5 |
    | Surviving: each real-time minute alive while simulated (Requirement 11) and while the world is not paused | 5 |

    Only the enemy whose hit caused the down or kill receives that XP. No enemy receives XP when players die because every player in the session is Downed or dead (`specs/combat-loot/combat-loot.md` Requirement 16). The enemy tracks its XP per source category (Players, Town NPCs, Enemies, Survival) for its Veteran title (Requirement 45).
37. An enemy's gained levels come from its total XP: gained level N requires 100 × N more XP than gained level N − 1 (starting values in `DT_EnemyAI_XP`: 100, 300, 600, … total XP). Gained levels are capped at 10 (tuning value), and an enemy at the cap gains no more XP. When one XP award crosses several levels, each level-up applies in order.
38. An enemy's **current level** = its base level + its gained levels. The base level is its `DT_Combat_Enemies` row level, or for a raider the row level plus the raid's level offset (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 55).
39. Each gained level multiplies the enemy's MaxHealth by HealthPerLevel, 1.10, its attack damage by DamagePerLevel, 1.05, and its row MaxPoise by PoisePerLevel, 1.05 (tuning values in `DT_EnemyAI_XP`), compounding per level, on top of player-count scaling (`specs/combat-loot/combat-loot.md` Requirement 21) and a raider's per-level offset multipliers (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 55). The enemy's current level is used for:
    - its loot item level (`specs/combat-loot/combat-loot.md` Requirement 31);
    - the KillXP each eligible player receives: `KillXP = row KillXP × (1 + 0.1 × gained levels)` (0.1 is a tuning value), plus the Veteran bonus (Requirement 51);
    - the skill XP difficulty multiplier (`specs/character-progression/character-progression.md` Requirement 22).
40. On each level-up, the server raises MaxHealth and raises Health by the amount MaxHealth increased, raises MaxPoise, replicates the new level, and plays a level-up VFX event on every client. Every name plate showing the enemy updates (Requirement 41). The server then checks promotion (Requirement 44).
41. Every `Hostile` and `Wildlife` enemy has a name plate that shows its current level with its row display name, or, for a Veteran, its name, title, and level (Requirement 45). A name plate renders in a local player's viewport only while that player is locked on to the enemy (`specs/combat-loot/combat-loot.md` Requirement 6) or within 15 m of it (tuning value), and each viewport decides this independently. `Boss` rows and town NPCs have no enemy name plate.
42. Every newly spawned enemy starts at its base level (Requirement 38) with 0 XP, 0 gained levels, no picked-up items, and 0 gold. This covers normal spawning (`specs/combat-loot/combat-loot.md` Requirement 20), camp spawning (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 18), and raider spawning (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 55). A Veteran spawning from its record (Requirement 47) is not a new spawn.

### Veterans
43. A **Veteran** is a named enemy saved with the world. Veterans are `Hostile`-category enemies for every rule: they are executable (`specs/combat-loot/combat-loot.md` Requirement 44), can be taunted, and follow the loot eligibility and kill rules (`specs/combat-loot/combat-loot.md` Requirement 29).
44. **Promotion.** When a non-Veteran enemy reaches 3 gained levels (tuning value), and at each later level-up while it is still a non-Veteran, the server promotes it to a Veteran unless it is a raider or a cap blocks promotion (Requirement 48). Raiders gain XP and levels but are never promoted. On promotion the enemy gets a Veteran GUID, a name and title (Requirement 45), and its current home position as its Veteran home (Requirement 4), checked by Requirement 53, and it starts being saved (Requirement 46).
45. **Names and titles.** `DT_EnemyAI_Names` holds name lists keyed by faction ID (for `Bandits` and `Beastmen` rows) or by skeleton family ID (for Faction `None` rows), and title lists keyed by XP source category (Players, Town NPCs, Enemies, Survival), with placeholder starting content such as "the Kinslayer" (Players) and "the Survivor" (Survival). The name is picked at random from its key's list among names no living Veteran in the world uses, or from the whole list when every name is in use. When no list exists for the key, the name is the enemy row's display name. The title is picked at random from the list of the source category that gave the enemy the most XP before promotion (Requirement 36), with ties broken in the order Players, Town NPCs, Enemies, Survival. The name and title never change. A Veteran's name plate shows its name, title, and current level (Requirement 41).
46. **Saving.** `UNamecWorldSave` stores one record per living Veteran (`specs/game-foundation/game-foundation.md` Requirement 6): Veteran GUID, enemy row ID, faction, current level, XP, current health, every equipped and carried item instance with its slot, gold, home position, name, title, kill counts (players downed, players killed, town NPCs killed, enemies killed), and, for a camp Veteran, its camp and spawn point (Requirement 54). Current health is saved without player-count scaling (Health ÷ the `specs/combat-loot/combat-loot.md` Requirement 21 health multiplier in effect when saved) and multiplied by the current health multiplier when the Veteran spawns, clamped to MaxHealth.
47. **Streaming and roaming.** A Veteran record lasts until the Veteran is killed. The Veteran is simulated only while the chunk it stands in is streamed in. When that chunk streams out, the Veteran despawns and keeps its record (health, items, gold, XP). When its home position's chunk streams in and the Veteran is not simulated and not leading a raid, it spawns at its home position, exempt from the building piece distance, player distance, spawn density, and per-chunk cap rules of `specs/combat-loot/combat-loot.md` Requirement 20. Veterans do not count toward spawn density or per-chunk caps. A Veteran roams within its enlarged home radius and leash distance (Requirement 4).
48. **Caps.** At most 5 Veterans have their home position in any one region, and at most 20 Veterans exist in a world (tuning values). While a promotion would exceed either cap, the enemy is not promoted and keeps levelling normally as a non-Veteran up to the gained-level cap (Requirement 37).
49. **Combat.** A Veteran's Health is not restored when it reaches home in Return (Requirement 9), and Veterans have no health regeneration. Its Health rises only through level-ups (Requirement 40).
50. **Death.** When a Veteran is killed, its record is removed from the world's Veterans at once and is absent from every later save, so a killed Veteran never returns. It drops its picked-up items and gold (Requirement 32).
51. **Rewards.** When a Veteran dies, each loot-eligible player (`specs/combat-loot/combat-loot.md` Requirement 29) receives, in addition to its normal KillXP and per-player loot: bonus character XP equal to its KillXP (Requirement 39) × 1.0 (tuning value), and one guaranteed item roll from its loot table's item categories whose rarity rolls from `DT_Loot_RarityWeights` with the Common and Magic weights excluded, so the item is Rare or better (tuning value), at the Veteran's current level, placed in that player's per-player loot actor.
52. **Raid leaders.** When a raid roll succeeds and `specs/factions-kingdoms/factions-kingdoms.md` Requirement 54 picks the `Bandits` or `Beastmen` faction as the raider source, the server looks for that faction's living Veterans whose home position is within 300 m (tuning value) of the base center and who are not already leading a raid. The nearest one joins the raid as its raid leader:
    - It despawns from the world if simulated and spawns at a valid raider spawn candidate with the raiders (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 56), in addition to the rolled raid size.
    - It keeps its current level (no raid level offset), health, equipment, carried items, and gold, and runs Raid (Requirement 10).
    - It counts as a raider for targeting, raid end, and kill rules (`specs/combat-loot/combat-loot.md` Requirement 43, `specs/factions-kingdoms/factions-kingdoms.md` Requirements 58 and 60), and stays a Veteran for rewards, saving, and death (Requirements 46, 50, and 51).
    - When the raid ends, a surviving leader retreats and despawns with the raiders (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 58), keeps its picked-up items and gold, and spawns at its home position again by Requirement 47.

    Region monster and kingdom raider raids have no raid leader.
53. **Home validity.** A Veteran's home position is invalid when it lies inside the base area of any base (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 51) or inside a town's protected radius (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 15). The server checks it at promotion, when the world loads, and each time the Veteran is about to spawn at home. An invalid home moves to the nearest point on walkable terrain outside every base area and every town protected radius, sampled at the raider spawn sample spacing (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 56), and the new home position is saved.
54. **Camp Veterans.** A camp enemy promoted to Veteran stays its spawn point's enemy. That spawn point's enemy is not dead while the Veteran lives, so its camp is not cleared (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 19), and a camp respawn or stream-in spawns no new enemy at that spawn point. When the Veteran dies, that spawn point's enemy is dead, and camp clearing and Clear Camp credit follow `specs/factions-kingdoms/factions-kingdoms.md` Requirements 19 and 44.

### Tuning Data
55. Every value marked "tuning value" or "starting value" in this spec that names no other table lives in `DT_EnemyAI_Rules`.

### Data Validation
56. An editor data validator (Data Validation, `specs/engine-tech/engine-tech.md` Requirement 3) fails the content build when `DT_EnemyAI_Hostility`:
    - lacks a row for any AI group or a column for any group of Requirement 12;
    - has a cell other than `Neutral` where a group's row meets its own column;
    - has a `Hostile` cell between two of Region Monster, Boss, Bandits, Beastmen, Kingdom Raider, and Guard whose mirror cell is not `Hostile`;
    - has a `Flee` cell outside the Wildlife row, a Wildlife row cell other than `Flee` or `Neutral`, or a Wildlife column cell other than `Neutral`;
    - has a Town NPC row cell other than `Neutral`, or a `Reputation` cell anywhere except the Guard row's Player column.

## Data Flow
1. A chunk streams in → combat-loot spawning (`specs/combat-loot/combat-loot.md` Requirement 20), camp spawning (`specs/factions-kingdoms/factions-kingdoms.md` Data Flow 2 and Requirement 18), and `UNamecVeteranSubsystem` (Veterans whose home is in that chunk, Requirement 47, after the Requirement 53 home check) spawn enemies → each enemy's `UNamecEnemyAIComponent` sets its home position, starts its StateTree in Idle/Patrol, and `UNamecEnemyProgressionComponent` sets its base level (Requirement 42) or restores a Veteran's level, XP, and health.
2. On each perception interval (Requirement 11), `UNamecEnemyAIComponent` queries `UNamecHostilityQuery` for each perceived actor's group and the `DT_EnemyAI_Hostility` cell → updates stimuli, detection, and the threat table → the StateTree switches state (Requirements 5–10).
3. An enemy's perception finds a lootable pickup (Requirement 23) → the enemy claims the pickup's Smart Object slot → walks to it → plays the pickup montage → at the pickup notify, `UNamecEnemyLootComponent::TryPickup(PickupId)` runs on the server → when the pickup still exists, the server removes it and adds its item or gold to the enemy (Requirement 25) → `UNamecEnemyEquipmentComponent` computes Item Scores (Requirement 28) and makes the equip decision (Requirement 29) → applies the Requirement 30 attribute changes → replicates equipped item visuals → each client rebuilds the enemy's Mutable body and sets dye parameters (Requirement 31).
4. A per-player loot actor reaches 2 real-time minutes unclaimed → `ANamecLootPickup` converts it into lootable shared world pickups with the despawn time remaining from its spawn (Requirement 20) → the new pickups replicate to every client.
5. An enemy's hit downs or kills a player or town NPC, kills a hostile enemy, or a survival minute passes → `UNamecEnemyProgressionComponent::AwardXP(Source, Amount)` → level-ups apply in order (Requirements 37–40) → promotion check (Requirement 44) → `UNamecVeteranSubsystem` checks caps (Requirement 48), assigns name and title (Requirement 45), validates home (Requirement 53), and registers the record.
6. An enemy dies → `UNamecEnemyLootComponent` drops its picked-up items and gold as lootable pickups (Requirement 32) → `specs/combat-loot/combat-loot.md` Data Flow 4 runs with KillXP at the current level (Requirement 39) → for a Veteran, each eligible player receives the Veteran rewards (Requirement 51) and `UNamecVeteranSubsystem` removes the record (Requirement 50).
7. `UNamecRaidSubsystem` picks Bandits or Beastmen as a raid source (`specs/factions-kingdoms/factions-kingdoms.md` Data Flow 9) → `UNamecVeteranSubsystem::FindRaidLeader(Faction, BaseCenter)` returns the nearest eligible Veteran (Requirement 52) → the raid spawns it with the raiders → when the raid ends, it despawns and later spawns at home.
8. On each world save, `UNamecVeteranSubsystem` writes every Veteran record (Requirement 46) and `UNamecEnemyLootComponent` writes each live non-Veteran's picked-up items and gold as lootable shared world pickups (Requirement 34) into `UNamecWorldSave`.
9. On world load, `UNamecVeteranSubsystem` reads the Veteran records → skips and logs any record whose enemy row is missing (Edge Case 8) → checks each home position (Requirement 53) → Veterans spawn when their home chunks stream in (Data Flow 1).

## Edge Cases
1. When two enemies perceive the same pickup, only the first to claim its Smart Object slot enters Loot for it, and the other chooses another pickup or stays in its state. When a player and an enemy request the same pickup, the first request the server processes wins (Requirement 24).
2. When a player dies, nothing drops: players drop no items and no gold on death (`specs/combat-loot/combat-loot.md` Requirement 17), so this spec creates no pickups from player deaths.
3. When a base is built, or a town's protected radius covers, a Veteran's home position, the home moves to the nearest valid point outside every base area and town protected radius the next time Requirement 53 checks it, and the Veteran spawns there.
4. When the host exits or autosaves while non-Veteran enemies hold picked-up items, those items and gold are saved as lootable shared world pickups at each enemy's position (Requirement 34), and after the next load they lie there.
5. When a LAN client disconnects, its players leave every threat table and every Investigate stimulus, and AI state, XP, and picked-up items are otherwise unaffected. The disconnected players' per-player loot actors still convert after 2 real-time minutes (Requirement 20).
6. When the world saves while an enemy is in Loot, the pickup is saved as a shared world pickup, claims are not saved, and no enemy is in Loot after a load. A Veteran saved mid-Loot is saved without that item.
7. When a Veteran record names an item definition that no longer exists, that item is skipped with a log entry and the rest of the Veteran loads.
8. When a Veteran record names an enemy row missing from `DT_Combat_Enemies`, the record is skipped on load with a log entry, its items and gold are not restored, and the next world save omits it.
9. When an enemy in Loot is chosen as an execution victim, Loot ends and its claim is released before the paired animation starts, and it cannot loot during the execution. Its picked-up items drop when it dies at the kill notify (Requirement 32).
10. When a non-humanoid enemy picks up a weapon or armor piece, the item is carried and has no effect on the enemy.
11. When a humanoid enemy holding a one-handed sword and a shield picks up a two-handed axe, the axe equips only when its Item Score is greater than the sword's and the shield's combined, and then the sword and the shield are carried.
12. When a humanoid enemy picks up a weapon at 0 durability, it carries the weapon and never equips it.
13. When a converted pickup reaches its despawn time while an enemy is walking to it, the pickup despawns and the enemy's Loot ends.
14. When a pickup lies inside a sealed boss arena, the boss never loots it, and every player inside can pick it up once it is a shared world pickup.
15. When an enemy is killed by fall damage or a falling tree log, no enemy receives XP, its picked-up items drop, and players within 50 m or who damaged it are loot-eligible (Requirement 18).
16. When a Veteran dies while the world is at a Veteran cap, the next enemy that levels up at or above 3 gained levels in a region below the cap is promoted (Requirement 44).
17. When two raids start at bases within 300 m of the same Veteran, the raid that spawns first takes it as leader, and the other raid has no leader.
18. When the host exits during a raid led by a Veteran, the raid is not saved (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 59), and the Veteran is saved with its home position, current health, and items, and spawns at home after the next load.
19. When a raid ends because no player is within 100 m of its base center, a Veteran leader despawns at once with the raiders and keeps its items, and non-Veteran raiders drop their picked-up items where they despawn (Requirement 33).
20. When a raider reaches 3 gained levels, it is not promoted, and it despawns with its raid like any raider.
21. When the session's player count changes while a Veteran is simulated, it keeps its current scaling (`specs/combat-loot/combat-loot.md` Edge Case 5). When it next spawns from its record, its saved unscaled health is multiplied by the current health multiplier (Requirement 46).
22. When every name in a Veteran name list is used by a living Veteran, the new Veteran's name repeats one at random (Requirement 45).
23. When a Guard kills a Hostile-tier player or an enemy, the Guard gains no XP (Requirement 35).
24. When an enemy's gold counter would exceed 2,147,483,647, it is clamped, and the extra gold of that pickup is lost.
25. When a camp enemy despawns because its chunk streams out while it holds picked-up items, the items drop at its position (Requirement 33), and the enemy spawns again with no picked-up items when the camp streams in (Requirement 42).
26. When a local player on a split-screen machine is within 15 m of an enemy and another local player is not, the name plate shows only in the first player's viewport.

## Acceptance Criteria
- [ ] Every enemy and town NPC runs its row's StateTree only on the host, and a LAN client's copy shows the replicated AI state without running perception.
- [ ] A region monster that chases a player beyond 40 m from its spawn position returns home and restores full Health, and a Veteran doing the same keeps its current Health.
- [ ] An enemy row with FleeHealthThreshold 0.2 flees below 20% Health and returns home after 8 s with no hostile target detected, and a row with no threshold never flees.
- [ ] A crouching player just outside a region monster's perception radius but in line of sight at 1.4× the radius makes it investigate, and a Stealth-trained player earns Stealth XP while it investigates.
- [ ] A Bandit and a Beastman that detect each other fight, a region monster attacks a Bandit, a Citizen, and a Guard inside a town, a Guard attacks a region monster inside the town but not outside the protected radius, a kingdom raider attacks a Bandit and ignores a Citizen, and no enemy ever attacks a `Wildlife` creature.
- [ ] A `Wildlife` creature flees from a player, a region monster, and a Guard it detects.
- [ ] A Bandit's area-of-effect attack that hits two Bandits and a Beastman damages only the Beastman.
- [ ] A region monster's hit on a Bandit in a 3-player session deals the same player-count damage multiplier as its hit on a player.
- [ ] Per-player loot left unclaimed for 2 real-time minutes becomes visible to every player and can be picked up by another player or an enemy, and it despawns 10 minutes after the original drop.
- [ ] A region monster in Idle/Patrol picks up an item a player dropped 8 m away, and hitting it during the pickup montage interrupts the pickup and leaves the item on the ground.
- [ ] A player who presses pickup before an enemy's pickup notify gets the item, and the enemy gets nothing.
- [ ] A humanoid Bandit picks up a sword whose Item Score beats its current sword, equips it visibly on every client with its dye colors, deals the new sword's WeaponBase and damage type with its `UsesWeapon` attacks, and carries its old sword.
- [ ] A humanoid enemy wielding a picked-up sword with its own Bleed buildup and a Poison coating applies Bleed buildup with its `UsesWeapon` attacks, applies no Poison buildup, and leaves the coating's remaining hits unchanged.
- [ ] A raider whose target is unreachable behind building pieces outside its snapshot attacks the first blocking piece and keeps raiding after destroying it.
- [ ] A humanoid enemy equipping a Metal chest piece gains that piece's Armor and Poise, and a player's Slash hit on it is reduced by `Armor ÷ (Armor + ArmorConstant)`.
- [ ] A humanoid enemy never equips a ring, cloak, bag, arrow, bow, torch, or tool it picks up, and a non-humanoid enemy never equips anything.
- [ ] A killed enemy drops every picked-up item with unchanged affixes, dye colors, and durability, plus its picked-up gold, as shared pickups, and every eligible player still gets normal per-player loot.
- [ ] A non-Veteran enemy holding a picked-up item drops it where it despawns when its chunk streams out, and an item held at host exit is on the ground at that position after reload.
- [ ] An enemy that downs a player gains 50 XP, one that gains its first level has MaxHealth × 1.10 and Health raised by the same amount, and its name plate shows the new level within 15 m or while locked on.
- [ ] Loot from an enemy with 2 gained levels has item level = row level + 2, and its KillXP is row KillXP × 1.2.
- [ ] An enemy reaching 3 gained levels becomes a Veteran with a name and a title from its top XP source, and its name plate shows name, title, and level.
- [ ] A Veteran with items survives save → quit → load with the same name, title, level, health, equipment, carried items, gold, and home, and spawns at home when its chunk streams in.
- [ ] With 5 Veterans homed in a region, a sixth enemy in that region reaching 3 gained levels is not promoted and keeps levelling.
- [ ] A killed Veteran never appears again after save → quit → load, and each eligible player receives bonus KillXP and one extra item of Rare rarity or better.
- [ ] A successful Bandit raid roll on a base 250 m from a living Bandit Veteran spawns that Veteran with the raiders, and after the raid it is back at its home.
- [ ] A Veteran whose home ends up inside a base area is homed at the nearest valid point outside it after the next world load.
- [ ] A camp whose enemy became a Veteran is not cleared until that Veteran dies.
- [ ] A content build with a non-symmetric `Hostile` cell between Bandits and Beastmen, or a humanoid skeleton family with no body variant, fails validation.
- [ ] With 30 AI enemies in the benchmark scene (`specs/engine-tech/engine-tech.md` Requirement 11), AI farther than 50 m from every player run perception every 1.0 s, and the benchmark frame rate targets pass.

## Key Files
- `Source/NAMEC/EnemyAI/NamecEnemyAIComponent.h` — new; `UNamecEnemyAIComponent`: StateTree host, home position, home radius and leash distance, the seven states and their transitions (Requirements 3–10), including a raider attacking the first building piece blocking its path to an unreachable target (Requirement 10), replicated AI state, and AI LOD perception intervals (Requirement 11).
- `Source/NAMEC/EnemyAI/NamecHostilityQuery.h` — new; hostility group lookup (Requirement 12), `DT_EnemyAI_Hostility` cell lookup, and the hostile-target and damage filter (Requirements 13–17).
- `Source/NAMEC/EnemyAI/NamecEnemyLootComponent.h` — new; lootable pickup perception, Smart Object claims, pickup requests, carried items and gold, and drops on death, despawn, and world save (Requirements 21–25 and 32–34).
- `Source/NAMEC/EnemyAI/NamecEnemyEquipmentComponent.h` — new; humanoid equipment slots, Item Score, equip decision, attribute changes, the equipped weapon's own status buildup on `UsesWeapon` attacks (Requirement 30), and the replicated equipped item visuals built through Mutable (Requirements 26–31).
- `Source/NAMEC/EnemyAI/NamecEnemyProgressionComponent.h` — new; enemy XP sources and per-source totals, gained levels, per-level multipliers, current level, level-up event, and name plate data (Requirements 35–42).
- `Source/NAMEC/EnemyAI/NamecVeteranSubsystem.h` — new; `UNamecVeteranSubsystem`: promotion, caps, names and titles, Veteran records and save and load, streaming and home spawning, home validity, camp Veterans, and raid leader selection (Requirements 43–54).
- `Source/NAMEC/EnemyAI/StateTree/` — new; StateTree tasks, conditions, and evaluators for the Requirement 3 states.
- `Source/NAMEC/UI/HUD/NamecEnemyNamePlateWidget.h` — new; per-viewport enemy name plate with level, Veteran name and title (Requirement 41).
- `Source/NAMECEditor/NamecHostilityValidator.h` — new; data validator for `DT_EnemyAI_Hostility` (Requirement 56).
- `Source/NAMEC/Combat/AI/NamecEnemyBase.h` — changed; hosts `UNamecEnemyAIComponent`, `UNamecEnemyLootComponent`, `UNamecEnemyEquipmentComponent`, and `UNamecEnemyProgressionComponent`.
- `Source/NAMEC/Combat/AI/NamecEnemyAttributeSet.h` — changed; adds the Armor attribute raised by equipped items (Requirement 30).
- `Source/NAMEC/Loot/NamecLootPickup.h` — changed; lootable flag, Smart Object slot, per-player loot conversion after 2 real-time minutes, and the saved remaining despawn time (Requirements 19–21).
- `Source/NAMEC/Save/NamecWorldSave.h` — changed; Veteran records and converted pickups' remaining despawn times (Requirements 20 and 46).
- `Content/AI/SmartObjects/` — new; the Smart Object definition for lootable pickups (Requirement 21).
- `Content/AI/Enemies/` — changed; enemy StateTree assets implement the Requirement 3 states.
- `Content/Data/DT_EnemyAI_Hostility.uasset` — new; the hostility matrix (Requirement 13).
- `Content/Data/DT_EnemyAI_SkeletonFamilies.uasset` — new; skeleton family rows with the Humanoid flag, body race and sex, appearance preset, and IK Retargeter asset (Requirement 26).
- `Content/Data/DT_EnemyAI_ItemScore.uasset` — new; Item Score weights per term and per affix type, and LegendaryPowerScore (Requirement 28).
- `Content/Data/DT_EnemyAI_XP.uasset` — new; XP per source, the hostile kill factor, survival XP per minute, XP per gained level, and HealthPerLevel, DamagePerLevel, and PoisePerLevel (Requirements 36–39).
- `Content/Data/DT_EnemyAI_Names.uasset` — new; Veteran name lists per faction and skeleton family, and title lists per XP source category (Requirement 45).
- `Content/Data/DT_EnemyAI_Rules.uasset` — new; home radius, leash distance, VeteranRoamMultiplier, patrol waits, investigate multiplier and timeout, detection threat, target loss time, flee calm time, return timeout, AI LOD distance and perception intervals, loot conversion time, loot radius, pickup reach and montage time, gained-level cap, KillXP per gained level, name plate distance, Veteran promotion threshold, Veteran caps, Veteran bonus KillXP multiplier and minimum rarity, raid leader range, and other enemy AI tuning values (Requirement 55).
