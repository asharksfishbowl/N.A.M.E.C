# Spec: Crafting Jobs

## Overview
Crafting is organized into nine Jobs that any character can level, independent of class. Jobs level by crafting, unlock recipes and station tiers as they grow, and raise the quality and random-stat rolls of crafted gear. Shared terms are defined in `specs/game-foundation/game-foundation.md`.

## Goals
- Any character can pursue any Job, so a co-op group naturally splits crafting roles by preference.
- Higher Job levels produce noticeably better gear, so crafted items compete with dropped loot.
- Crafting ties into gathering, survival, building, and boss progression.

## Non-Goals
- Player-to-player trading UI (items and gold can be dropped on the ground; see combat-loot spec Requirement 34).
- NPC vendors selling boss materials. Vendors and their stock are defined in `specs/factions-kingdoms/factions-kingdoms.md` Requirements 34 and 35.
- Automation (conveyor belts, auto-smelters).

## Requirements
1. Nine Jobs exist, each a row in `DT_Crafting_Jobs`:
   - **Blacksmith** — metal weapons (the One-Handed Sword, One-Handed Axe, One-Handed Mace, Dagger, Spear, Two-Handed Sword, Two-Handed Axe, and Two-Handed Hammer weapon categories), metal tools (axes, pickaxes, shovels), Repair Kits, and jewelry bases (Plain Ring and Plain Amulet, crafted at Common rarity with 0 affixes; Requirement 10).
   - **Armorsmith** — metal armor (Metal armor category) and shields (Shield weapon category).
   - **Carpenter** — wooden building pieces, furniture (including beds), storage containers (chests, barrels, crates), training dummies, torches, bows (Bow weapon category), staves (Staff weapon category), arrows, wooden tools (including Hammers and Fishing Rods), stone tools (Stone Axe, Stone Pickaxe, Stone Shovel), Workbenches, Campfires, Stonecutters, Alchemy Tables, Looms, Tanning Racks, Dye Stations (Requirement 19).
   - **Mason** — stone building pieces, Forges, Enchanting Altars, stone fortifications.
   - **Alchemist** — potions (healing, mana, resistance), poisons, weapon oils, dyes (Requirement 18).
   - **Cook** — meals that restore hunger and thirst and grant timed buffs.
   - **Tailor** — cloth armor and cold-weather and hot-weather clothing (all Cloth armor category), cloth capes (Cloak slot, no armor category), bags.
   - **Leatherworker** — leather armor (Leather armor category), leather cloaks (Cloak slot, no armor category), waterskins.
   
   Armor categories and the skills they train are defined in `specs/character-progression/character-progression.md` Requirement 27, and weapon categories, their skills, and the Job that crafts each in `specs/character-progression/character-progression.md` Requirement 29. Capes and jewelry are defined in `specs/inventory/inventory.md` Requirements 25 and 26.
   - **Enchanter** — runes that add or reroll affixes on existing gear.
2. Every character can level every Job. Job levels range 1–100 (tuning value). Required XP comes from `DT_Crafting_JobXPCurve`.
3. Job XP is awarded on each successful craft: `recipeBaseXP × tierMultiplier`, both from the recipe's row in `DT_Crafting_Recipes`. A recipe more than 20 levels below the crafter's Job level awards 10% XP (tuning values).
4. Job XP does **not** feed character XP. Jobs are a fully separate track.
5. Each recipe is a row in `DT_Crafting_Recipes` with: owning Job, required Job level, required station and station tier (the recipe tier), input items and quantities, output item and quantity, craft time in seconds, base XP, and TierMultiplier.
6. Recipes are known automatically once the crafter's Job level meets the requirement. No recipe scrolls are needed.
7. Crafting happens at **stations**, which are placeable world objects. Each station serves one or more Jobs:
   - Workbench — Carpenter.
   - Forge (one station that includes its anvil) — Blacksmith, Armorsmith.
   - Stonecutter — Mason.
   - Alchemy Table — Alchemist.
   - Cooking Fire — Cook. The Campfire is the tier-1 Cooking Fire station.
   - Loom — Tailor.
   - Tanning Rack — Leatherworker.
   - Enchanting Altar — Enchanter.
