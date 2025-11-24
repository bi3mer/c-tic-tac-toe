# Tic-Tac-Toe

A simple [raylib](https://www.raylib.com/index.html)-based implementation of *Tic-Tac-Toe* with a [minimax](https://en.wikipedia.org/wiki/Minimax) agent to play against; you can't win!

## Compiling and Running

Instructions assume you are (1) using Linux or macOS and (2) that you have [gcc (C99)](https://gcc.gnu.org/), [zig](https://ziglang.org/), and [git](https://git-scm.com/) installed. (Theoretically, the command below will also work with Windows with minor changes.)

```bash
git clone https://github.com/bi3mer/c-tic-tac-toe.git
cd c-tic-tac-toe
zig build
./zig-out/tic-tac-toe
```

## Release build

```bash
zig build -Doptimize=ReleaseFast                         # mac build
zig build -Dtarget=x86_64-windows -Doptimize=ReleaseFast # Windows Build
```