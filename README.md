# arduboy-games

Tiny games for the [Arduboy](https://www.arduboy.com)

## Minesweeper

Minimalistic implementation, flashes red LED on lose, green LED on win

Controls:

- `UP`, `DOWN`, `RIGHT`, `LEFT` buttons to navigate the field
- `A` button to: reveal a tile if not revealed, reveal closest non-flagged neighbors (helps speed up opening the non-mined tiles)
- `B` button to toggle flag on a tile (mark tile as mined)

.hex file is available in the release, you can try it [on the emulator as well](https://felipemanga.github.io/ProjectABE/?url=https://github.com/SuperSolik/arduboy-games/releases/download/minesweeper-done/minesweeper.ino.hex)
