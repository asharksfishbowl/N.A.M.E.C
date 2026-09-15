# Spec: Character Creation, Races & Customization

## Overview
Players create a character by choosing a name, one of six races, a sex, an appearance, and a starting class. Each race has stat bonuses, a passive trait, an active ability, and a downside, so the choice matters for gameplay. Sex is purely cosmetic. Appearance uses preset-based customization that works with a gamepad in split-screen, and appearance (not race or sex) can be changed later at a crafted Mirror. Shared terms are defined in `specs/game-foundation/game-foundation.md`.

## Goals
- Six races that look and play distinctly, with strengths balanced by downsides.
- Character creation is fully usable with a gamepad inside one split-screen viewport.
- Every wearable item renders correctly on every race and sex.
- Appearance can be changed later without affecting progression.

## Non-Goals
- Backgrounds or origin stories.
- Gameplay differences between male and female.
- Freeform facial sliders (Skyrim/BG3-style morph sliders).
- Changing race, sex, or name after creation.
- Hybrid or mixed-race characters.
- Racial ability leveling. Racial abilities do not gain XP or levels.

## Requirements

### Creation Flow
1. Character creation opens from the main menu Characters screen (`specs/game-foundation/game-foundation.md` Requirement 11) and from the "Create New" option in every character select: each local player's lobby character select (`specs/game-foundation/game-foundation.md` Data Flow 2), the mid-session join prompt's character select (`specs/multiplayer/multiplayer.md` Requirement 4), and the LAN pre-join character pick (`specs/multiplayer/multiplayer.md` Requirement 13).
2. Creation steps, in order, each on its own screen with Back and Next: Race → Sex → Appearance → Starting Class (`specs/character-progression/character-progression.md` Requirement 13) → Name & Confirm.
3. A 3D preview of the character is visible on every step. The player can rotate it, zoom between full body and face, and toggle between three preview outfits: starting clothes, Leather armor, and Metal armor (`specs/character-progression/character-progression.md` Requirement 27).
4. Changing race on step 1 after visiting later steps resets appearance to that race's default preset. The sex and starting class choices are kept.
5. The name is 1–24 characters from Unicode letters (including accented Latin letters), digits, spaces, apostrophes, and hyphens, with no leading or trailing space. Emoji and symbols are not allowed. Names do not need to be unique. With a gamepad, the name is entered with an on-screen keyboard rendered inside the owning local player's viewport, provided through `INamecPlatform` (`specs/game-foundation/game-foundation.md` Requirement 10) so a console build can swap in the platform keyboard. The UI font covers at least Latin Extended-A.
6. The Confirm screen shows a summary: race and its traits, sex, starting class and its abilities, and final starting stats. Confirming creates a new `UNamecCharacterSave` with a new character GUID.
7. The "Randomize" action on the Appearance step rolls a random valid appearance for the chosen race and sex.

### Races
8. Six races exist, each a row in `DT_Character_Races` with: race ID, display name, description, stat bonuses, passive trait gameplay effect, active ability class, downside gameplay effect, male and female skeletal mesh sets, male and female first-person arm meshes, and the appearance options available to that race.
9. Racial stat bonuses apply at creation and are permanent. They add to the starting class bonuses (`specs/character-progression/character-progression.md` Requirement 2). Starting stats = 10 + racial bonus + starting class bonus.
10. Racial passives and downsides are infinite-duration GAS gameplay effects applied when the character spawns and never removed.
11. Each race's active ability is a GAS `UGameplayAbility` with a cooldown, following the racial ability rules in Requirement 27. On creation it is placed in the last ability bar slot (slot 6 at the starting bar size; `specs/character-progression/character-progression.md` Requirement 18). The player can move it to any bar slot, but cannot remove it from the bar. Moving it onto an occupied slot swaps the two abilities. It always occupies one of the bar's slots.
12. The six races (all values are starting values in `DT_Character_Races`):

