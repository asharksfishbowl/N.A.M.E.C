# Races and Character Creation

This page covers creating a character, the six races and their traits, male and female characters, appearance options, name rules, and changing your look at a Mirror.

← [Home](Home.md)

## Where you create a character

- From the main menu's **Characters** screen.
- From **Create New** in any character select: the lobby after choosing New World or Load World, the join prompt when a player presses Start mid-game, and the character pick on a machine joining a LAN game.

Two local players can create characters at the same time, each in their own viewport. See [Co-op and Controls](Co-op-and-Controls.md).

## Creation steps

Each step is its own screen with Back and Next:

1. **Race**
2. **Sex**
3. **Appearance** (includes a Randomize button that rolls a valid look for your race and sex)
4. **Starting Class** (see [Stats and Classes](Stats-and-Classes.md))
5. **Name & Confirm**

- A 3D preview is shown on every step. You can rotate it, zoom between full body and face, and switch between three preview outfits: starting clothes, Leather armor and Metal armor.
- **Starting clothes:** every new character starts with exactly two items, a Plain Shirt (Chest) and Plain Trousers (Legs), already equipped. They're Cloth clothing with 0 Armor, 0 Insulation and 0 Cooling, have 2 dye zones, are worth 0 gold and can't be sold.
- The Confirm screen sums up your race and its traits, sex, starting class and its abilities, and your final starting stats.
- **Going back:**
  - Changing race after visiting later steps resets your appearance to that race's default look. Sex and class are kept.
  - Changing sex keeps everything, but any appearance option the new sex doesn't have resets to the default.
- **Cancelling:** pressing Back on the Race step (or cancelling) discards the character and returns you to the screen you came from.
- **Controller disconnect:** if your controller disconnects during creation, your progress is kept and "Reconnect controller" shows in your viewport.

## Starting stats

Starting stats = 10 + race bonus + starting class bonus. Both bonuses are permanent.

Example: a Human Warrior (Human +1 to all, Warrior +2 STR, +1 CON) starts with STR 13, CON 12 and every other stat 11.

## The six races

Race is chosen once and can never be changed.

| Race | Stat bonus | Passive | Active ability | Downside |
|------|-----------|---------|----------------|----------|
| **Human** | +1 to all six stats | **Versatile:** +10% skill XP from all sources | **Second Wind:** instantly restore 30% max stamina and 10% max health (120 s cooldown) | **Fleeting Vigor:** Fatigue rises 15% faster, both from time awake and from stamina use |
| **Felari** (cat folk) | +2 DEX, +1 CHA | **Night Eyes:** night vision at night and underground, and no fall damage from falls under 8 m | **Pounce:** leap up to 8 m at your locked-on target (or straight ahead), dealing unarmed damage and 60 poise damage on landing (20 s cooldown) | **Water-Averse:** swim 30% slower, swimming costs 50% more stamina, and Wet lasts twice as long |
| **Hundari** (dog folk) | +2 CON, +1 WIS | **Keen Nose:** ore, Wildlife and Hostile enemies within 30 m get an outline only you can see | **Rally Howl:** every player within 20 m gets +25% stamina regen for 20 s (60 s cooldown) | **Loud:** enemies notice you from 25% farther away |
| **Sauren** (lizard folk) | +2 CON, +1 INT | **Amphibious:** 3× Breath (90 s of air), swim 30% faster, +25% Poison resistance, and 25% less Poison buildup | **Shed Skin:** clear Poison, Bleed, Burn and Frostbite, then heal 2% max health per second for 5 s (90 s cooldown) | **Cold-Blooded:** your comfort range's lower bound is 6 °C higher |
| **Vanari** (monkey folk) | +2 DEX, +1 INT | **Climber:** climbing costs 50% less stamina and is 50% faster | **Swing Leap:** leap up to 5 m up or 10 m forward (30 s cooldown) | **Light Frame:** max equip load −15% |
| **Ursan** (bear folk) | +2 STR, +1 CON | **Thick Hide:** +10 built-in Insulation and +50 max carry weight | **Mauling Roar:** taunt every Hostile enemy and Boss within 6 m and deal 100 poise damage to each (45 s cooldown) | **Heat-Prone:** your comfort range's upper bound is 6 °C lower |

All numbers are starting values, tunable.

### How racial traits work

- **Passives and downsides** are always on and never wear off.
- **Racial abilities:**
  - Your racial ability starts in the last ability bar slot (slot 6). You can move it to another slot (it swaps with whatever is there), but you can never remove it from the bar.
  - Racial abilities cost no stamina or mana, have no stat requirement, never level up and give no class XP.
