# Crafting and Jobs

This page covers the 9 crafting Jobs, stations, hand-crafting and the fresh-character path, station tiers and boss gating, crafted gear quality, Enchanter runes and the enchanting screen, repair, which recipe an item came from, and dyes.

← [Home](Home.md)

## Jobs

Every character can level every Job, independent of class. Jobs level 1–100 (starting value, tunable) by crafting. Job XP does **not** feed character XP.

| Job | Makes | Station |
|-----|-------|---------|
| Blacksmith | Metal weapons (One-Handed Sword, One-Handed Axe, One-Handed Mace, Dagger, Spear, Two-Handed Sword, Two-Handed Axe, Two-Handed Hammer), metal tools (axes, pickaxes, shovels), Repair Kits, Plain Rings and Plain Amulets | Forge |
| Armorsmith | Metal armor (Metal category), shields (Shield category) | Forge |
| Carpenter | Wooden building pieces, furniture (including beds and Mirrors, Carpenter level 5), storage containers (chests, barrels, crates), training dummies, torches, bows, staves, arrows, wooden tools (including Hammers and Fishing Rods), stone tools (Stone Axe, Stone Pickaxe, Stone Shovel), Workbenches, Campfires, Stonecutters, Alchemy Tables, Looms, Tanning Racks, Dye Stations | Workbench |
| Mason | Stone building pieces, Forges, Enchanting Altars, stone fortifications | Stonecutter |
| Alchemist | Potions (healing, mana, resistance), poisons and weapon oils (coat your weapon with a status buildup; see [Combat and Loot](Combat-and-Loot.md)), dyes | Alchemy Table |
| Cook | Meals that restore hunger and thirst and grant timed buffs | Cooking Fire (the Campfire is the tier-1 Cooking Fire) |
| Tailor | Cloth armor, cold-weather and hot-weather clothing (all Cloth category), cloth capes (no armor category), bags | Loom |
| Leatherworker | Leather armor (Leather category), leather cloaks (no armor category), waterskins | Tanning Rack |
| Enchanter | Runes that add or reroll affixes on existing gear | Enchanting Altar |

## Job XP, recipes and perks

- Each successful craft gives Job XP = recipe base XP × recipe tier multiplier.
- A recipe more than 20 levels below your Job level gives 10% XP (starting values, tunable).
- Recipes are known automatically once your Job level meets the requirement. There are no recipe scrolls.
- Each recipe lists: Job, required Job level, station and station tier, inputs, output, craft time, base XP, tier multiplier.
- Each Job gains a perk at levels 25, 50, 75 and 100. Example from the spec: Cook 50 makes meals last 25% longer.

## Using a station

- Stations are placeable building pieces, placed from the Hammer build menu.
- A station pulls materials from your inventory and from any storage container within 10 m (starting value, tunable). All containers are open to every player. There are no locks or ownership.
- You can queue up to 10 crafts (starting value, tunable). Crafting continues while you stay within 10 m and pauses when you leave. Materials already consumed for the current craft stay reserved.
- If materials run out mid-queue, the rest of the queue is cancelled with "Not enough materials". Finished crafts are kept.
- Two players at the same station each have their own queue. Materials from shared containers are reserved at the start of each craft, first come, first served.
- Disconnecting or quitting mid-craft cancels the current craft and returns its materials to your inventory.
- If a station is destroyed or deconstructed with a Hammer, all queues on it are cancelled and reserved materials go back to their crafters.
- There is no automation (no conveyors or auto-smelters). Town Vendors exist but never sell boss materials (see [Factions and Kingdoms](Factions-and-Kingdoms.md)).

## Hand-crafting and the fresh-character path

A hand-crafting menu, opened from the inventory screen, crafts without any station. It uses materials from your own inventory only.

| Hand recipe | Inputs |
|-------------|--------|
| Stone Axe | Stick, Stone, Fiber |
| Stone Pickaxe | Stick, Stone, Fiber |
| Stone Shovel | Stick, Stone, Fiber |
| Workbench | Stick, Stone, Fiber (one recipe) or Wood (a second recipe) |
| Hammer | drawn only from Stick, Stone, Fiber and Wood |
| Campfire | drawn only from Stick, Stone, Fiber and Wood |
| Torch | drawn only from Stick, Stone, Fiber and Wood |