| Race | Stat bonus | Passive | Active ability | Downside |
|------|-----------|---------|----------------|----------|
| **Human** | +1 to all six stats | **Versatile:** +10% skill XP from all sources | **Second Wind:** restore 30% max stamina and 10% max health instantly (120 s cooldown) | **Fleeting Vigor:** Fatigue from time awake and Fatigue from stamina spent both rise 15% faster (×1.15) |
| **Felari** (cat folk) | +2 DEX, +1 CHA | **Night Eyes:** night vision at night and underground (Requirement 13), and no fall damage (`specs/combat-loot/combat-loot.md` Requirement 40) from falls under 8 m | **Pounce:** leap up to 8 m toward the locked-on target (or straight ahead), dealing Unarmed-profile damage and 60 poise damage on landing (20 s cooldown) | **Water-Averse:** swim speed −30%, swimming stamina cost +50%, and the Wet status lasts twice as long |
| **Hundari** (dog folk) | +2 CON, +1 WIS | **Keen Nose:** ore voxels, Wildlife, and Hostile enemies within 30 m show an outline visible only to this player | **Rally Howl:** every player within 20 m gains +25% stamina regen for 20 s (60 s cooldown) | **Loud:** enemy perception radius against this character is +25% |
| **Sauren** (lizard folk) | +2 CON, +1 INT | **Amphibious:** 3× max Breath (90 seconds of air at starting values), +30% swim speed, +0.25 `PoisonResistance` (additive), and 25% less Poison buildup | **Shed Skin:** remove Poison, Bleed, Burn, and Frostbite buildup and active effects (Requirement 13) and restore 2% max health per second for 5 s (90 s cooldown) | **Cold-Blooded:** comfort range lower bound is 6 °C higher |
| **Vanari** (monkey folk) | +2 DEX, +1 INT | **Climber:** climbing stamina cost −50%, climb speed +50% | **Swing Leap:** leap up to 5 m upward or 10 m forward (30 s cooldown) | **Light Frame:** MaxEquipLoad −15% |
| **Ursan** (bear folk) | +2 STR, +1 CON | **Thick Hide:** +10 built-in Insulation and +50 MaxCarryWeight | **Mauling Roar:** taunt every Hostile and Boss enemy within 6 m and deal 100 poise damage to each (45 s cooldown) | **Heat-Prone:** comfort range upper bound is 6 °C lower |