- **Night Eyes:**
  - Turns on automatically at night (20:00–06:00 in-game) and whenever you are underground (in a cave or with terrain overhead).
  - Only you see it, in your own viewport. You can turn it off in Settings (saved per local player slot).
  - Falls of 8 m or more deal normal fall damage for the full distance.
- **Pounce** uses the normal unarmed damage formula, including penalties like Weakened, and trains One-Handed.
- **Pounce and Swing Leap** stop at the last clear spot if they would hit terrain, a building piece or a boss arena barrier.
- **Keen Nose** outlines show only in your own viewport.
- **Rally Howl:**
  - Its strength and radius scale with CHA, like other party buffs.
  - Howling at a player who already has the buff refreshes its duration instead of stacking.
- **Shed Skin:**
  - Clears only Poison, Bleed, Burn and Frostbite, both the buildup and the active effect.
  - It does not remove Weakened, Wet, Salty, hunger/thirst/temperature/fatigue states, Over-Encumbered or racial downsides.
  - Its healing is a flat percentage with no WIS or skill scaling. Second Wind's healing works the same way.
- **Mauling Roar:**
  - A taunted enemy must attack you for 6 seconds, then goes back to normal targeting.
  - A Boss is taunted for 3 seconds and takes only 25% of the poise damage.
  - Stagger happens only if the enemy's Poise reaches 0.
- **Loud** stacks with crouching and Stealth: crouching and Stealth shrink the range first, then Loud increases the result by 25%. See [Combat and Loot](Combat-and-Loot.md).

See [Survival](Survival.md) for Breath, swimming, climbing and comfort ranges, and [Inventory](Inventory.md) for carry weight and equip load.

## Male and female

You choose Male or Female. This picks only the body, voice and animations. Stats, abilities, carry weight, collision size and every other gameplay value are identical. Sex cannot be changed after creation.

## Appearance

Appearance is picked from presets and a few sliders, so it works well with a gamepad in split-screen. There are no freeform face sliders.

| Race | Options |
|------|---------|
| All races | Body type (3), height slider (±8%), build slider (slim to heavy), face (8 per race and sex), eye color (12), voice (3 per race and sex) |
| Human | Skin tone (16), hair style (12), hair color (16), facial hair (8, available to both sexes), scars and markings (8) |
| Felari, Hundari, Ursan | Fur color (12), fur pattern (8), ear variant (4), tail variant (4), mane or head fur style (8) |
| Sauren | Scale color (12), scale pattern (8), frill or crest variant (6), tail variant (4) |
| Vanari | Fur color (12), face skin tone (8), fur pattern (8), tail variant (4), head fur style (8) |

Numbers are preset counts (starting values).

- Height and build are cosmetic only. They never change your collision size, reach or movement speed.
- Every wearable item (armor, clothing, bags, cloaks) fits every race and sex, and every dye zone on it works on every race and sex (see [Crafting and Jobs](Crafting-and-Jobs.md)).
- Helmets can hide ears, frills, crests, manes and hair (they reappear when the helmet comes off). Chest armor, leg armor, cloaks and bags can hide tails.
- First-person arms match your race, sex and gloves.
- If a game update removes an appearance option you used, that option resets to the race default and your character still loads.

## Name rules

- 1–24 characters.
- Allowed: letters (including accented letters), digits, spaces, apostrophes and hyphens. No emoji or symbols. No leading or trailing space.
- Names don't have to be unique.
- The name cannot be changed after creation.
- With a gamepad, you type on an on-screen keyboard inside your own viewport.

## The Mirror

- A Carpenter build piece (Carpenter level 5, crafted at a Workbench), placed with the Hammer. See [World and Building](World-and-Building.md).
- Using a Mirror opens the same Appearance editor as creation. Race, sex, name and starting class cannot be changed.
- Changing your look is free, with no cooldown. Confirming applies it right away for every player, and it is saved with your character.
- The world keeps running while you use a Mirror, and you are not invulnerable. The editor closes without applying changes if:
  - you take damage
  - you are Downed
  - you move out of range
  - the Mirror is destroyed or deconstructed
- Several players can use the same Mirror at once.

## Not in the game

- Backgrounds or origin stories.
- Gameplay differences between male and female.
- Changing race, sex or name after creation.
- Mixed-race characters.
- Racial ability leveling.

## Source spec

- [Character Creation](../../specs/character-creation/character-creation.md)
