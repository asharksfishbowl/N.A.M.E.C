# Inventory

This page covers carry weight and encumbrance, equip load, equipment slots, capes and jewelry, the inventory screen, gold and item Value, dyes and quest items, favorites and hotkeys, and storage containers.

← [Home](Home.md)

Inventory is a categorized list limited by total weight. There is no grid, no slot count and no stack limit: any quantity stacks in one row per item definition plus potency value, so consumables with different potency are separate rows.

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
| Cloak | Capes: cloth capes (Tailor) and leather cloaks (Leatherworker) |
| Back | Tailor bags (add max carry weight) |
| Neck | Amulets (jewelry) |
| Ring ×2 | Rings (jewelry) |
| Right Hand | Weapons, shields, tools, torches |
| Left Hand | Weapons, shields, tools, torches |
| Ammo | Arrows (each bow shot uses 1) |

- Two-handed items use both hand slots: Two-Handed Swords, Axes and Hammers, and bows. See [Stats and Classes](Stats-and-Classes.md) for weapon categories.
- Two-handing a one-handed weapon **stows** your Left Hand item: it stays in its slot and still counts toward equip load, but gives none of its effects (including Insulation and Cooling) until you switch back. See [Combat and Loot](Combat-and-Loot.md).
- Spells are not items and never go in a hand slot. They are used from the ability bar.
- Clothing is Cloth armor category and shares the Head/Chest/Hands/Legs/Feet slots with armor.
- Insulation and Cooling come from every equipped item in any slot, including affixes, except a stowed Left Hand item. An Ursan's built-in Insulation adds to that total. See [Survival](Survival.md).
- Every wearable item fits every race and sex. See [Races and Character Creation](Races-and-Character-Creation.md).

## Capes and jewelry

**Capes** (Cloak slot):

- Cloth capes come from the Tailor and leather cloaks from the Leatherworker.
- A cape has Insulation or Cooling, a weight, and dye zones.
- Capes have no armor category, so no category penalty applies and no armor skill trains.
- Crafted capes roll rarity and affixes like other crafted equipment. Capes are listed under the Clothing tab.

**Jewelry** (amulets in Neck, rings in the two Ring slots):

- No base stats. Jewelry's only stats are random affixes, rolled by rarity like other loot.
- Jewelry dropped by enemies or found in loot chests is always Magic or better, so it has at least 1 affix.
- Sources: enemy drops, loot chests and Vendors. Vendor jewelry is Common with no affixes, like all Vendor equipment.
- The Blacksmith crafts a Plain Ring and Plain Amulet: always Common with 0 affixes, as a base for an Enchanter's add rune (see [Crafting and Jobs](Crafting-and-Jobs.md)).
- No durability (never needs repair) and no dye zones.

## Inventory screen

- **Tabs:** All, Favorites, Weapons, Armor, Clothing, Jewelry, Tools, Runes, Potions, Food, Ingredients, Materials, Building, Misc. Capes are under Clothing, and rings and amulets under Jewelry.
- **Each row:** icon, name (colored by rarity), quantity, weight, equipped/favorite marker.
- **Detail panel:** stats, affixes, durability, requirements, Value, dye colors per zone, comparison with the equipped item in the same slot, 3D preview.
- **Sort:** Name, Weight, Rarity, Item Level, Recently Acquired. Your sort choice is saved with your character.
- **Actions:** Equip/Unequip, Use, Favorite/Unfavorite, Drop (choose quantity), Inspect.
- The screen also opens the hand-crafting menu. See [Crafting and Jobs](Crafting-and-Jobs.md).
- The footer always shows carried weight / max carry weight and your gold.
- Your inventory is locked while you are Downed.

## Gold and Value

- **Gold** is a counter on your character, not an item. It weighs nothing, never counts toward carry weight or equip load, and doesn't appear in any tab.
- The footer's **Drop Gold** action drops an amount you choose as a shared pickup anyone can take. You can't drop more than you have.
- Every item has a **Value** in gold. Vendors buy and sell based on it. See [Factions and Kingdoms](Factions-and-Kingdoms.md).

## Dyes and quest items

- Every item keeps its own dye colors in 4 zones (Primary, Secondary, Accent, Trim). An empty zone shows the item's default color. Only the zones the item has can be dyed. See [Crafting and Jobs](Crafting-and-Jobs.md).
- Dye colors stay on the item when you drop it, another player picks it up, or it goes in a container. Everyone sees the colors on equipped gear.
- Two otherwise identical items with different dye colors show as separate rows.
- Consumables with different potency (for example a looted potion and one crafted by a high-level Alchemist) show as separate rows. Consumables with the same potency stack. Looted, bought and found arrows, potions and meals have Job level 1 potency.
- Dye items are listed under Materials.
- **Quest items** weigh nothing, are listed under Misc, and can't be dropped, stored in a container or sold. They disappear when the quest is turned in or abandoned.

## Favorites and hotkeys

- Any weapon, shield, tool, torch or consumable can be marked Favorite.
- The **Favorites quick menu** (gamepad D-pad Up, keyboard Q) lists favorites to equip or use. It does not pause the world.
- **8 hotkey slots** are assigned from Favorites.
  - Keyboard: keys 1–8 activate them.
  - Gamepad: each slot joins a cycle based on the item type. For a weapon, shield, tool or torch, you choose the hand when assigning. Two-handed weapons and bows join the right-hand cycle.

| Gamepad | Does |
|---------|------|
| D-pad Left | Cycle right-hand hotkeyed items |
| D-pad Right | Cycle left-hand hotkeyed items |
| D-pad Down | Cycle hotkeyed consumables |
| X (without LB) | Use the selected consumable (keyboard F) |

- With a shovel in your Right Hand, D-pad Left/Right cycle the shovel's fill material instead of hand hotkeys. In Hammer placement mode, D-pad Left/Right rotate the piece and holding X deconstructs instead. Outside placement mode, X always uses the consumable. See [Co-op and Controls](Co-op-and-Controls.md).

- If a hotkeyed item leaves your inventory (dropped or used up), its slot clears. A 0-durability item keeps its slot.
- Hotkeys are separate from the ability bar. See [Stats and Classes](Stats-and-Classes.md).

## Containers

- Storage containers (chests, barrels, crates) are crafted by Carpenters and placed as building pieces.
- Each container type has a weight capacity. A transfer that would go over it moves only what fits and shows "Container full".
- Containers use the same list UI with a two-column transfer view (player | container).
- Container contents do not count toward anyone's carried weight.
- Every player in the session can use every container. There are no locks.
- Containers belong to the world. Your inventory and gold belong to your character and leave with you.
- A destroyed or deconstructed container drops its contents as shared pickups.
- Crafting stations pull materials from containers within 10 m (starting value, tunable).

## Source spec

- [Inventory](../../specs/inventory/inventory.md)
- [Character Creation](../../specs/character-creation/character-creation.md) (racial carry weight and equip load)
- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md) (gold, Value, quest items)
- [Crafting Jobs](../../specs/crafting-jobs/crafting-jobs.md) (dyes)
- [Combat and Loot](../../specs/combat-loot/combat-loot.md) (arrows, bows, two-handing, jewelry rarity)
- [Character Progression](../../specs/character-progression/character-progression.md) (weapon and armor categories)