13. Racial effects plug into existing systems as follows:
    - Versatile raises skill XP in `UNamecProgressionComponent::AwardSkillXP` (`specs/character-progression/character-progression.md` Data Flow 3) before the character XP share is computed. It adds with any "+skill XP gain" affix for that skill (`specs/combat-loot/combat-loot.md` Requirement 32): skill XP multiplier = 1 + 0.10 + affix bonus.
    - Fleeting Vigor multiplies both Fatigue sources in `specs/survival/survival.md` Requirement 16 (time awake and stamina spent) by 1.15.
    - Night Eyes is a post-process effect applied only in the owning local player's viewport. It is active automatically at night (in-game 20:00–06:00, tuning value) and whenever the character is underground: inside a cave, or with terrain overhead per the shelter upward check (`specs/survival/survival.md` Requirement 10 and Edge Case 7). A per-local-player Settings toggle can disable it.
    - Shed Skin removes every buildup and active effect carrying the gameplay tag `Status.Negative.Cleansable`: Poison, Bleed, Burn, and Frostbite (`specs/combat-loot/combat-loot.md` Requirement 10). It does not remove Weakened, Wet, Salty, the Hunger, Thirst, temperature, or Fatigue states, Over-Encumbered, or racial downsides.
    - Keen Nose outlines render only in the owning local player's viewport, and outlined actors are found by server-independent local queries (no replication).
    - Amphibious multiplies max Breath (`specs/survival/survival.md` Requirement 23) by 3, and multiplies base swim speed (`specs/survival/survival.md` Requirement 25) by 1.3. Its `PoisonResistance` bonus adds to the attribute in `specs/character-progression/character-progression.md` Requirement 6, used by `specs/combat-loot/combat-loot.md` Requirement 9. Its buildup reduction multiplies Poison buildup received by 0.75, in addition to the `StatusResistance` multiplier (`specs/combat-loot/combat-loot.md` Requirement 10).
    - Water-Averse multiplies base swim speed (`specs/survival/survival.md` Requirement 25) by 0.7, multiplies the swimming stamina cost (`specs/survival/survival.md` Requirement 15) by 1.5, and doubles the Wet linger duration (`specs/survival/survival.md` Requirement 14).
    - Night Eyes' fall exemption applies to fall damage in `specs/combat-loot/combat-loot.md` Requirement 40.
    - Cold-Blooded and Heat-Prone shift the comfort range from `DT_World_Climates` before Insulation, Cooling, and WIS widening are applied (`specs/survival/survival.md` Requirements 11–12).
    - Thick Hide Insulation adds to equipped Insulation (`specs/inventory/inventory.md` Requirement 13).
    - Thick Hide and Light Frame modify `MaxCarryWeight` and `MaxEquipLoad` (`specs/inventory/inventory.md` Requirements 3 and 6).
    - Climber multiplies the climbing stamina cost (`specs/survival/survival.md` Requirement 15) by 0.5 and base climb speed (`specs/survival/survival.md` Requirement 25) by 1.5. Climbing itself is defined in `specs/character-progression/character-progression.md` Requirement 21.
    - Loud multiplies the final enemy perception radius used for detection and aggro against this character by 1.25 (`specs/combat-loot/combat-loot.md` Requirement 41), which also applies to Stealth (`specs/character-progression/character-progression.md` Requirement 21).
    - Mauling Roar applies a taunt (`specs/combat-loot/combat-loot.md` Requirement 22) to each affected enemy: a forced-target effect that makes the enemy target this character for 6 s (tuning value) for non-Boss enemies, after which the enemy's normal threat table resumes. Its poise damage is a racial ability poise source (`specs/combat-loot/combat-loot.md` Requirement 38) and staggers only through Poise reaching 0 (`specs/combat-loot/combat-loot.md` Requirements 7 and 10).
    - Pounce and Swing Leap are root-motion leap abilities that respect collision and cannot pass through terrain or building pieces.

### Sex
14. The player selects Male or Female. Sex selects the body mesh set, voice set, and animation set only. Animation sets are shared across races and sexes with the IK Retargeter per `specs/engine-tech/engine-tech.md` Requirement 3. Stats, abilities, carry weight, and every other gameplay value are identical between sexes.

### Appearance
15. Appearance is preset-based, built using Mutable per `specs/engine-tech/engine-tech.md` Requirement 3. Each race row lists which options it supports, and each option is a list of presets or a bounded slider:
    - **All races:** body type (3 presets), height slider (±8%), build slider (slim ↔ heavy), face preset (8 per race and sex), eye color (12 presets), voice (3 per race and sex).
    - **Human:** skin tone (16 presets), hair style (12 presets), hair color (16 presets), facial hair (8 presets, male and female both allowed), scars and markings (8 presets).
    - **Felari, Hundari, Ursan:** fur color (12 presets), fur pattern (8 presets), ear variant (4 presets), tail variant (4 presets), mane or head fur style (8 presets).
    - **Sauren:** scale color (12 presets), scale pattern (8 presets), frill or crest variant (6 presets), tail variant (4 presets).
    - **Vanari:** fur color (12 presets), face skin tone (8 presets), fur pattern (8 presets), tail variant (4 presets), head fur style (8 presets).
