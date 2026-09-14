# Spec: Inventory

## Overview
Inventory is Skyrim-style: no slots or grid, just a categorized list limited by total carry weight. A Favorites list and hotkeys give fast access to equipment and consumables during Souls-like combat. Shared terms are defined in `specs/game-foundation/game-foundation.md`.

## Goals
- Carrying capacity is limited by weight, not by slot count.
- The inventory list is fast to browse, sort, and filter with a gamepad.
- Players can swap weapons, shields, and consumables mid-combat without opening the full inventory. Spells are cast from the ability bar, not the inventory.

## Non-Goals
- Grid or Tetris-style slot inventories.
- Item stacking limits. Any quantity of an item stacks in one row.
- Fast travel (so there are no fast-travel restrictions when over-encumbered).
- Player-to-player trade UI (players drop items; see `specs/combat-loot/combat-loot.md` Requirement 34).

## Requirements

### Weight & Encumbrance
1. Every item definition has a `Weight` (float, may be 0). An item row's total weight = `Weight × Quantity`.
2. Carried weight = the total weight of every item the character holds, including equipped items.
3. `MaxCarryWeight` = base 150 + 10 × STR modifier + equipped bag bonus (Requirement 12) + perk and affix bonuses (tuning values in `DT_Progression_DerivedStats`). `MaxCarryWeight` is a GAS attribute on `UNamecAttributeSet`.
4. There is no hard cap. A player can always pick up, loot, or receive an item.
5. When carried weight > `MaxCarryWeight`, the character is **Over-Encumbered**: cannot sprint or dodge roll, walk speed is 50%, and stamina regen is 50% (tuning values). A HUD icon shows while Over-Encumbered.
6. **Equip load** is separate from carried weight and drives combat movement: equip load = the total weight of every item in an equipment slot (Requirement 12) ÷ `MaxEquipLoad`. `MaxEquipLoad` = base 40 + 3 × STR modifier + 2 × CON modifier (tuning values in `DT_Progression_DerivedStats`) and is a GAS attribute on `UNamecAttributeSet`. Dodge-roll tiers in `specs/combat-loot/combat-loot.md` Requirement 3 use equip load.

### Inventory Screen
7. The inventory screen is a vertical list with category tabs: All, Favorites, Weapons, Armor, Clothing, Tools, Runes, Potions, Food, Ingredients, Materials, Building, Misc.
8. Each row shows: icon, name (colored by rarity), quantity, weight, and an equipped/favorite marker. The selected item shows a detail panel with stats, affixes, durability, requirements, a comparison against the currently equipped item in the same slot, and a 3D preview.
9. Sort options: Name, Weight, Rarity, Item Level, Recently Acquired. Sort choice persists per character.
10. Actions on a selected item: Equip/Unequip, Use (consumables), Favorite/Unfavorite, Drop (choose quantity), Inspect. The inventory screen also opens the hand-crafting menu (see `specs/crafting-jobs/crafting-jobs.md` Requirement 16).
11. The footer always shows carried weight / `MaxCarryWeight`.

### Equipment Slots
12. Equipment slots: Head, Chest, Hands, Legs, Feet, Cloak, Back, Neck, Ring ×2, Right Hand, Left Hand, Ammo. Two-handed weapons occupy both hand slots. Hand slots hold only weapons, shields, tools, and torches. Spells are class abilities, not items, and are used from the ability bar (Requirement 17). Tailor bags equip in the Back slot and add `MaxCarryWeight` by the bonus value on the bag's item definition. Arrows equip in the Ammo slot.
13. Clothing (Tailor) and armor share the Head/Chest/Hands/Legs/Feet slots. Clothing has the Cloth armor category (`specs/character-progression/character-progression.md` Requirement 27). Insulation and Cooling come from every equipped item in any equipment slot, including Insulation and Cooling affixes.

### Favorites & Hotkeys
14. Any weapon, shield, tool, torch, or consumable can be marked Favorite.
15. A Favorites quick menu opens with D-pad Up (gamepad) or Q (keyboard) and does not pause the world. It lists favorites for equipping or using.
16. Eight hotkey slots can be assigned from Favorites. Each slot's gamepad cycle (right hand, left hand, or consumables) comes from the assigned item's type, and for a weapon, shield, tool, or torch the hand is chosen when the slot is assigned. Two-handed weapons join the right-hand cycle. Keyboard: keys 1–8 activate them. Gamepad: D-pad Left cycles right-hand hotkeyed items, D-pad Right cycles left-hand items, and D-pad Down cycles hotkeyed consumables, and pressing X without LB held uses the selected consumable (Souls-style belt; keyboard F). Default bindings are listed in `specs/game-foundation/game-foundation.md` Requirement 14.
17. The ability bar from `specs/character-progression/character-progression.md` Requirement 18 is separate from the hotkeys. Spells and all other class abilities are used through the ability bar, with the inputs listed in that requirement.

