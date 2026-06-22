# 📖 Tutorial — How the features work

A quick guide to everything the client ships with. Every assistant lives in the
**Options** menu and has its own little window. They all work the same way: set it up
once, tick **Enabled**, and the client does the rest.

> 🌎 *Versión en español: [TUTORIAL.md](TUTORIAL.md)*

> 💡 **Shortcuts.** Each assistant has a `Ctrl+Alt+key` to open its window on the fly.
> When you log in, the client reminds you of the list in the Server Log.

---

## 🗂️ The Options menu

Everything starts here. Right-click the bottom panel → **Options**.

<p align="center">
  <img src="screenshots/01-options-menu.png" alt="Options menu" width="280">
</p>

| Button | What it does |
|---|---|
| **General** | Game options (classic control, marks, PvP frames, sidebars…) |
| **Graphics** | Performance and visual quality |
| **Console** | Customize the console/chat |
| **Hotkeys** | Edit your 36 item/spell hotkeys |
| **Mana Trainer** | Train magic level by casting on its own |
| **Auto Fisher** | Fish automatically |
| **Auto Logout** | Log out on its own (GM/PK/player nearby) |
| **Auto Runer** | Make runes automatically |
| **Auto Eat** | Eat food automatically |
| **Auto Healer** | Heal yourself (and friends) automatically |
| **Combo Leader** | Mirror the combo leader's runes |

---

## ⌨️ Hotkeys — your item shortcuts

<p align="center">
  <img src="screenshots/02-hotkeys.png" alt="Hotkeys window" width="340">
</p>

36 configurable slots (`F1–F12`, `Shift+F1–F12`, `Ctrl+F1–F12`). For each one you pick
an **action**:

- **Clear** — empty the slot.
- **Text** — say a fixed text (e.g. a spell like `exura`).
- **Use on self** — use the item on yourself (e.g. a healing rune).
- **Use on target** — use the item on your current target.
- **Use with crosshair** — use with the crosshair to pick the target with the mouse.

Tick **Send automatically** to fire the text on its own, and fill in the **Rune/Item ID**
when the action uses an item.

> 🎮 **WASD movement.** Besides arrow keys, you can move with `W/A/S/D`.

---

## 💧 Mana Trainer

Casts an attack spell (or whatever you set) automatically to raise your **magic level**
while you're AFK in a safe spot. Turn it on, set the spell and delay, and train.

**Shortcut:** `Ctrl+Alt+M`

---

## 🩹 Auto Healer

<p align="center">
  <img src="screenshots/06-auto-healer.png" alt="Auto Healer" width="320">
</p>

Keeps you alive on its own. Set:

- **Spell** — the healing spell (e.g. `exura`) and **Heal when HP** the % it triggers at.
- **Use UH rune on yourself first** — if ticked, it prioritizes a rune (Ultimate Healing)
  over the spell when your HP drops to **Self HP %**. Give the **UH rune ID** (e.g. `3160`).
- **Heal friends with UH** — also heals the friends in the list (comma-separated) when
  their HP drops below **Friend HP %**.
- **Delay** — how often it checks (ms).

**Shortcut:** `Ctrl+Alt+H`

---

## 🔮 Auto Runer

<p align="center">
  <img src="screenshots/04-auto-runer.png" alt="Auto Runer" width="320">
</p>

Makes runes on its own. You need a **blank rune in hand or an open BP of blanks**. Set the
**Spell** (e.g. `adori vis`), the minimum **Min mana %** to cast, and the **Delay** between
runes.

**Shortcut:** `Ctrl+Alt+R`

---

## 🍖 Auto Eat

<p align="center">
  <img src="screenshots/05-auto-eat.png" alt="Auto Eat" width="320">
</p>

Eats every so often so you don't lose regeneration. Set the **Food ID** (e.g. `3577` = Ham)
and the **Delay** (e.g. 60000 ms = once a minute).

**Shortcut:** `Ctrl+Alt+E`

---

## 🎣 Auto Fisher

Fishes automatically on water tiles with fish. Turn it on with a fishing rod and let it run.

**Shortcut:** `Ctrl+Alt+F`

---

## 🚪 Auto Logout

<p align="center">
  <img src="screenshots/03-auto-logout.png" alt="Auto Logout" width="280">
</p>

The safety assistant. Logs you out based on the conditions you pick:

- **Logout after X min** — log out on inactivity (0 = off).
- **PK skull detected** — if someone with a skull shows up.
- **Player nearby** — if any visible player appears (not you).
- **Players/VIPs** — log out if someone on your list comes online.
- **Hidden GMs** — detects hidden GMs via "last login" changes.
- **Poll every X sec** — how often it checks.
- **Discord webhook** — sends an alert to Discord (with a **Test Alert** button).
- Extras: **sound alarm**, log out on **private message** or on **broadcast/GM message**.

**Shortcut:** `Ctrl+Alt+L`

---

## 🤝 Combo Leader

<p align="center">
  <img src="screenshots/07-combo-leader.png" alt="Combo Leader" width="320">
</p>

For team PvP: it **mirrors the leader's runes**. When the leader throws a rune at a target,
you throw the matching one at the same target instantly.

- **Leader** — the leader's name to follow.
- **Rune / Effect / Rune ID** table — maps each effect to your rune:
  - SD → effect `10`, rune `3155`
  - HMM → effect `4`, rune `3198`
  - GFB → effect `3`, rune `3191`
  - Explosion → effect `0`, rune `3200`
  - Custom → whatever you want
- **Cooldown** — minimum ms between shots.

> 🔎 Set an effect to `0` to make the client log the projectile ID, so you can discover
> new effects.

**Shortcut:** `Ctrl+Alt+C`

---

## 🗺️ Minimap

The minimap ships **pre-loaded** with the server map, so it shows from the first launch (no
black screen). Whatever you explore is saved automatically, next to the `.exe`.

---

## 💰 Handy detail

- **Right-click to stack gold** — right-clicking gold sends it straight to your backpack/bag.

---

Something not working as expected? Open an *issue* on the repo. 🐛