16. Appearance is stored as `FNamecAppearance`: race ID, sex, and one integer or float per option. Preset counts and slider ranges live in `DT_Character_AppearanceOptions`.
17. Height and build sliders are cosmetic only. Collision capsule, reach, and movement speed are identical across all slider values within a race. Collision capsule size per race is defined in `DT_Character_Races` and is the same for both sexes.

### Equipment Fit
18. Every wearable item (armor, clothing, bags, cloaks) must provide a visual variant for each of the 12 race × sex body combinations, using Mutable per `specs/engine-tech/engine-tech.md` Requirement 3. The Researcher records how each item's variants are authored as Mutable inputs in the roadmap. Characters and wearables are standard skeletal meshes, never Nanite (`specs/engine-tech/engine-tech.md` Requirement 4).
19. Helmets and head armor define per-race visibility for ears, frills, crests, and manes (hide, show, or use a cutout mesh). Chest armor, leg armor, cloaks, and Back-slot items (bags) define tail visibility (show through, or hide).
20. An editor data validator (Data Validation per `specs/engine-tech/engine-tech.md` Requirement 3) fails the content build when a wearable item is missing any of the 12 body variants, any Requirement 19 visibility setting for its slot (ear, frill, crest, and mane visibility for head items; tail visibility for chest, leg, cloak, and Back-slot items), or, for hand armor, any of the 12 first-person variants (Requirement 21), or when any dye zone defined on the item (`specs/crafting-jobs/crafting-jobs.md` Requirement 20) is missing from the material mask of any of the 12 body variants or, for hand armor, any of the 12 first-person variants, so every dye zone works on every body variant.
21. First-person arms use the race and sex's first-person arm mesh with the equipped hand armor's first-person variant for that race × sex combination, rendered with First Person Rendering (an Evaluate-marked row with a fallback in `specs/engine-tech/engine-tech.md` Requirement 3).

### Mirror
22. The Mirror is a Carpenter build piece (required level 5, crafted at a Workbench) placed with the Hammer (`specs/voxel-world/voxel-world.md` Requirement 24).
23. Interacting with a Mirror opens the Appearance editor for that player's character. The editor shows the same options as creation step 3. Race, sex, name, and starting class cannot be changed.
24. Changing appearance at a Mirror is free and has no cooldown. Confirming applies the new appearance immediately, replicates it to all clients, and saves it to the character save on the next save.
25. The world does not pause while a player uses the Mirror. A player using the Mirror is not invulnerable, and taking damage closes the editor without applying changes.

### Tuning Data
26. Every value marked "tuning value" or "starting value" in this spec that names no other table lives in `DT_Character_Races`.

### Racial Ability Rules
27. Racial active abilities follow these rules:
    - They cost no stamina and no mana, have no RequiredStat or RequiredValue, and award no class XP. They are not rows in `DT_Progression_ClassAbilities`.
    - Pounce's damage is the `specs/combat-loot/combat-loot.md` Requirement 8 formula computed with the Unarmed weapon profile at WeaponDamagePercent 1.0, including every penalty that formula applies. Its hits award One-Handed skill XP like Unarmed hits (`specs/character-progression/character-progression.md` Requirement 21).
    - Second Wind and Shed Skin heal a flat percentage of max health with no stat or skill scaling. The `specs/combat-loot/combat-loot.md` Requirement 39 healing formula does not apply.
    - Rally Howl's stamina regen bonus and radius scale with CHA like other party buffs (`specs/character-progression/character-progression.md` Requirement 5), with the CHA coefficients in `DT_Character_Races` (tuning values).