8. Stations have tiers 1–5. Upgrading a station requires placing upgrade attachments within 5 m of the station (tuning value), each attachment adding one tier in order. An attachment binds to the nearest station of its matching station type within that range. Attachments are crafted by the station's own Job (Blacksmith for Forge attachments) from materials gated by boss progression (see `specs/combat-loot/combat-loot.md` boss requirements). A station's tier is 1 plus the number of consecutive bound attachments starting from the tier-2 attachment. When an attachment is deconstructed or destroyed, the station's tier recomputes. When a station is deconstructed or destroyed, its attachments stay placed, unbind, and each rebinds to the nearest other station of the matching type within range, if one exists. A recipe requires the station to be at or above its tier.
9. A station pulls input materials from the crafter's inventory and from any storage container within 10 meters (tuning value). All containers in a world are accessible to every player in the session. There are no container locks or ownership.
10. Equipment outputs (weapons, armor, tools, clothing, capes) roll rarity and affixes using the loot system (see combat-loot spec), with these crafting rules:
    - Crafter Job level raises the rarity roll floor per `DT_Crafting_QualityByLevel` (e.g., Job 50+ never crafts Common).
    - The crafted item's item level = recipe tier × 10 + floor(Job level / 10).
    - Exception: crafted jewelry (the Blacksmith's Plain Ring and Plain Amulet recipes) does not use the crafting rarity roll. It is always crafted at Common rarity with 0 affixes, as a base for the Enchanter's add-affix rune (Requirement 12), and its item level still follows the formula above. Jewelry is defined in `specs/inventory/inventory.md` Requirement 26.
11. Consumable outputs (potions, meals, arrows) do not roll affixes. Their potency scales with Job level per `DT_Crafting_ConsumablePotency`. For arrows, a Carpenter recipe, the crafter's Carpenter Job-level potency scales the arrow's `ArrowDamage` (`specs/combat-loot/combat-loot.md` Requirement 53). A consumable's potency value is stored on its item instance. Arrows, potions, and meals that have no crafter (looted from enemies or loot chests, bought from Vendors, or received as quest rewards) have the potency of a Job level 1 craft (Common potency) from `DT_Crafting_ConsumablePotency`. Consumables of the same item definition with different potency values are separate inventory rows and never stack together (`specs/inventory/inventory.md` Edge Case 11).
12. Enchanter runes applied at the Enchanting Altar can: add one affix to an item with fewer than its rarity's max affix count, or reroll one chosen affix. Exception: an add rune can also be applied to Common jewelry with 0 affixes (the Plain Ring and Plain Amulet bases, Requirement 10), and the added affix makes that item Magic rarity, so later adds follow the Magic max affix count. An add rolls the affix type at random from the item slot's affix pool in `DT_Loot_Affixes`, excluding affix types already on the item. A reroll replaces the chosen affix with a different affix type, rolled at random from the item slot's affix pool excluding every affix type already on the item (including the chosen one). Both add and reroll roll the new affix's value. A rune's tier is the tier of the Enchanting Altar the rune was crafted at, stored on the rune item instance and saved with it. Runes of the same item definition with different tiers are separate inventory rows (`specs/inventory/inventory.md` Edge Case 12). The new affix's value rolls from the `DT_Loot_Affixes` value range for item level `min(item's item level, RuneMaxItemLevel)`, where RuneMaxItemLevel is the max item level for the rune's tier (starting values 20, 30, 40, 50, 60 for rune tiers 1–5, matching the highest crafted item level at each recipe tier per Requirement 10; tuning values).
13. Crafting takes the recipe's craft time. The player may queue up to 10 crafts at a station (tuning value). Crafting continues while the player is within 10 meters of the station, and pauses when they leave.
14. Each Job gains milestone perks at levels 25, 50, 75, and 100, defined in `DT_Crafting_JobPerks` (e.g., Cook 50: meals last 25% longer).
15. Every value marked "tuning value" in this spec that names no other table lives in `DT_Crafting_Rules`.
16. A hand-crafting menu, opened from the inventory screen, crafts recipes whose required station is "None" without any station. The hand-crafting recipes are Workbench, Hammer, Campfire, Torch, Stone Axe, Stone Pickaxe, and Stone Shovel. Each is a Carpenter recipe at required Job level 1 and awards Carpenter Job XP. Hand-crafting pulls materials from the crafter's inventory only. Every hand-crafting recipe's inputs are drawn only from Stick, Stone, Fiber, and Wood, with quantities in `DT_Crafting_Recipes`:
    - Stone Axe, Stone Pickaxe, and Stone Shovel — Stick, Stone, and Fiber. The Stone Shovel is a Tier 0 shovel and, like every shovel, digs only soft materials (`specs/voxel-world/voxel-world.md` Requirement 12).
    - Workbench — two recipe rows: one from Stick, Stone, and Fiber, and one from Wood (gathered with the Stone Axe).
    
    Stick and Stone come from Loose Stick and Loose Stone pickups collected by hand, and Fiber comes from fiber plant forage nodes (`specs/voxel-world/voxel-world.md` Requirements 22 and 30), so a character with only the starting clothes (`specs/character-creation/character-creation.md` Requirement 28) and no tools can reach every hand-crafting recipe.
17. Tier-1 station recipes, all at required Job level 1, so a new character can reach a tier-1 station for every Job:
    - Workbench and Campfire — Carpenter, hand-crafted (Requirement 16).
    - Stonecutter, Alchemy Table, Loom, Tanning Rack — Carpenter, at a Workbench.
    - Forge and Enchanting Altar — Mason, at a Stonecutter.

### Dyes
18. 48 dye colors exist, each a row in `DT_Crafting_DyeColors` with: color ID, display name, linear color value, and dye item definition. Each color has one dye item. Dye items are crafted by the Alchemist at an Alchemy Table from plant and mineral inputs, with required Job level, station tier, inputs, and quantities in `DT_Crafting_Recipes`. Dye items roll no rarity and no affixes, and their effect does not scale with Job level (Requirement 11 does not apply).
19. The Dye Station is a Carpenter build piece (required Job level 1, crafted at a Workbench) placed with the Hammer (`specs/voxel-world/voxel-world.md` Requirement 24). Using a Dye Station needs no Job level and awards no Job XP. The Dye Station has no tiers, no upgrade attachments, and no recipes, and it is not a crafting station for Requirements 7–9 or for base anchors (`specs/factions-kingdoms/factions-kingdoms.md` Requirement 51).
20. Every armor, clothing, cloak, bag, shield, and weapon item definition defines 1–4 dye zones, chosen from Primary, Secondary, Accent, and Trim, each mapped to a channel of the item's material mask. A zone's color is applied as a Substrate material parameter per `specs/engine-tech/engine-tech.md` Requirement 3, set at runtime on the rendered item: on the character meshes Mutable builds for worn items, and on the held mesh for weapons and shields. Dye colors are not Mutable inputs, so applying or clearing a dye changes only the material parameter and never rebuilds the character through Mutable. Items of other types define no dye zones. Jewelry (Neck and Ring slots) has 0 dye zones (`specs/inventory/inventory.md` Requirement 26).
21. Interacting with a Dye Station opens the dye screen in the owning local player's viewport. The player picks a dyeable item from the inventory (equipped or not), one of that item's dye zones, and one of the 48 colors. A 3D preview of the player's character wearing or holding the item shows the chosen colors before applying. The preview renders only in the owning local player's viewport and does not replicate. Apply consumes 1 dye item of that color per zone from the player's inventory and sets the zone's color. Apply is disabled when the player has no dye item of that color or the zone already has that color. "Clear zone" is free and restores the zone's default color.
22. Dye colors are stored per item instance (`specs/inventory/inventory.md` Requirement 23). An applied dye takes effect immediately and replicates with equipped item visuals to every client.
23. The world does not pause while a player uses a Dye Station. The dye screen closes without applying the pending selection when the player becomes Downed, takes damage, moves out of the Dye Station's interaction range (tuning value), or the Dye Station is destroyed or deconstructed. Two or more players can use the same Dye Station at the same time, each with an independent dye screen.

## Data Flow
1. Player interacts with a station → station UI opens, listing recipes for the Jobs that station serves, filtered to recipes the player's Job level unlocks and the station tier supports.
2. Player selects a recipe and quantity → client sends `ServerQueueCraft(StationId, RecipeId, Quantity)`.
3. Server validates Job level, station tier, and material availability (inventory + containers within range), then reserves and removes materials for the first craft.
4. Server runs the craft timer. On completion, it generates the output (rolling rarity and affixes for equipment other than crafted jewelry, Requirement 10, and setting the crafter's Job-level potency on consumables, Requirement 11), places the output in the crafter's inventory (inventory has no hard cap; see `specs/inventory/inventory.md`), and awards Job XP via `UNamecJobComponent::AwardJobXP`.
5. The next queued craft starts, repeating steps 3–4, until the queue is empty or materials run out.
6. Job levels and XP save to `UNamecCharacterSave`. Station placement and tier save to `UNamecWorldSave`.
7. Player opens the hand-crafting menu from the inventory screen → client sends `ServerQueueCraft` with no station → server validates Job level and materials in the crafter's inventory, then runs steps 4–5.
8. Player interacts with a Dye Station → the dye screen opens with the character preview → the player picks an item, a zone, and a color, and the preview updates locally → client sends `ServerApplyDye(DyeStationId, ItemInstanceId, Zone, ColorId)` or `ServerClearDyeZone(DyeStationId, ItemInstanceId, Zone)` → server validates range, item ownership, that the item defines the zone, and (for Apply) that the player holds a dye item of that color → server removes 1 dye item, sets the item instance's zone color, and replicates equipped item visuals to every client.

## Edge Cases
1. When materials run out mid-queue, the remaining queued crafts are cancelled and the player sees "Not enough materials". Completed crafts are kept.
2. When the player leaves the 10 m range mid-craft, the current craft timer pauses and resumes when they return. Materials already consumed for that craft stay reserved.
3. When the player disconnects or quits mid-craft, the current craft is cancelled and its reserved materials return to the player's inventory before the character saves.
4. When two players use the same station at once, each has an independent queue. Materials pulled from shared containers are reserved at the start of each craft, first come, first served.
5. When a station is destroyed or deconstructed with a Hammer while crafts are queued, all queues on that station are cancelled and reserved materials return to their crafters' inventories.
6. When a nearby container's contents change during material validation, the server re-validates at the moment the craft starts. The server is authoritative.
7. When a player applies a dye to a zone that already has a different dye color, the new color replaces the old one, and the old dye item is not refunded.
8. When a player's last dye item of the chosen color leaves the inventory while the dye screen is open, the server rejects Apply and the item's colors are unchanged.
9. When an item instance stores a color ID that no longer exists in `DT_Crafting_DyeColors` (content changed), that zone shows its default color and the item still loads.
10. When a player dyes an equipped item, every client sees the new color on that player's character without the item being re-equipped.

## Acceptance Criteria
- [ ] A new character with only the starting clothes collects Loose Sticks, Loose Stones, and Fiber by hand, hand-crafts a Stone Axe, Stone Pickaxe, and Stone Shovel, gathers Wood and Stone with the axe and pickaxe, digs Soil with the shovel, and hand-crafts a Workbench and Hammer without any station.
- [ ] A Carpenter 1 character hand-crafts a Stone Shovel from Stick, Stone, and Fiber, and the Stone Shovel digs Soil but cannot dig Stone.
- [ ] Starting from that Workbench, the same character obtains a tier-1 station for every Job and crafts a Level-1 recipe for all nine Jobs at the matching tier-1 stations.
- [ ] Hand-crafting a Torch awards Carpenter Job XP.
- [ ] Crafting a recipe awards Job XP and does not change character XP.
- [ ] A recipe requiring Blacksmith 30 does not appear for a Blacksmith 29 character.
- [ ] A tier-3 recipe cannot be crafted at a tier-2 Forge.
- [ ] A Blacksmith 50 crafting 100 swords produces 0 Common-rarity swords.
- [ ] A station uses materials from a chest 8 m away and ignores a chest 12 m away.
- [ ] Quitting mid-craft returns the reserved materials to inventory.
- [ ] An Enchanter rune adds an affix to a Magic item below its affix cap.
- [ ] A Blacksmith crafting 100 Plain Rings at any Job level produces 100 Common rings with 0 affixes, and an add rune applied to one of them gives it 1 affix and makes it Magic.
- [ ] A looted or Vendor-bought healing potion has Job level 1 potency, and it lists in a separate inventory row from a healing potion crafted at a higher Alchemist level.
- [ ] An Alchemist crafts a dye item at an Alchemy Table, and a Carpenter 1 crafts a Dye Station at a Workbench.
- [ ] Applying a color to the Primary and Trim zones of a chest armor piece consumes 2 dye items of that color, the preview shows the colors before applying only in that player's viewport, and every client sees the result.
- [ ] Using a Dye Station awards no Job XP and requires no Job level.
- [ ] "Clear zone" restores the default color and consumes nothing.
- [ ] Dye colors survive dropping the item, another player picking it up, and save → quit → load.
- [ ] Rerolling an affix replaces it with a different affix type that was not already on the item, and a rune crafted at a tier-2 Enchanting Altar applied to an item-level-50 item never rolls a value above the `DT_Loot_Affixes` range for the tier-2 RuneMaxItemLevel.

## Key Files
- `Source/NAMEC/Crafting/NamecJobComponent.h` — new; per-character Job XP, levels, perks.
- `Source/NAMEC/Crafting/NamecCraftingStation.h` — new; station actor (subclass of `ANamecBuildPiece`), attachment binding and tier, craft queues, material reservation.
- `Source/NAMEC/Crafting/NamecRecipeTypes.h` — new; recipe and Job DataTable row structs.
- `Source/NAMEC/Crafting/NamecEnchantingService.h` — new; affix add/reroll logic, including adding an affix to Common jewelry and raising it to Magic (Requirement 12).
- `Source/NAMEC/UI/Crafting/` — new; station UI, queue display, hand-crafting menu, dye screen with character preview.
- `Source/NAMEC/Crafting/NamecDyeTypes.h` — new; dye zone enum (Primary, Secondary, Accent, Trim) and the `DT_Crafting_DyeColors` row struct.
- `Source/NAMEC/World/Building/NamecDyeStationPiece.h` — new; Dye Station build piece that opens the dye screen and validates `ServerApplyDye` and `ServerClearDyeZone`.
- `Content/Data/DT_Crafting_DyeColors.uasset` — new; 48 dye color rows (color ID, display name, linear color, dye item definition).
- `Content/Data/DT_Crafting_Jobs.uasset` — new; 9 Job rows.
- `Content/Data/DT_Crafting_Recipes.uasset` — new; all recipes.
- `Content/Data/DT_Crafting_JobXPCurve.uasset` — new; XP per Job level.
- `Content/Data/DT_Crafting_QualityByLevel.uasset` — new; rarity floors by Job level.
- `Content/Data/DT_Crafting_ConsumablePotency.uasset` — new; potency scaling, including Carpenter Job-level scaling of arrow `ArrowDamage` and the Job level 1 potency used by consumables with no crafter (Requirement 11).
- `Content/Data/DT_Crafting_JobPerks.uasset` — new; milestone perks.
- `Content/Data/DT_Crafting_Rules.uasset` — new; station ranges, queue size, low-level XP penalty, RuneMaxItemLevel per rune tier, Dye Station interaction range, and other crafting tuning values.
