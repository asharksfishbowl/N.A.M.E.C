# Spec: Inventory

## Overview
Inventory is Skyrim-style: no slots or grid, just a categorized list limited by total carry weight. A Favorites list and hotkeys give fast access to equipment and consumables during Souls-like combat. Shared terms are defined in `specs/game-foundation/game-foundation.md`.

## Goals
- Carrying capacity is limited by weight, not by slot count.
- The inventory list is fast to browse, sort, and filter with a gamepad.
- Players can swap weapons, shields, and consumables mid-combat without opening the full inventory. Spells are cast from the ability bar, not the inventory.

## Non-Goals
- Grid or Tetris-style slot inventories.
- Item stacking limits. Any quantity stacks in one row per item definition plus potency value, rune tier, and waterskin remaining drinks, so consumables with different potency values are separate rows (Edge Case 11), and runes with different tiers and waterskins with different remaining drinks are separate rows (Edge Case 12). Items whose instances differ in other ways, such as dye colors, also show as separate rows (Edge Case 8).
- Fast travel (so there are no fast-travel restrictions when over-encumbered).
- Player-to-player trade UI (players drop items; see `specs/combat-loot/combat-loot.md` Requirement 34).

## Requirements

### Weight & Encumbrance
1. Every item definition has a `Weight` (float, may be 0). An item row's total weight = `Weight × Quantity`.
2. Carried weight = the total weight of every item the character holds, including equipped items.
3. `MaxCarryWeight` = base 150 + 10 × STR modifier + equipped bag bonus (Requirement 12) + perk and affix bonuses + racial carry weight bonus (`specs/character-creation/character-creation.md` Requirement 12; Ursan Thick Hide +50, otherwise 0) (tuning values in `DT_Progression_DerivedStats`; the racial bonus lives in `DT_Character_Races`). `MaxCarryWeight` is a GAS attribute on `UNamecAttributeSet`.
4. There is no hard cap. A player can always pick up, loot, or receive an item.
5. When carried weight > `MaxCarryWeight`, the character is **Over-Encumbered**: cannot sprint or dodge roll, walk speed is 50%, and stamina regen is 50% (tuning values). A HUD icon shows while Over-Encumbered.
6. **Equip load** is separate from carried weight and drives combat movement: equip load = the total weight of every item in an equipment slot (Requirement 12), including a stowed Left Hand item (`specs/combat-loot/combat-loot.md` Requirement 54), ÷ `MaxEquipLoad`. `MaxEquipLoad` = (base 40 + 3 × STR modifier + 2 × CON modifier) × racial equip load multiplier (`specs/character-creation/character-creation.md` Requirement 12; Vanari Light Frame 0.85, otherwise 1) (tuning values in `DT_Progression_DerivedStats`; the racial multiplier lives in `DT_Character_Races`) and is a GAS attribute on `UNamecAttributeSet`. Dodge-roll tiers in `specs/combat-loot/combat-loot.md` Requirement 3 use equip load.