## Data Flow
1. Player opens Create Character → `UNamecCharacterCreationWidget` loads race rows from `DT_Character_Races` and option ranges from `DT_Character_AppearanceOptions`.
2. Each selection updates a local `FNamecCharacterDraft` (name, race, sex, appearance, starting class) and refreshes the preview actor `ANamecCharacterPreview`.
3. On Confirm, `UNamecCharacterFactory::CreateCharacter(Draft)` validates the name and option indexes, computes starting stats (10 + race + class), creates the character GUID, places the racial active ability in the last ability bar slot (Requirement 11), and writes a new `UNamecCharacterSave`.
4. On world entry, the character payload carries race, sex, and `FNamecAppearance` (`specs/multiplayer/multiplayer.md` Data Flow 3). The server spawns the pawn, applies the racial passive and downside effects, grants the racial active ability, and replicates race, sex, and appearance to all clients.
5. Each client builds the character's visual through Mutable from race, sex, appearance, and equipped items, selecting the matching body variant for every equipped item, then sets each equipped item's dye zone colors as runtime Substrate material parameters on the built meshes (`specs/crafting-jobs/crafting-jobs.md` Requirement 20). A dye change updates only those parameters and does not rebuild through Mutable.
6. At a Mirror, the client sends `ServerSetAppearance(FNamecAppearance)` → the server validates the race and sex match the character, option indexes are in range, and the player is within the Mirror's interaction range (Edge Case 14) → applies and replicates.

## Edge Cases
1. When a loaded character save contains an appearance option index outside the current range in `DT_Character_AppearanceOptions` (content changed), that option resets to the race default and the character still loads.
2. When a character save references a race ID missing from `DT_Character_Races`, the character is shown greyed out with "Race data missing" and cannot be selected.
3. When a client sends `ServerSetAppearance` with a different race or sex, or with out-of-range indexes, the server rejects the request and the appearance is unchanged.
4. When two local players on the same machine are creating characters at the same time, each creation flow runs independently in its own viewport.
5. When Pounce or Swing Leap would end inside terrain, a building piece, or a sealed boss arena barrier, the leap stops at the last valid position before the collision.
6. When Rally Howl hits a player who already has an active Rally Howl effect, the duration refreshes instead of stacking.
7. When Mauling Roar affects a Boss, the Boss is taunted for 3 s (tuning value) and takes 25% of Mauling Roar's poise damage (tuning value).
8. When a Felari falls 8 m or more, fall damage (`specs/combat-loot/combat-loot.md` Requirement 40) is calculated normally for the full fall distance.
9. When a player equips a helmet whose ear visibility is "hide", Felari, Hundari, Ursan, and Vanari ears are hidden while the helmet is equipped and reappear when it is unequipped.
10. When the player presses Back on the Race step, or cancels creation, the creation flow closes, the draft is discarded, and the player returns to the screen that opened creation (Requirement 1).
11. When a local player's controller disconnects during creation, the draft is kept and "Reconnect controller" shows in that viewport (`specs/multiplayer/multiplayer.md` Edge Case 1). When that local player is removed via the pause menu instead, the draft is discarded and no character save is written.
12. When a player changes sex on step 2 after visiting later steps, the race, appearance, and starting class choices are kept, and any appearance option index outside the new sex's range in `DT_Character_AppearanceOptions` resets to the race default.
13. When a player cancels creation opened from a mid-session join prompt (`specs/multiplayer/multiplayer.md` Requirement 4), the world keeps running, no pawn is spawned, and the player returns to that character select.
14. When a player using a Mirror becomes Downed, moves out of the Mirror's interaction range (tuning value), or the Mirror is destroyed or deconstructed while the editor is open, the editor closes without applying changes.
15. When two or more players use the same Mirror at the same time, each has an independent appearance editor.

