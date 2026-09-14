# Survival

This page covers hunger, thirst and water types, temperature and comfort ranges, shelter, stamina, fatigue, sleep, and swimming and breath.

← [Home](Home.md)

## Meters at a glance

| Meter | Range | Notes |
|-------|-------|-------|
| Hunger | 0–100 | Starts at 100, drains |
| Thirst | 0–100 | Starts at 100, drains |
| Body temperature | °C | Comfortable band 35.5–37.5 °C |
| Fatigue | 0–100 | Rises while awake |
| Stamina | 0–max stamina | Combat resource |
| Breath | 30 s of air | Only underwater, not saved |

- Meters update once per second (starting value, tunable).
- Meters pause while you are Downed.
- Meters are saved with your character, so you join any world with the values you left with.
- There is no disease beyond the Poison status effect, no sanity meter and no food spoilage.

## HUD

- Health, Stamina and Mana bars are always shown.
- Breath shows only while your head is underwater.
- Hunger, Thirst, Temperature and Fatigue icons appear only past a warning threshold:

| Icon | Shows when |
|------|-----------|
| Hunger | below 30 |
| Thirst | below 30 |
| Temperature | outside 35.5–37.5 °C |
| Fatigue | above 60 |

## Hunger and thirst

- Both drain over time. Sprinting, combat and mining drain them faster. Your CON modifier slows the drain.
- Eating restores Hunger. Drinking restores Thirst. Amounts depend on the item.
- **Hunger 0:** lose 1% max health per 5 seconds, and health regen stops (starting values, tunable).
- **Thirst 0:** lose 1% max health per 3 seconds, and max stamina is halved.
- At Hunger 0 and Thirst 0 together, both health losses apply.

### Meals

Cooked meals (Cook Job) grant a timed buff to one derived stat. Only one meal buff is active at a time. A new meal replaces the old one.

### Water

| Water | Where | Effect of drinking |
|-------|-------|--------------------|
| Fresh | Lakes, rivers | Restores Thirst |
| Poison | Swamp and Jungle hazard water | Restores Thirst like Fresh water, and builds up Poison |
| Salt | The ocean | No Thirst. Applies "Salty": increased Thirst drain for 60 seconds |

Waterskins (Leatherworker) hold 5 drinks and refill at Fresh water.

## Temperature

### Ambient temperature

Ambient temperature = region base temperature + time-of-day offset + weather offset (including wind) + altitude offset + nearby heat sources.

- **Heat sources:** campfire, forge, torch in hand, lava. Each warms within its own radius. Overlapping sources add together, capped at the highest single source's max contribution.
- **Wet:** applies while swimming or standing in rain without shelter, and lasts until 60 seconds (starting value, tunable) after you leave the water or the rain. While Wet, ambient temperature counts as 8 °C lower.

### Shelter

You are sheltered with a roof (terrain or building piece) within 4 m overhead **and** walls in at least 3 of 6 horizontal directions within 5 m. Shelter removes the weather offset. If the roof or walls are dug out, shelter updates on the next tick.

### Comfort range

- Each region has a comfort range of ambient temperatures (starting value 10–26 °C, tunable).
- Your personal range starts from the region's range:
  - equipped **Insulation** lowers the lower bound (cold protection)
  - equipped **Cooling** raises the upper bound (heat protection)
  - your **WIS** modifier widens both bounds
- Insulation and Cooling come from all equipped items, including Insulation and Cooling affixes.

### Body temperature drift

- Ambient inside your comfort range: body temperature drifts back toward 37 °C.
- Ambient below the range: body temperature drifts down. Above the range: it drifts up. The farther outside the range, the faster it drifts.

### Temperature states

| State | Body temperature | Effect |
|-------|------------------|--------|
| Cold | below 35.5 °C | Stamina regen −25% |
| Freezing | below 34 °C | Lose 1% max health per 4 s, movement speed −15%, plus Cold's effect |
| Hot | above 37.5 °C | Thirst drain ×2 |
| Overheating | above 39 °C | Lose 1% max health per 4 s, stamina regen −50%, plus Hot's effect |

You are in at most one state at a time.

## Stamina

- Spent by dodge rolls, attacks, blocking hits, sprinting, climbing and swimming.
- Regenerates after 1 second (starting value, tunable) without spending.
- Base max stamina comes from CON.

**Effective max stamina** = base max stamina × (1 − Fatigue / 200) × Thirst multiplier × Weakened multiplier

| Multiplier | Value |
|------------|-------|
| Thirst | 0.5 at Thirst 0, otherwise 1 |
| Weakened | 0.8 while Weakened (after death), otherwise 1 |

At Fatigue 100 with nothing else active, max stamina is 50%.

## Fatigue

- Rises over time awake: 0 → 100 over 40 real-time minutes (starting value, tunable).
- Rises faster with stamina use: every stamina point spent adds some Fatigue.

## Sleep and beds

- Interacting with a placed bed sets it as your respawn point **in this world** and puts you in bed. Each world keeps its own bed per character.
- If that bed is destroyed or deconstructed, the respawn point is cleared and you respawn at the world spawn point.
- **Everyone in bed at once:** time skips to the next morning (06:00 in-game) and everyone's Fatigue resets to 0.
- **Not everyone in bed:** time does not skip, and each player in bed loses 5 Fatigue per second (starting value, tunable).
- If any player is in combat (dealt or took damage from a hostile enemy within the last 10 seconds), the time skip is blocked and sleeping players see "Cannot sleep — a player is in combat".

## Swimming and breath

- Swimming costs stamina. Swimming at 0 stamina loses 2% max health per second (starting value, tunable) until stamina regenerates above 0 or you leave the water.
- Breath holds 30 seconds of air and drains while your head is underwater.
- After surfacing, Breath refills from empty to full in 2 seconds.
- At Breath 0, you lose 5% max health per second until your head surfaces.
- Breath is full every time you enter a world.

## Source spec

- [Survival](../../specs/survival/survival.md)