All seven are Carpenter recipes at Job level 1 and give Carpenter XP. The specs fix which of the four materials the Stone Axe, Stone Pickaxe, Stone Shovel and Workbench use. The Stone Shovel is a Tier 0 shovel and, like every shovel, digs only soft materials (see [World and Building](World-and-Building.md)). Exact inputs for the Hammer, Campfire and Torch, and all quantities, are set per recipe in data.

**Bootstrap path for a new character with only its starting clothes:**

1. Pick up Loose Sticks and Loose Stones by hand, and harvest Fiber from fiber plants.
2. Hand-craft a Stone Axe, a Stone Pickaxe and a Stone Shovel.
3. Gather Wood and Stone with the axe and pickaxe, and dig soft ground (such as Soil) with the shovel.
4. Hand-craft a Workbench and a Hammer (and a Campfire and Torches).
5. At the Workbench (Carpenter): craft a Stonecutter, Alchemy Table, Loom and Tanning Rack.
6. At the Stonecutter (Mason): craft a Forge and an Enchanting Altar.

All tier-1 station recipes need only Job level 1, so a new character can reach a tier-1 station for every Job.

## Station tiers and boss gating

- Stations have tiers 1–5. A recipe needs the station at or above its tier.
- To upgrade, place upgrade attachments within 5 m (starting value, tunable) of the station. Each attachment adds one tier, in order.
- An attachment binds to the nearest station of its matching type in range. Tier = 1 + the number of consecutive bound attachments starting from the tier-2 attachment.
- Attachments are crafted by the station's own Job (for example Blacksmith for Forge attachments), from materials gated by boss progression.
- Removing an attachment recomputes the tier. Removing a station leaves its attachments in place. They unbind and rebind to the nearest other matching station in range, if any.

| Upgrade tier | Boss materials that unlock it |
|--------------|-------------------------------|
| Tier 2 | Temperate boss material |
| Tier 3 | Swamp boss material, Savanna boss material |
| Tier 4 | Desert boss material, Jungle boss material |
| Tier 5 | Tundra boss material, Snowy Mountains boss material |

Each boss material is a distinct item. Each recipe names the specific boss material it needs. See [World and Building](World-and-Building.md) for the regions.

## Crafted gear quality

Equipment outputs (weapons, armor, tools, clothing, capes) roll rarity and affixes like dropped loot (see [Combat and Loot](Combat-and-Loot.md)), with two crafting rules:

- **Rarity floor:** higher Job level raises the minimum rarity. Example from the spec: Job 50+ never crafts Common.
- **Item level** = recipe tier × 10 + floor(Job level / 10).
- **Jewelry exception:** the Blacksmith's Plain Ring and Plain Amulet skip the rarity roll. They are always Common with 0 affixes, as bases for an Enchanter's add rune. Their item level still uses the formula.

| Recipe tier | Item level at Job 1 | Item level at Job 100 |
|-------------|---------------------|-----------------------|
| 1 | 10 | 20 |
| 2 | 20 | 30 |
| 3 | 30 | 40 |
| 4 | 40 | 50 |
| 5 | 50 | 60 |

Consumables (potions, meals, arrows, poisons, weapon oils) do not roll affixes. The potency of potions, meals and arrows scales with Job level. Poisons and weapon oils have no potency: their coating buildup and hit count are fixed per item and don't change with Job level. For arrows (a Carpenter recipe), the crafter's Carpenter level raises the arrow's ArrowDamage. See [Combat and Loot](Combat-and-Loot.md).

Arrows, potions and meals with no crafter (looted, bought from a Vendor, or found) have the potency of a Job level 1 craft. Consumables with different potency don't stack together: they show as separate inventory rows (see [Inventory](Inventory.md)).

## Enchanter runes

Runes are applied at an Enchanting Altar. Each rune is either an add rune or a reroll rune:

- **Add** one affix to an item that has fewer affixes than its rarity's maximum. The affix type is random from the slot's pool, excluding types already on the item. Exception: an add rune also works on a Common Plain Ring or Plain Amulet with 0 affixes, and the new affix makes it Magic.
- **Reroll** one chosen affix. It becomes a different random type from the slot's pool, excluding every type already on the item (including the one being rerolled).

Both roll the new affix's value. A rune's tier is the tier of the Enchanting Altar it was crafted at. The tier is saved on the rune, and runes with different tiers show as separate inventory rows. The value rolls as if the item level were the lower of the item's own level and the rune's max item level:

| Rune tier | 1 | 2 | 3 | 4 | 5 |
|-----------|---|---|---|---|---|
| Max item level | 20 | 30 | 40 | 50 | 60 |

(starting values, tunable)

**Applying a rune:**

- Interact with an Enchanting Altar and open its enchanting screen. Pick an item (equipped or not), a rune, and for a reroll rune the affix to reroll.
- Applying uses up the rune. It needs no Enchanter level, gives no Job XP, and works at an altar of any tier: only the rune's own tier limits the value.
- The server checks that you're in range, that you still hold the item and the rune, that the rune type matches the action, that an add rune's item is below its affix cap (or is a Common Plain Ring or Plain Amulet), and that the item can carry affixes at all. If anything fails, nothing is used up.
- If the item or the rune leaves your inventory while the screen is open, applying fails and nothing changes.
- The screen closes without applying if you're Downed, take damage, walk away, or the altar is destroyed, just like the Dye Station. Several players can use one altar at once.

## Repair

Equipment loses durability with use. At 0 it is unusable but not destroyed. Jewelry has no durability and never needs repair.

- **Station repair:** at the station of the Job that owns the item's recipe (see Which recipe an item came from, below), restore full durability for 25% (starting value, tunable) of that recipe's materials, rounded up.
- **Repair Kit:** a Blacksmith recipe (Job level 1). Repairs any item to full at any crafting station, with no Job level requirement, using one kit.
- An item with no recipe at all can only be repaired with a Repair Kit.

## Which recipe an item came from

- Every crafted item remembers the recipe row that made it, including hand-crafted items. This is saved on the item and stays with it when it's dropped, stored, picked up by someone else, or placed as a building piece.
- That recipe decides what you get back when you deconstruct a placed piece, what a destroyed piece drops, and what a station repair costs. For example, a Workbench made from Wood refunds Wood, and one made from Stick, Stone and Fiber refunds those.
- Items nobody crafted (loot, Vendor stock, quest rewards, starting clothes) use the first recipe listed for that item.
- If no recipe exists for the item at all, deconstructing or destroying it gives nothing back, and only a Repair Kit repairs it.
- Two copies of the same item made by different recipes (or one crafted and one not) show as separate inventory rows.


## Dyes

### Dye items

- There are 48 dye colors. Each is a dye item crafted by the Alchemist at an Alchemy Table from plants and minerals.
- Dyes don't roll rarity or affixes, and Job level doesn't change them.

### Dye Station

- A Carpenter build piece crafted at a Workbench (Job level 1), placed with the Hammer.
- Using it needs no Job level and gives no Job XP.
- It has no tiers or attachments, isn't a crafting station, and doesn't count as a base anchor for raids (see [Raids](Raids.md)).

### Dye zones

Armor, clothing, cloaks, bags, shields and weapons each have 1–4 dye zones: **Primary, Secondary, Accent, Trim**. Jewelry has none. Every dye zone works on every race and sex. A zone's color is a material parameter set on the item as rendered, so dyeing never rebuilds the character's appearance.

### Dyeing an item

1. Interact with a Dye Station. The dye screen opens in your viewport.
2. Pick an item from your inventory (equipped or not), one of its zones, and a color.
3. A preview of your character wearing or holding the item shows the colors. Only you see the preview.
4. **Apply** uses 1 dye of that color per zone. **Clear zone** is free and restores the default color.

- Applying over a different color replaces it. The old dye is not refunded.
- Dye colors are saved on the item, show on your character for every player right away, and stay when the item is dropped or passed to another player.
- The screen closes without applying if you're Downed, take damage, walk away, or the Dye Station is destroyed. Several players can use one Dye Station at once.

## Source spec

- [Crafting Jobs](../../specs/crafting-jobs/crafting-jobs.md)
- [Character Progression](../../specs/character-progression/character-progression.md) (which Job crafts each weapon category)
- [Inventory](../../specs/inventory/inventory.md) (capes and jewelry)
- [Combat and Loot](../../specs/combat-loot/combat-loot.md) (weapon coatings and repair, Requirements 35 and 61)
- [Voxel World](../../specs/voxel-world/voxel-world.md) (deconstruct refunds and destroyed-piece drops, Requirements 26 and 27)
