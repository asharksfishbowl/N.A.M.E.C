# Factions and Kingdoms

This page covers the eight factions, reputation and its tiers, the six kingdoms and their home regions, towns and their protected areas, town NPCs, Bandit and Beastmen camps, gold and vendors, and quests.

← [Home](Home.md)

The world holds six race kingdoms with towns, vendors and quest boards, plus two always-hostile factions who live in camps. Your character earns and loses standing with each faction, earns and spends gold, and takes quests. Raids on player bases are covered on [Raids](Raids.md).

## Factions

| Faction | Type | Notes |
|---------|------|-------|
| Human kingdom | Kingdom | Home region: Temperate Forest & Meadows |
| Sauren kingdom | Kingdom | Home region: Swamp |
| Hundari kingdom | Kingdom | Home region: Savanna |
| Felari kingdom | Kingdom | Home region: Desert |
| Vanari kingdom | Kingdom | Home region: Jungle |
| Ursan kingdom | Kingdom | Home region: Tundra |
| Bandits | Always hostile | Live in camps, can raid bases |
| Beastmen | Always hostile | Live in camps, can raid bases |

- Kingdom display names are placeholders for now.
- Region monsters (the normal enemies of each region) belong to no faction and have no reputation.
- Bandits, Beastmen and kingdom raiders count as Hostile enemies for every combat rule: XP, loot, gold, executions, perception. See [Combat and Loot](Combat-and-Loot.md).

## Reputation

Each character has a reputation value with every faction, from −100 to +100. It is saved with the character, so it is the same in every world.

| Tier | Reputation | Effect with that kingdom |
|------|-----------|--------------------------|
| Hostile | −100 to −50 | Guards attack you on sight. Vendors, Quest Givers and Quest Boards refuse service. The kingdom can raid bases. Standing recovers daily, and the Guard Captain accepts a fine. |
| Unfriendly | −49 to −10 | Vendor prices +25% |
| Neutral | −9 to +9 | Normal prices |
| Friendly | +10 to +49 | Vendor prices −10% |
| Honored | +50 to +100 | Vendor prices −20%, and the kingdom's questline finale unlocks |

(price changes are starting values, tunable)

**Starting values:**

- +30 with your own race's kingdom.
- 0 with the other five kingdoms.
- −100 with Bandits and Beastmen. This never changes.

**Gaining reputation:** through quest rewards, and, while Hostile, through daily recovery and paying a Guard Captain's fine (see [Leaving the Hostile tier](#leaving-the-hostile-tier)).

**Losing reputation** (starting values, tunable):

- Damaging a kingdom's Guard, Vendor, Quest Giver or Citizen (including with a tool swing): −10. This applies once per NPC, and again only after that NPC has gone a while without taking damage from you.
- Killing a Guard or Citizen: a further −25.
- Area attacks count too, and the friendly-fire setting does not protect NPCs.

The Quests tab shows every faction's reputation value and tier.

### Leaving the Hostile tier

- **Daily recovery:** every day at 06:00 in-game, while you're in the world, your standing with each kingdom you're Hostile with rises by 2 (starting value, tunable). It stops at −49 (Unfriendly) and never goes higher this way. Days skipped by sleeping count. Bandits and Beastmen stay at −100.
- **Paying a fine:** each capital has a **Guard Captain**. Interact with them to open the fine screen. The fine is 500 gold + 10 gold per point below −49 (starting values, tunable). For example, at −60 the fine is 610 gold. Paying sets your standing to −49 at once. If you don't have enough gold, you can't pay.
- Guards still attack Hostile players on the way to the Guard Captain. After you pay, Guards within 20 m of the Guard Captain leave you alone for 60 seconds (starting values, tunable), unless you hit one of them.

## Kingdoms and towns

- Each kingdom has **one capital and 2 towns**, placed by world generation in its home region. The same seed always places them in the same spots.
- Snowy Mountains and Volcanic have no kingdom and no towns.
- Every town has exactly one Quest Board and at least one Vendor. Every capital has exactly one Quest Giver, who offers the kingdom's questline, and exactly one Guard Captain. Every other town has at least one Quest Giver.
- Town buildings are part of the generated town. They can't be damaged or deconstructed.

### Protected area

Each town has a protected radius (tunable), measured flat from the town center, so it covers every depth and height. Inside it:

- terrain can't be dug, filled or mined
- you can't place building pieces
- enemies don't spawn

## Town NPCs

| Role | What it does | Can be killed? |
|------|--------------|----------------|
| Guard | Attacks Hostile-tier players, raiders, and enemies inside the town. Fights back against anyone who hits it. | Yes, respawns after 1 in-game day |
| Vendor | Buys and sells items | No. It cowers when hit. |
| Quest Giver | Turns in quests and offers the kingdom's questline (capital) | No. It cowers when hit. |
| Citizen | Flees when hit | Yes, respawns after 1 in-game day |
| Guard Captain (capitals only) | Takes fines from Hostile-tier players | No. It cowers when hit. |

(respawn time is a starting value, tunable)

- NPCs stand at fixed posts in their town. They have no daily schedules.
- NPCs fight with the same combat system as enemies, and enemies attack NPCs too. Guards use their own attack values, and NPC health and damage don't scale with the number of players.
- Guards don't chase anyone beyond the town's protected radius.
- Killing or hitting NPCs gives no XP, loot or gold.
- Talk to an NPC or use a Quest Board with Interact. Each screen opens in your own viewport and does not pause the world.

