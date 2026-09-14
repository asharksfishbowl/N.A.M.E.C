# Spec: Survival

## Overview
Characters manage four survival pressures — hunger, thirst, temperature, and stamina/fatigue — that push players to gather, cook, build shelter, and dress for the climate. Survival ties the voxel world, crafting, and combat together. Shared terms are defined in `specs/game-foundation/game-foundation.md`.

## Goals
- Each climate region feels different to survive in.
- Survival creates reasons to cook, craft clothing, build shelters, and sleep, without dominating the moment-to-moment game.
- Souls-like combat stamina and long-term fatigue work together predictably.

## Non-Goals
- Disease or illness systems beyond the timed Poison status effect (defined in `specs/combat-loot/combat-loot.md` Requirement 10) applied by Swamp and Jungle Poison Water and Poison Plant hazards (defined in `specs/voxel-world/voxel-world.md` Requirement 31).
- Sanity or mental-state meters.
- Food spoilage.

## Requirements

### Meters
1. Each character has four survival meters, stored as GAS attributes in `UNamecSurvivalAttributeSet`: Hunger (0–100), Thirst (0–100), BodyTemperature (°C, comfortable band 35.5–37.5), Fatigue (0–100).
2. Stamina is a combat resource (0–MaxStamina). Stamina, MaxStamina, and StaminaRegen are GAS attributes also in `UNamecSurvivalAttributeSet`, as is Breath (Requirement 23). Base MaxStamina is driven by CON, with the coefficient in `DT_Progression_DerivedStats` (see character-progression spec).
3. Meters update on the server on a fixed survival tick of 1 second (tuning value), driven by a GAS periodic gameplay effect, not the actor Tick.

### Hunger & Thirst
4. Hunger and Thirst start at 100 and drain over time. Drain rates per minute come from `DT_Survival_DrainRates` and are multiplied by activity (sprinting, combat, mining raise drain) and reduced by CON modifier.
5. Eating restores Hunger. Drinking restores Thirst. Values per item come from the item definition.
6. Water sources: drinking from Fresh water volumes (lakes, rivers) restores Thirst directly. Waterskins (Leatherworker) store 5 drinks and refill at Fresh water. Drinking from Poison water volumes (`specs/voxel-world/voxel-world.md` Requirement 31) restores Thirst like Fresh water and applies Poison buildup at the drinking value in `DT_Combat_StatusEffects`. Drinking from Salt water volumes (the ocean) gives no Thirst and adds a "Salty" effect that increases Thirst drain for 60 seconds.
7. At Hunger 0, the character loses 1% max health per 5 seconds and health regen stops. At Thirst 0, the character loses 1% max health per 3 seconds and max stamina is halved (tuning values).
8. Cooked meals (Cook Job) grant a timed buff to one derived stat. Only one meal buff is active at a time, and a new meal replaces the old.

### Temperature
9. Ambient temperature at the character's location = region base temperature (from `DT_World_Climates`) + time-of-day offset + weather offset (includes wind) + altitude offset + local heat sources.
10. Heat sources (campfire, forge, torch-in-hand, lava) add warmth within a radius defined per source. Shelter (a roof voxel/building piece within 4 m overhead plus walls in at least 3 of 6 horizontal directions within 5 m) removes the weather offset.
11. Clothing and armor have Insulation (cold protection) and Cooling (heat protection) values. Each climate row in `DT_World_Climates` has a comfort range of ambient temperatures (starting value 10–26 °C). A character's comfort range is the climate comfort range with the lower bound lowered by total equipped Insulation, the upper bound raised by total equipped Cooling, and both bounds widened by WIS modifier. The Insulation, Cooling, and WIS coefficients live in `DT_Survival_Temperature`.
12. Effective ambient temperature is the Requirement 9 ambient temperature, including the Wet modifier (Requirement 14). When effective ambient temperature is inside the character's comfort range, BodyTemperature drifts toward 37 °C. When effective ambient temperature is below the range, BodyTemperature drifts downward away from 37 °C, and when it is above the range, BodyTemperature drifts upward away from 37 °C. The outside-range drift rate is proportional to the distance outside the range. Both drift rates live in `DT_Survival_Temperature`.
13. Temperature states and effects:
    - Cold (below 35.5 °C): stamina regen −25%.
    - Freezing (below 34 °C): lose 1% max health per 4 seconds, movement speed −15%.
    - Hot (above 37.5 °C): Thirst drain ×2.
    - Overheating (above 39 °C): lose 1% max health per 4 seconds, stamina regen −50%.
    
    The states are exclusive: a character is in at most one state at a time. Freezing also applies Cold's effect, and Overheating also applies Hot's effect.
