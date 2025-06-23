# Lesson 25: Performance Tuning – Make It Fast, Then Faster

A smooth frame-rate makes any game feel polished.  Before rewriting code blindly, you must **measure**.  This lesson shows three low-cost ways to profile your ASCII-RPG and then walks through common optimisations.

> Estimated time: 30 minutes.  Requires a finished or near-finished build.

---
## 1.  Baseline – How Fast Is Fast Enough?

Target frame-rate:

* Desktop: 60 FPS (frames per second)
* Low-power laptops: 30 FPS acceptable

Anything below feels sluggish.  Let's measure.

---
## 2.  Quick & Dirty FPS Counter

Add this to your main draw loop **temporarily**:

```c
DrawFPS(10, 10);   // Raylib helper – shows real-time FPS
```

Run the game, observe the number.  Drops under 60?  Time to investigate.

---
## 3.  Using Raylib's Built-in Profiler Logs

Enable trace logs **before** initialising the window:

```c
SetTraceLogLevel(LOG_DEBUG);
```

In the terminal you'll see timing for every frame:

```
DEBUG: frame: 16.6 ms | draw: 5.1 ms | update: 11.2 ms | fps: 60
```

If *update* dominates, your game logic is slow.  If *draw* dominates, rendering or GPU uploads are the issue.

---
## 4.  Measuring Function Hot-Spots With `gprof`

1. Re-compile with profiling support:

```bash
gcc -pg -O2 main.c *.c -o ascii_rpg -lraylib
./ascii_rpg      # play for 20-30 seconds, then quit
```

2. Generate the report:

```bash
gprof ascii_rpg gmon.out > perf.txt
less perf.txt | cat
```

Look for the **self seconds** column – that is pure CPU time spent inside each function.

---
## 5.  Common Bottlenecks and Fixes

| Symptom | Likely Cause | Quick Fix |
|---------|--------------|-----------|
| High time in `DrawText()` | Drawing every tile every frame | Only redraw tiles that changed or batch similar tiles |
| Lots of malloc/free | Per-frame dynamic allocations | Pre-allocate arrays, reuse memory |
| Large collision loop | Checking every enemy vs every wall | Spatial partitioning (grid or quadtree) |
| Slow AI | Complex pathfinding each frame | Cache paths, run AI every N frames |

---
## 6.  Example Optimisation – Tile Rendering

*Before*: loop over entire 80×50 map and call `DrawText()` for each cell (4000 calls).

*After*: keep a dirty-flag array; only redraw cells whose glyph or colour changed.  Typical frame now touches < 200 cells – >10× speed-up.

```c
if (dirty[y][x]) {
    DrawTextEx(font, &glyph, pos, fontSize, 0, color);
    dirty[y][x] = false;
}
```

---
## 7.  Re-measure and Celebrate

Run the profiler again.  Did FPS improve?  Commit the optimisation with a clear message:

```bash
git commit -am "Optimise tile drawing – +25 FPS on Intel i5"
```

Remember: *measure → optimise → measure again*.

---
## 8.  Challenge Tasks

1. Use `valgrind --tool=callgrind` to generate a detailed CPU map, view with *KCachegrind* (Linux) or *qcachegrind* (Windows/macOS).
2. Instrument your audio mixer and check if sample conversion is stealing cycles.
3. Add an in-game toggle `F3` to show/hide `DrawFPS()`.
4. Compare `-O2` vs `-O3` vs `-Ofast` – any regressions?

---
## 9.  Summary

• Built-in Raylib logs give you first clues.  
• `gprof` (or `perf`, `Instruments.app`, `Visual Studio Profiler`) pin-point hot-spots.  
• Optimise the *algorithm* before micro-optimising the *code*.  
• Always verify improvements with numbers.

Your game now runs faster and is ready for more complex features – or simply to wow your players with smooth gameplay. 