### Inventory Screen
7. The inventory screen is a vertical list with category tabs: All, Favorites, Weapons, Armor, Clothing, Jewelry, Tools, Runes, Potions, Food, Ingredients, Materials, Building, Misc. Capes (Requirement 25) list under Clothing, and rings and amulets (Requirement 26) list under Jewelry.
8. Each row shows: icon, name (colored by rarity), quantity, weight, and an equipped/favorite marker. The selected item shows a detail panel with stats, affixes, durability, requirements, `Value` (Requirement 21), dye colors per zone (Requirement 23), a comparison against the currently equipped item in the same slot, and a 3D preview.
9. Sort options: Name, Weight, Rarity, Item Level, Recently Acquired. Sort choice persists per character.
10. Actions on a selected item: Equip/Unequip, Use (consumables and waterskins; a waterskin's Use drinks 1, `specs/survival/survival.md` Requirement 6), Favorite/Unfavorite, Drop (choose quantity), Inspect. The inventory screen also opens the hand-crafting menu (see `specs/crafting-jobs/crafting-jobs.md` Requirement 16). Quest items cannot be dropped (Requirement 24).
11. The footer always shows carried weight / `MaxCarryWeight` and the character's gold (Requirement 22).

### Equipment Slots
12. Equipment slots: Head, Chest, Hands, Legs, Feet, Cloak, Back, Neck, Ring ×2, Right Hand, Left Hand, Ammo. Two-handed items occupy both hand slots: two-handed weapons (the Two-Handed Sword, Two-Handed Axe, and Two-Handed Hammer categories) and bows (`specs/character-progression/character-progression.md` Requirement 29; `specs/combat-loot/combat-loot.md` Requirement 55). The Right Hand holds weapons, tools (axes, pickaxes, shovels, Hammers, and Fishing Rods), and torches. The Left Hand holds one-handed weapons of any weapon category except Staff, shields, and torches. Tools and Staffs can be equipped only in the Right Hand, and shields only in the Left Hand (`specs/character-progression/character-progression.md` Requirement 29). What the block input does with each Left Hand item, and which item blocks and parries, follow `specs/combat-loot/combat-loot.md` Requirement 59. The Cloak slot holds capes (Requirement 25), and the Neck and Ring slots hold jewelry: amulets in Neck and rings in each Ring slot (Requirement 26). Spells are class abilities, not items, and are used from the ability bar (Requirement 17). Tailor bags equip in the Back slot and add `MaxCarryWeight` by the bonus value on the bag's item definition. Arrows equip in the Ammo slot, and each bow shot consumes 1 arrow from it (`specs/combat-loot/combat-loot.md` Requirement 53). While the Right Hand weapon is two-handed, the Left Hand item is stowed: it stays in the Left Hand slot but grants none of its effects (`specs/combat-loot/combat-loot.md` Requirement 54).
13. Clothing (Tailor) and armor share the Head/Chest/Hands/Legs/Feet slots. Clothing has the Cloth armor category (`specs/character-progression/character-progression.md` Requirement 27). Insulation and Cooling come from every equipped item in any equipment slot, including Insulation and Cooling affixes, except a stowed Left Hand item (`specs/combat-loot/combat-loot.md` Requirement 54). Racial built-in Insulation (`specs/character-creation/character-creation.md` Requirement 12) adds to this total (`specs/survival/survival.md` Requirement 11).

### Favorites & Hotkeys
14. Any weapon, shield, tool, torch, consumable, or waterskin can be marked Favorite.
15. A Favorites quick menu opens with D-pad Up (gamepad) or Q (keyboard) and does not pause the world. It lists favorites for equipping or using.
16. Eight hotkey slots can be assigned from Favorites. Each slot's gamepad cycle (right hand, left hand, or consumables) comes from the assigned item's type, and for a one-handed weapon other than a Staff, or a torch, the hand is chosen when the slot is assigned. Shields join the left-hand cycle, and tools and Staffs join the right-hand cycle. Two-handed items (two-handed weapons and bows) join the right-hand cycle. Waterskins join the consumables cycle like consumables (`specs/survival/survival.md` Requirement 6). Keyboard: keys 1–8 activate them. Gamepad: D-pad Left cycles right-hand hotkeyed items, D-pad Right cycles left-hand items, and D-pad Down cycles hotkeyed consumables, and pressing X without LB held uses the selected consumable (Souls-style belt; keyboard F). While a shovel is equipped in the Right Hand, D-pad Left and D-pad Right cycle the shovel's fill material instead (`specs/voxel-world/voxel-world.md` Requirement 40), and in Hammer placement mode they rotate the piece and X deconstructs instead of using the consumable (`specs/voxel-world/voxel-world.md` Requirement 41). Default bindings are listed in `specs/game-foundation/game-foundation.md` Requirement 14.
17. The ability bar from `specs/character-progression/character-progression.md` Requirement 18 is separate from the hotkeys. Spells and all other class abilities are used through the ability bar, with the inputs listed in that requirement.

### Containers
18. Storage containers (chests, barrels, crates) are crafted and placed. Each container type has a weight capacity in `DT_Inventory_Containers`. Container contents use the same list UI with a two-column transfer view (player | container).
19. Containers do not count toward any player's carried weight.

### Tuning Data
20. Every value marked "tuning value" in this spec that names no other table lives in `DT_Inventory_Rules`.

### Value & Gold
21. Every item definition has a `Value` (integer gold, 0 or more). Vendors buy and sell items at prices derived from `Value` (`specs/factions-kingdoms/factions-kingdoms.md` Requirements 32 and 33).
22. Gold is a per-character counter, not an item (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 28). Gold has 0 weight, never counts toward carried weight or equip load, and does not appear in any category tab. The footer's Drop Gold action asks for an amount and drops that gold as a shared world pickup (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 30).

### Dyes & Quest Items
23. Every item instance has 4 dye zone slots (Primary, Secondary, Accent, Trim), each holding a color ID from `DT_Crafting_DyeColors` or empty, where empty means the item's default color (`specs/crafting-jobs/crafting-jobs.md` Requirements 18–22). Only the zones the item definition defines can hold a color. Dye colors are saved with the item instance, replicate to all clients with equipped item visuals, and stay on the item instance when it is dropped, picked up by another player, or moved into or out of a container.
24. Quest items (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 46) are flagged on their item definition, have 0 weight and `Value` 0, list under the Misc tab, and cannot be dropped, moved into a container, or sold. Dye items list under the Materials tab.

### Capes & Jewelry
25. The Cloak slot holds capes: cloth capes crafted by the Tailor and leather cloaks crafted by the Leatherworker (`specs/crafting-jobs/crafting-jobs.md` Requirement 1). A cape's item definition has an Insulation or Cooling value, a `Weight`, and 1–4 dye zones (`specs/crafting-jobs/crafting-jobs.md` Requirement 20). Capes have no armor category, so no armor category penalty applies and no armor skill trains (`specs/character-progression/character-progression.md` Requirement 27). A cape's Insulation or Cooling adds to the equipped total (Requirement 13).
26. The Neck slot holds amulets and each Ring slot holds a ring. Amulets and rings are jewelry:
    - Jewelry has no base stats. Its only stats are the random affixes it rolls by rarity, from the Neck or Ring slot's affix pool in `DT_Loot_Affixes`, under the same loot rules as other equipment (`specs/combat-loot/combat-loot.md` Requirements 30–32).
    - Jewelry rolled as loot never rolls Common: a jewelry item dropped by an enemy or found in a loot chest rolls Magic or above, so it always has at least 1 affix (`specs/combat-loot/combat-loot.md` Requirement 30).
    - Jewelry comes from enemy loot drops, loot chests, and Vendors (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 34). The Blacksmith also crafts the Plain Ring and Plain Amulet at Common rarity with 0 affixes, as bases for the Enchanter's add-affix rune (`specs/crafting-jobs/crafting-jobs.md` Requirements 10 and 12).
    - Jewelry has no armor category (`specs/character-progression/character-progression.md` Requirement 27) and no weapon category.
    - Jewelry has no durability: it never loses durability, never reaches 0 durability, and never needs repair (`specs/combat-loot/combat-loot.md` Requirement 35).
    - Jewelry has 0 dye zones (`specs/crafting-jobs/crafting-jobs.md` Requirement 20).

## Data Flow
1. Player picks up a world item → client sends `ServerPickup(PickupId)` → server validates range and pickup ownership (per-player loot owned by the requesting player, or shared) → adds the item to `UNamecInventoryComponent` → recalculates carried weight → applies or removes the `GE_OverEncumbered` effect.
2. Player equips an item → `ServerEquip(ItemInstanceId, Slot)` → server validates slot rules → applies the item's gameplay effects (armor, affixes, insulation) → recalculates equip load.
3. Inventory contents and gold replicate only to the owning connection. Equipped item visuals, including each equipped item instance's dye colors, replicate to all clients.
4. On save, the inventory component serializes every item instance (definition ID, quantity, rarity, affixes, durability (none for jewelry), item level, dye colors, potency value for consumables, rune tier for runes (`specs/crafting-jobs/crafting-jobs.md` Requirement 12), and remaining drinks for waterskins (`specs/survival/survival.md` Requirement 6)), equipped slots, favorites, hotkeys, and sort choice into `UNamecCharacterSave`, and `UNamecCurrencyComponent` serializes gold (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 28).

## Edge Cases
1. When an item is picked up that pushes carried weight over `MaxCarryWeight`, the pickup succeeds and Over-Encumbered applies immediately.
2. When STR drops (debuff expires, affix item unequipped) and carried weight now exceeds `MaxCarryWeight`, Over-Encumbered applies on the attribute-change callback.
3. When a hotkeyed item leaves the inventory (dropped or consumed), its hotkey slot clears. An item at 0 durability stays in the inventory, keeps its hotkey slot, and is unusable until repaired (`specs/combat-loot/combat-loot.md` Requirement 35).
4. When a player puts an item into a container that would exceed the container's weight capacity, the transfer moves only the quantity that fits and shows "Container full".
5. When two players transfer into the same container at once, the server processes transfers in receive order against the container's capacity.
6. When a player is downed, their inventory is inaccessible until revived.
7. When a storage container is destroyed or deconstructed, its contents drop at the container's location as shared world pickups.
8. When items of the same definition have different dye colors, they show as separate rows, because dye colors belong to each item instance.
9. When a player tries to drop more gold than the character holds, the Drop Gold amount is capped at the character's gold.
10. When a player tries to move a quest item into a container or drop it, the action is unavailable for that row.
11. When a player holds consumables of the same item definition with different potency values (for example a looted healing potion at Job level 1 potency and one crafted by an Alchemist 60), they show as separate rows and never stack together (`specs/crafting-jobs/crafting-jobs.md` Requirement 11). Consumables of the same definition and the same potency value stack in one row.
12. When a player holds runes of the same item definition with different rune tiers (`specs/crafting-jobs/crafting-jobs.md` Requirement 12), or waterskins with different remaining drinks (`specs/survival/survival.md` Requirement 6), they show as separate rows and never stack together, and each keeps its rune tier or remaining drinks through save → quit → load, dropping, and container transfers.

## Acceptance Criteria
- [ ] A non-Ursan character with STR 10 and no bag, perk, or affix bonuses has `MaxCarryWeight` 150.
- [ ] Picking up items past `MaxCarryWeight` succeeds and disables sprint and dodge roll.
- [ ] Dropping items back under `MaxCarryWeight` removes Over-Encumbered.
- [ ] Dodge-roll tier changes when heavy armor is equipped even if carried weight is unchanged.
- [ ] Inventory tabs filter correctly, and the chosen sort persists across save → load.
- [ ] Hotkey 1 equips its assigned weapon. D-pad Down cycles hotkeyed consumables.
- [ ] A waterskin can be marked Favorite and hotkeyed into the consumables cycle, and pressing X with it selected, or its Use action, drinks 1 and lowers its remaining drinks by 1.
- [ ] Opening the Favorites menu does not pause the world in a multiplayer session.
- [ ] A container rejects the portion of a transfer beyond its weight capacity.
- [ ] Every item's rarity, affixes, durability, and dye colors survive save → quit → load.
- [ ] The footer shows the character's gold, and gaining gold does not change carried weight.
- [ ] Dropping 50 gold from the footer creates a shared world pickup that another player can pick up for 50 gold.
- [ ] The detail panel shows each item's `Value`.
- [ ] A quest item cannot be dropped or placed in a container.
- [ ] A cape lists under the Clothing tab, and a ring and an amulet list under the Jewelry tab.
- [ ] A leather cloak equips in the Cloak slot, adds its Insulation to the equipped total, and trains no armor skill, and a Mage wearing it takes no category penalty.
- [ ] 100 rings dropped by enemies all roll Magic or above with at least 1 affix, none has base stats, dye zones, or durability, and a death's durability loss leaves equipped jewelry unchanged.
- [ ] Two arrows of the same definition with different potency values show as two rows, and arrows with equal potency stack in one row.
- [ ] Equipping a bow occupies both hand slots, so the Left Hand item is unequipped.
- [ ] A dagger or torch can be equipped in either hand, a pickaxe, Fishing Rod, or Staff only in the Right Hand, and a shield only in the Left Hand.
- [ ] A tier-1 rune and a tier-3 rune of the same definition show as two rows, and a waterskin with 2 drinks left and a full waterskin show as two rows, and each keeps its rune tier or remaining drinks after save → quit → load.

## Key Files
- `Source/NAMEC/Inventory/NamecInventoryComponent.h` — new; item storage with one row per item definition plus potency value, rune tier, and waterskin remaining drinks for stackable items (Edge Cases 11 and 12), weight, equip slots including Cloak, Neck, and Ring (Requirements 25 and 26), favorites, hotkeys (including waterskins in the consumables cycle, Requirement 16).
- `Source/NAMEC/Inventory/NamecItemDefinition.h` — new; item definition data asset (weight, `Value`, category, slot, armor category, weapon category (`specs/character-progression/character-progression.md` Requirement 29), `Poise` for armor, `PoiseDamage` for weapons, block percentage and parry-capable flag for weapons and shields (`specs/combat-loot/combat-loot.md` Requirements 4, 5, and 59), `WeaponBase` for weapons and every tool except the Fishing Rod (`specs/combat-loot/combat-loot.md` Requirement 56), `ArrowDamage` for arrows (`specs/combat-loot/combat-loot.md` Requirement 53), Insulation or Cooling for capes (Requirement 25), `DrinkCapacity` for waterskins (`specs/survival/survival.md` Requirement 6), the no-durability flag and no base stats for jewelry (Requirement 26), dye zones and material mask channels, quest item flag, effects; see `specs/combat-loot/combat-loot.md` Requirement 38 and `specs/crafting-jobs/crafting-jobs.md` Requirement 20).
- `Source/NAMEC/Inventory/NamecContainerActor.h` — new; placeable storage container (subclass of `ANamecBuildPiece`) with weight capacity.
- `Source/NAMEC/UI/Inventory/` — new; inventory list, detail panel, transfer view, Favorites quick menu, footer gold display and Drop Gold action.
- `Content/Inventory/Effects/GE_OverEncumbered.uasset` — new.
- `Content/Data/DT_Inventory_Containers.uasset` — new; container types and capacities.
- `Content/Data/DT_Inventory_Rules.uasset` — new; Over-Encumbered penalties and other inventory tuning values.