14. The Wet status applies while swimming or standing in rain without shelter, and lasts until 60 seconds after the character is last in water or unsheltered rain (tuning value). While Wet, effective ambient temperature is 8 °C lower.

### Stamina & Fatigue
15. Stamina is spent by dodge rolls, attacks, blocking hits, sprinting, climbing, and swimming (costs in `DT_Survival_StaminaCosts`). Stamina regenerates after a 1-second delay without spending (tuning value).
16. Fatigue rises over time awake (starting value: 0 → 100 over 40 real-time minutes) and rises faster with stamina use: each stamina point spent adds Fatigue at the rate in `DT_Survival_DrainRates` (tuning value).
17. Effective MaxStamina = base MaxStamina × (1 − Fatigue / 200) × ThirstMultiplier × WeakenedMultiplier. ThirstMultiplier is the Requirement 7 max stamina multiplier (starting value 0.5) at Thirst 0, otherwise 1. WeakenedMultiplier is 1 minus the Weakened max stamina penalty from `specs/combat-loot/combat-loot.md` Requirement 17 (penalty starting value 0.2, so the multiplier is 0.8) while Weakened, otherwise 1. The multipliers stack multiplicatively. At Fatigue 100 with no other multiplier active, max stamina is 50%.
18. Interacting with a placed bed sets that bed as the character's respawn point and puts the character in bed. The respawn point is stored in `UNamecWorldSave`, keyed by character GUID, so each world keeps its own bed per character. When that bed is destroyed or deconstructed, the respawn point is cleared, and the character respawns at the world spawn point.
19. When every player in the session is in a bed at the same time, time skips to the next morning (06:00 in-game) and every player's Fatigue resets to 0. When at least one player is not in a bed, time does not skip, and each player in a bed loses 5 Fatigue per second (tuning value).

### HUD
20. The HUD shows Health, Stamina, and Mana bars at all times. The Breath meter (Requirement 23) shows only while the character's head is underwater. Hunger, Thirst, Temperature, and Fatigue show as icons that appear only when their meter crosses a warning threshold (Hunger/Thirst below 30, Temperature outside the comfortable band, Fatigue above 60).

### Tuning Data
21. Every value marked "tuning value" in this spec that names no other table lives in `DT_Survival_Penalties`.

### Swimming & Breath
22. While swimming at 0 stamina, the character loses 2% max health per second (tuning value) until stamina regenerates above 0 or the character leaves the water.
23. Diving underwater drains Breath, a GAS attribute in `UNamecSurvivalAttributeSet` that holds 30 seconds of air (tuning value). Breath drains while the character's head is underwater. After the head surfaces, Breath refills at a rate that takes it from empty to full in 2 seconds (tuning value). Breath is not saved: it is full on every world entry.
24. At Breath 0, the character loses 5% max health per second (tuning value) until the head surfaces.