## Camps

- Bandit camps and Beastmen camps generate in every region (density set per region per faction).
- Each camp has a camp radius (tunable). You can't build inside it, but you can still dig, fill and mine there.
- Each camp has enemy spawn points and loot chests. Camp chests follow the normal chest rules: each player opens each chest once, and it never refills.
- A camp is **cleared** when every enemy spawned there is dead. It respawns its enemies 1 in-game day later (starting value, tunable). Every spawn point always refills, even with players or building pieces nearby.

## Gold

- Gold is a per-character counter, not an item. It weighs nothing and shows in the inventory footer.
- It is saved with your character and is **not lost on death**.
- **Sources:** Hostile, Bandit, Beastmen and raider kills (rolled per player, like other loot), loot chests, quest rewards, and selling to Vendors. Wildlife and bosses drop no gold.
- **Sharing:** use Drop Gold in the inventory footer to drop an amount on the ground. Anyone can pick it up.

## Vendors

Every item has a **Value** in gold.

| Deal | Price per unit |
|------|----------------|
| Vendor buys from you | Value × 25% × rarity multiplier, rounded down |
| Vendor sells to you | Value × (1 − CHA modifier × 3%) × reputation tier multiplier, rounded, minimum 1 |

| Rarity | Common | Magic | Rare | Epic | Legendary |
|--------|--------|-------|------|------|-----------|
| Sell multiplier | ×1 | ×1.5 | ×2.5 | ×4 | ×8 |

(25%, 3% and the rarity multipliers are starting values, tunable)

Example: with CHA modifier +2 and Honored tier, an item worth 100 costs 100 × 0.94 × 0.80 = 75 gold. It sells for 25 gold if Common, 62 gold if Rare and 200 gold if Legendary.

- Items without a rarity (consumables, materials, dyes) sell at the Common multiplier.
- Affixes, durability, CHA and reputation don't change the sell price.
- The rarity multiplier doesn't apply when you buy. Vendor equipment is always Common, so it would make no difference anyway.
- Items worth 0, quest items and equipped items can't be sold. Items you sell don't go into the Vendor's stock.
- Each Vendor has a stock list. Stock is shared by every player in the world, restocks every day at 06:00 in-game, and is saved with the world.
- Equipment bought from a Vendor is Common with no affixes, including jewelry. Bought arrows, potions and meals have Job level 1 potency.
- Vendors never sell boss materials.
- If two players buy the last unit at once, the first request wins and the other sees "Out of stock".

## Quests

### Quest Boards

- Each town's Quest Board offers 3–5 random quests (starting values, tunable) from that kingdom's templates.
- Boards refresh every day at 06:00 in-game and are saved with the world.
- Each character can accept each offer once. The world save remembers which characters accepted each offer. The offer stays on the board for other players, and quests you've accepted stay in your log after a refresh.
- If you abandon a board quest, you can accept it again while its offer is still on the board.

### Quest types

| Type | Objective |
|------|-----------|
| Hunt | Kill N of a specific enemy |
| Gather | Deliver N of an item (the items are taken on turn-in) |
| Clear Camp | Clear any Bandit or Beastmen camp of the named faction in the named region |
| Escort | Lead an NPC to another town (capital, town 1 or town 2) of the same kingdom |
| Deliver | Carry a quest item to a Quest Giver in another town of the same kingdom |

- **Escort** fails if the NPC dies, gets too far from you, or you leave the world before it arrives. An Escort quest not yet completed when your character enters a world (for example after a crash) fails too. The escort NPC doesn't flee when hit: it keeps following you.
- When the escort NPC reaches the destination town, it leaves, and the completed quest stays in your log (even across worlds and saves) until you turn it in or abandon it.
- **Quest items** weigh nothing and can't be dropped, stored or sold.

### Credit in co-op

- **Hunt and Clear Camp:** every player with that quest active who is eligible for the kill gets credit (the same eligibility as loot, see [Combat and Loot](Combat-and-Loot.md)). Executions count.
- **Gather and Deliver:** only the player who turns in.
- **Escort:** only the player who accepted it.

### Turning in and rewards

- Board quests turn in at any Quest Giver of the kingdom that issued them. Deliver quests turn in at the destination town's Quest Giver.
- Rewards: gold, reputation with that kingdom, character XP, and sometimes an item. A reward arrow, potion or meal has Job level 1 potency.

### Kingdom questlines

- Each kingdom has a hand-made questline of 5 quests (placeholder content for now), offered in order by the capital's Quest Giver.
- The 5th quest (the finale) needs Honored standing with that kingdom.
- Questline quests turn in at the capital's Quest Giver, except Deliver steps, which turn in at the destination town's Quest Giver.
- A questline step's objective and rewards are rolled when you accept it.

### Quest log and tracker

- The quest log is the **Quests tab** of the Inventory and Character menu (gamepad: View tap, then the Quests tab; keyboard: J). See [Co-op and Controls](Co-op-and-Controls.md).
- It holds up to 10 active quests (starting value, tunable). You can track or abandon quests there.
- Up to 3 tracked quests show on your HUD, in your own viewport.
- Quest progress is saved with your character and works in any world.

## Source spec

- [Factions and Kingdoms](../../specs/factions-kingdoms/factions-kingdoms.md)
