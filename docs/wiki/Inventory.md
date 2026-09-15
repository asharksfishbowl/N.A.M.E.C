# Inventory

This page covers carry weight and encumbrance, equip load, equipment slots, the inventory screen, favorites and hotkeys, and storage containers.

← [Home](Home.md)

Inventory is a categorized list limited by total weight. There is no grid, no slot count and no stack limit: any quantity of an item stacks in one row.

## Carry weight

- Each item has a weight (can be 0). A row weighs weight × quantity.
- Carried weight includes equipped items.
- **Max carry weight** = 150 + 10 × STR modifier + equipped bag bonus + perk and affix bonuses + racial bonus (starting values, tunable).
  - The racial bonus is +50 for an Ursan (Thick Hide) and 0 for every other race.
  - For a non-Ursan with STR 10 and no bag, perk or affix bonuses, that gives 150.
- There is **no hard cap**. You can always pick up, loot or receive an item.

## Over-Encumbered

When carried weight goes over max carry weight:

- You cannot sprint or dodge roll.
- Walk speed is 50% and stamina regen is 50%.
- A HUD icon shows.

It applies right away when a pickup pushes you over, or when STR drops (a debuff ends, an affix item is removed). Dropping below the limit removes it.

## Equip load

Equip load is separate from carried weight and controls dodge rolls.

- **Equip load** = total weight of everything in equipment slots ÷ max equip load.
- **Max equip load** = (40 + 3 × STR modifier + 2 × CON modifier) × racial multiplier (starting values, tunable). The racial multiplier is 0.85 for a Vanari (Light Frame) and 1 for every other race.

| Tier | Equip load | Dodge roll |
|------|-----------|------------|
| Light | under 30% | Allowed |
| Medium | 30% to under 70% | Allowed |
| Heavy | 70% to 100% | Allowed |
| Overloaded | over 100% | Not allowed |

Each tier has its own i-frame window and roll recovery. Heavy armor can change your tier even if your carried weight doesn't change. See [Combat and Loot](Combat-and-Loot.md).

## Equipment slots

| Slot | Holds |
|------|-------|
| Head | Armor or clothing |
| Chest | Armor or clothing |
| Hands | Armor or clothing |
| Legs | Armor or clothing |
| Feet | Armor or clothing |
| Cloak | Not specified yet |
| Back | Tailor bags (add max carry weight) |
| Neck | Not specified yet |
| Ring ×2 | Not specified yet |
| Right Hand | Weapons, shields, tools, torches |
| Left Hand | Weapons, shields, tools, torches |
| Ammo | Arrows |

- Two-handed weapons use both hand slots.
- Spells are not items and never go in a hand slot. They are used from the ability bar.
- Clothing is Cloth armor category and shares the Head/Chest/Hands/Legs/Feet slots with armor.
- Insulation and Cooling come from every equipped item in any slot, including affixes. An Ursan's built-in Insulation adds to that total. See [Survival](Survival.md).
- Every wearable item fits every race and sex. See [Races and Character Creation](Races-and-Character-Creation.md).

## Inventory screen

- **Tabs:** All, Favorites, Weapons, Armor, Clothing, Tools, Runes, Potions, Food, Ingredients, Materials, Building, Misc.
- **Each row:** icon, name (colored by rarity), quantity, weight, equipped/favorite marker.
- **Detail panel:** stats, affixes, durability, requirements, comparison with the equipped item in the same slot, 3D preview.
- **Sort:** Name, Weight, Rarity, Item Level, Recently Acquired. Your sort choice is saved with your character.
- **Actions:** Equip/Unequip, Use, Favorite/Unfavorite, Drop (choose quantity), Inspect.
- The screen also opens the hand-crafting menu. See [Crafting and Jobs](Crafting-and-Jobs.md).
- The footer always shows carried weight / max carry weight.
- Your inventory is locked while you are Downed.

## Favorites and hotkeys

- Any weapon, shield, tool, torch or consumable can be marked Favorite.
- The **Favorites quick menu** (gamepad D-pad Up, keyboard Q) lists favorites to equip or use. It does not pause the world.
- **8 hotkey slots** are assigned from Favorites.
  - Keyboard: keys 1–8 activate them.
  - Gamepad: each slot joins a cycle based on the item type. For a weapon, shield, tool or torch, you choose the hand when assigning. Two-handed weapons join the right-hand cycle.

| Gamepad | Does |
|---------|------|
| D-pad Left | Cycle right-hand hotkeyed items |
| D-pad Right | Cycle left-hand hotkeyed items |
| D-pad Down | Cycle hotkeyed consumables |
| X (without LB) | Use the selected consumable (keyboard F) |

- If a hotkeyed item leaves your inventory (dropped or used up), its slot clears. A 0-durability item keeps its slot.
- Hotkeys are separate from the ability bar. See [Stats and Classes](Stats-and-Classes.md).

## Containers

- Storage containers (chests, barrels, crates) are crafted by Carpenters and placed as building pieces.
- Each container type has a weight capacity. A transfer that would go over it moves only what fits and shows "Container full".
- Containers use the same list UI with a two-column transfer view (player | container).
- Container contents do not count toward anyone's carried weight.
- Every player in the session can use every container. There are no locks.
- Containers belong to the world. Your inventory belongs to your character and leaves with you.
- A destroyed or deconstructed container drops its contents as shared pickups.
- Crafting stations pull materials from containers within 10 m (starting value, tunable).

## Source spec

- [Inventory](../../specs/inventory/inventory.md)
- [Character Creation](../../specs/character-creation/character-creation.md) (racial carry weight and equip load)