## Data Flow
1. The survival periodic effect ticks on the server every second for each character.
2. `UNamecSurvivalComponent` reads the character's activity state, equipped insulation/cooling, region climate from `UNamecClimateSubsystem`, nearby heat sources, and shelter state.
3. The component computes new Hunger, Thirst, Fatigue, and BodyTemperature values and applies them via gameplay effects.
4. When a meter crosses a threshold, the component applies or removes the matching status gameplay effect (e.g., `GE_Freezing`).
5. Attributes replicate to the owning client. The HUD binds to attribute-change delegates to show and hide icons.
6. Survival meter values save to `UNamecCharacterSave` on each save.

## Edge Cases
1. When a character is in the downed state (see combat-loot spec), survival meters pause.
2. When a character joins a world, their meters load from their character save. A character saved at Hunger 5 joins at Hunger 5.
3. When multiple heat sources overlap, their warmth values add, and the total is capped at the highest max contribution among the overlapping sources in `DT_Survival_Temperature`.
4. When a shelter's roof or walls are dug out while a character is inside, shelter state recomputes on the next survival tick.
5. When a player is at Hunger 0 and Thirst 0 at the same time, both health-loss effects apply.
6. When a player sleeps but another player is in combat (dealt damage to or took damage from a hostile enemy within the last 10 seconds, tuning value), time skip is blocked and all sleeping players see "Cannot sleep — a player is in combat".
7. When the shelter check runs, it uses at most one upward check and six horizontal checks per character per tick. Each check queries terrain via `UNamecVoxelWorld` density lookups and building pieces via physics traces. A character has walls when at least 3 of the 6 horizontal checks hit.

## Acceptance Criteria
- [ ] A character standing idle in the temperate region loses Hunger at the rate in `DT_Survival_DrainRates`.
- [ ] At Thirst 0 the character loses health, and max stamina is halved.
- [ ] Standing in a tundra region in no clothing drops BodyTemperature below 34 °C and applies Freezing.
- [ ] Standing within a campfire's radius in the tundra keeps BodyTemperature in the comfortable band.
- [ ] With effective ambient temperature inside the character's comfort range, a BodyTemperature of 35 °C drifts back toward 37 °C.
- [ ] Equipping higher-Insulation clothing lowers the character's comfort range lower bound by the coefficient in `DT_Survival_Temperature`.
- [ ] At Fatigue 100, max stamina is 50% of base.
- [ ] With 2 players both in beds, time skips to 06:00. With only 1 in bed, time does not skip.
- [ ] Hunger/Thirst/Temperature/Fatigue icons appear only past their warning thresholds.
- [ ] Survival meters are unchanged across save → quit → load.
- [ ] Swimming at 0 stamina loses 2% max health per second, and staying underwater for 30 seconds empties Breath and then loses 5% max health per second.
- [ ] The Breath meter appears on the HUD only while the character's head is underwater.

## Key Files
- `Source/NAMEC/Survival/NamecSurvivalAttributeSet.h` — new; Hunger, Thirst, BodyTemperature, Fatigue, Stamina, MaxStamina, StaminaRegen, Breath attributes.
- `Source/NAMEC/Survival/NamecSurvivalComponent.h` — new; per-tick survival computation and status effects.
- `Source/NAMEC/Survival/NamecShelterQuery.h` — new; shelter detection against voxel data.
- `Source/NAMEC/Survival/NamecHeatSourceComponent.h` — new; attachable heat source for fires, forges, torches.
- `Source/NAMEC/Survival/NamecSleepSubsystem.h` — new; bed tracking and time skip.
- `Content/Survival/Effects/` — new; `GE_Freezing`, `GE_Cold`, `GE_Hot`, `GE_Overheating`, `GE_Wet`, `GE_Salty`, starvation and dehydration effects.
- `Content/Data/DT_Survival_DrainRates.uasset` — new.
- `Content/Data/DT_Survival_Temperature.uasset` — new.
- `Content/Data/DT_Survival_StaminaCosts.uasset` — new.
- `Content/Data/DT_Survival_Penalties.uasset` — new; starvation, dehydration, temperature, Wet, sleep, swimming exhaustion, and Breath tuning values.