### Containers
18. Storage containers (chests, barrels, crates) are crafted and placed. Each container type has a weight capacity in `DT_Inventory_Containers`. Container contents use the same list UI with a two-column transfer view (player | container).
19. Containers do not count toward any player's carried weight.

### Tuning Data
20. Every value marked "tuning value" in this spec that names no other table lives in `DT_Inventory_Rules`.

## Data Flow
1. Player picks up a world item → client sends `ServerPickup(PickupId)` → server validates range and pickup ownership (per-player loot owned by the requesting player, or shared) → adds the item to `UNamecInventoryComponent` → recalculates carried weight → applies or removes the `GE_OverEncumbered` effect.
2. Player equips an item → `ServerEquip(ItemInstanceId, Slot)` → server validates slot rules → applies the item's gameplay effects (armor, affixes, insulation) → recalculates equip load.
3. Inventory contents replicate only to the owning connection. Equipped item visuals replicate to all clients.
4. On save, the inventory component serializes every item instance (definition ID, quantity, rarity, affixes, durability, item level), equipped slots, favorites, hotkeys, and sort choice into `UNamecCharacterSave`.

## Edge Cases
1. When an item is picked up that pushes carried weight over `MaxCarryWeight`, the pickup succeeds and Over-Encumbered applies immediately.
2. When STR drops (debuff expires, affix item unequipped) and carried weight now exceeds `MaxCarryWeight`, Over-Encumbered applies on the attribute-change callback.
3. When a hotkeyed item leaves the inventory (dropped or consumed), its hotkey slot clears. An item at 0 durability stays in the inventory, keeps its hotkey slot, and is unusable until repaired (`specs/combat-loot/combat-loot.md` Requirement 35).
4. When a player puts an item into a container that would exceed the container's weight capacity, the transfer moves only the quantity that fits and shows "Container full".
5. When two players transfer into the same container at once, the server processes transfers in receive order against the container's capacity.
6. When a player is downed, their inventory is inaccessible until revived.
7. When a storage container is destroyed or deconstructed, its contents drop at the container's location as shared world pickups.

## Acceptance Criteria
- [ ] A character with STR 10 has `MaxCarryWeight` 150.
- [ ] Picking up items past `MaxCarryWeight` succeeds and disables sprint and dodge roll.
- [ ] Dropping items back under `MaxCarryWeight` removes Over-Encumbered.
- [ ] Dodge-roll tier changes when heavy armor is equipped even if carried weight is unchanged.
- [ ] Inventory tabs filter correctly, and the chosen sort persists across save → load.
- [ ] Hotkey 1 equips its assigned weapon. D-pad Down cycles hotkeyed consumables.
- [ ] Opening the Favorites menu does not pause the world in a multiplayer session.
- [ ] A container rejects the portion of a transfer beyond its weight capacity.
- [ ] Every item's rarity, affixes, and durability survive save → quit → load.

## Key Files
- `Source/NAMEC/Inventory/NamecInventoryComponent.h` — new; item storage, weight, equip slots, favorites, hotkeys.
- `Source/NAMEC/Inventory/NamecItemDefinition.h` — new; item definition data asset (weight, category, slot, armor category, `Poise` for armor, `PoiseDamage` for weapons, effects; see `specs/combat-loot/combat-loot.md` Requirement 38).
- `Source/NAMEC/Inventory/NamecContainerActor.h` — new; placeable storage container (subclass of `ANamecBuildPiece`) with weight capacity.
- `Source/NAMEC/UI/Inventory/` — new; inventory list, detail panel, transfer view, Favorites quick menu.
- `Content/Inventory/Effects/GE_OverEncumbered.uasset` — new.
- `Content/Data/DT_Inventory_Containers.uasset` — new; container types and capacities.
- `Content/Data/DT_Inventory_Rules.uasset` — new; Over-Encumbered penalties and other inventory tuning values.
