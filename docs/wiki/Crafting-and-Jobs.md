# Crafting and Jobs

This page covers the 9 crafting Jobs, stations, hand-crafting and the fresh-character path, station tiers and boss gating, crafted gear quality, Enchanter runes, and repair.

← [Home](Home.md)

## Jobs

Every character can level every Job, independent of class. Jobs level 1–100 (starting value, tunable) by crafting. Job XP does **not** feed character XP.

| Job | Makes | Station |
|-----|-------|---------|
| Blacksmith | Metal weapons, metal tools (axes, pickaxes, shovels), Repair Kits | Forge |
| Armorsmith | Metal armor (Metal category), shields | Forge |
| Carpenter | Wooden building pieces, furniture (including beds and Mirrors, Carpenter level 5), storage containers (chests, barrels, crates), training dummies, torches, bows, arrows, wooden tools (including Hammers and Fishing Rods), stone tools (Stone Axe, Stone Pickaxe), Workbenches, Campfires, Stonecutters, Alchemy Tables, Looms, Tanning Racks | Workbench |
| Mason | Stone building pieces, Forges, Enchanting Altars, stone fortifications | Stonecutter |
| Alchemist | Potions (healing, mana, resistance), poisons, weapon oils | Alchemy Table |
| Cook | Meals that restore hunger and thirst and grant timed buffs | Cooking Fire (the Campfire is the tier-1 Cooking Fire) |
| Tailor | Cloth armor, cold-weather and hot-weather clothing (all Cloth category), bags | Loom |
| Leatherworker | Leather armor (Leather category), waterskins | Tanning Rack |
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
- There is no automation (no conveyors or auto-smelters) and no NPC vendors.

## Hand-crafting and the fresh-character path

A hand-crafting menu, opened from the inventory screen, crafts without any station. It uses materials from your own inventory only.

| Hand recipe | Inputs |
|-------------|--------|
| Stone Axe | Stick, Stone, Fiber |
| Stone Pickaxe | Stick, Stone, Fiber |
| Workbench | Stick, Stone, Fiber (one recipe) or Wood (a second recipe) |
| Hammer | drawn only from Stick, Stone, Fiber and Wood |
| Campfire | drawn only from Stick, Stone, Fiber and Wood |
| Torch | drawn only from Stick, Stone, Fiber and Wood |

All six are Carpenter recipes at Job level 1 and give Carpenter XP. The specs fix which of the four materials the Stone Axe, Stone Pickaxe and Workbench use. Exact inputs for the Hammer, Campfire and Torch, and all quantities, are set per recipe in data.

**Bootstrap path for a character with no items:**

1. Pick up Loose Sticks and Loose Stones by hand, and harvest Fiber from fiber plants.
2. Hand-craft a Stone Axe and Stone Pickaxe.
3. Gather Wood and Stone with them.
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

Equipment outputs (weapons, armor, tools, clothing) roll rarity and affixes like dropped loot (see [Combat and Loot](Combat-and-Loot.md)), with two crafting rules:

- **Rarity floor:** higher Job level raises the minimum rarity. Example from the spec: Job 50+ never crafts Common.
- **Item level** = recipe tier × 10 + floor(Job level / 10).

| Recipe tier | Item level at Job 1 | Item level at Job 100 |
|-------------|---------------------|-----------------------|
| 1 | 10 | 20 |
| 2 | 20 | 30 |
| 3 | 30 | 40 |
| 4 | 40 | 50 |
| 5 | 50 | 60 |

Consumables (potions, meals, arrows) do not roll affixes. Their potency scales with Job level.

## Enchanter runes

Runes are applied at an Enchanting Altar and do one of two things:

- **Add** one affix to an item that has fewer affixes than its rarity's maximum. The affix type is random from the slot's pool, excluding types already on the item.
- **Reroll** one chosen affix. It becomes a different random type from the slot's pool, excluding every type already on the item (including the one being rerolled).

Both roll the new affix's value. A rune's tier is the tier of the Enchanting Altar it was crafted at. The value rolls as if the item level were the lower of the item's own level and the rune's max item level:

| Rune tier | 1 | 2 | 3 | 4 | 5 |
|-----------|---|---|---|---|---|
| Max item level | 20 | 30 | 40 | 50 | 60 |

(starting values, tunable)

## Repair

Equipment loses durability with use. At 0 it is unusable but not destroyed.

- **Station repair:** at the station of the item's crafting Job, restore full durability for 25% (starting value, tunable) of the item's recipe materials, rounded up. Dropped items with no recipe use their base item type's recipe.
- **Repair Kit:** a Blacksmith recipe (Job level 1). Repairs any item to full at any crafting station, with no Job level requirement, using one kit.

## Source spec

- [Crafting Jobs](../../specs/crafting-jobs/crafting-jobs.md)