## Acceptance Criteria
- [ ] A new Human Warrior starts with STR 13, CON 12, and every other stat 11.
- [ ] A new Ursan Mage starts with STR = 12 + the Mage's STR bonus, CON = 11 + the Mage's CON bonus, and every other stat = 10 + the Mage's bonus to that stat.
- [ ] Male and female characters of the same race and class have identical stats, abilities, carry weight, and collision capsule.
- [ ] Each race's active ability starts in the last ability bar slot (slot 6 at the starting bar size), can be moved to another slot (swapping with any ability already there), and cannot be removed.
- [ ] A Sauren's comfort range lower bound is 6 °C higher than a Human's in the same region, with the same WIS modifier and the same equipment.
- [ ] Mauling Roar deals 100 poise damage to a Hostile enemy and 25 poise damage to a Boss, and a taunted non-Boss enemy targets the Ursan for 6 s, then returns to its threat table.
- [ ] A Felari takes 0 fall damage from a 7 m fall and normal fall damage from a 9 m fall.
- [ ] A Vanari's MaxEquipLoad is 15% lower than a Human's with identical stats.
- [ ] A Hundari's Keen Nose outlines appear only in that player's split-screen viewport.
- [ ] Character creation can be completed start to finish with a gamepad in a 4-way split-screen viewport.
- [ ] Changing race on step 1 resets appearance to that race's default preset.
- [ ] A content build with a chest armor item missing its Felari female variant fails validation.
- [ ] A content build with a chest armor item whose Trim dye zone is missing from its Ursan male variant's material mask fails validation.
- [ ] Changing appearance at a Mirror updates the character on every connected client, and the change survives save → quit → load.
- [ ] A Mirror cannot change race, sex, or name.

## Key Files
- `Source/NAMEC/Character/Creation/NamecCharacterCreationWidget.h` — new; creation step screens and navigation.
- `Source/NAMEC/Character/Creation/NamecCharacterDraft.h` — new; in-progress creation state struct.
- `Source/NAMEC/Character/Creation/NamecCharacterFactory.h` — new; validation, starting stats, GUID, save creation.
- `Source/NAMEC/Character/Creation/NamecCharacterPreview.h` — new; preview actor with rotate, zoom, and preview outfits.
- `Source/NAMEC/Character/Appearance/NamecAppearance.h` — new; `FNamecAppearance` struct.
- `Source/NAMEC/Character/Appearance/NamecAppearanceComponent.h` — new; builds body and equipment visuals from race, sex, and appearance; replicates appearance.
- `Source/NAMEC/Character/Races/NamecRaceDefinition.h` — new; `DT_Character_Races` row struct.
- `Source/NAMEC/UI/OnScreenKeyboard/NamecOnScreenKeyboardWidget.h` — new; per-viewport gamepad on-screen keyboard for name entry, used through `INamecPlatform` (Requirement 5).
- `Source/NAMEC/Character/Races/NamecNightEyesComponent.h` — new; owning-viewport night vision post-process, night and underground activation, Settings toggle (Requirement 13).
- `Source/NAMEC/Character/Races/Abilities/` — new; `GA_Race_SecondWind`, `GA_Race_Pounce`, `GA_Race_RallyHowl`, `GA_Race_ShedSkin`, `GA_Race_SwingLeap`, `GA_Race_MaulingRoar`.
- `Source/NAMECEditor/NamecWearableVariantValidator.h` — new; editor-only module `NAMECEditor`; data validator for 12 body variants, Requirement 19 visibility settings, 12 first-person hand armor variants, and dye zone masks on every variant (Requirement 20).
- `Source/NAMEC/World/Building/NamecMirrorPiece.h` — new; Mirror build piece that opens the appearance editor.
- `Content/Character/Mutable/` — new; Mutable Customizable Object assets for the six race bodies, appearance options, the 12 body variants of every wearable, and materials that keep each dye zone's mask channel as a runtime material parameter (Requirements 15–18; `specs/crafting-jobs/crafting-jobs.md` Requirement 20).
- `Content/Character/Races/Effects/` — new; passive and downside gameplay effects for all six races.
- `Content/Data/DT_Character_Races.uasset` — new; six race rows, plus taunt duration, Boss taunt duration and poise damage percentage, Mirror interaction range, Night Eyes night hours, Rally Howl CHA coefficients, and other character creation tuning values (Requirement 26).
- `Content/Data/DT_Character_AppearanceOptions.uasset` — new; per-race preset counts and slider ranges.
