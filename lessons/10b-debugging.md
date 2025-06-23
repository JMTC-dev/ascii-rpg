# Lesson 10b: Debugging 101 – Finding and Fixing Bugs

Programming without debugging skills is like flying blind.  In this mini-lesson you will set up a debugger, learn the basic commands, and trace a crash _step-by-step_.  We use **gdb** on Linux/Mac and the built-in VS Code debugger on all platforms.

> Estimated time: 20 minutes.  Keep the small `ascii_explorer.c` project from Lesson 10 open while following along.

---
## 1.  Compiling for Debugging

First ensure the executable contains debug symbols and no optimisations that would rearrange code lines:

```bash
gcc -g -O0 ascii_explorer.c -o ascii_explorer -lraylib
```

Flags explained:
* `-g`   → embed symbol information so the debugger knows which line equals which instruction
* `-O0` → turn off optimisations to keep the machine code 1-to-1 with your C source

> ⚠️  **Never release with `-O0`!**  Use `-O2` or `-O3` for the final build.

---
## 2.  Starting gdb

```bash
gdb ./ascii_explorer
```

Once inside you see a prompt `(gdb)`. Useful commands:

| Command | Short | Description |
|---------|-------|-------------|
| `start` |      | run until `main()` and stop |
| `break <line or func>` | `b` | set breakpoint |
| `next`  | `n`  | execute the _next line_, stepping _over_ function calls |
| `step`  | `s`  | execute the next line, stepping _into_ functions |
| `continue` | `c` | resume until next breakpoint |
| `print <var>` | `p` | show value of variable |
| `backtrace` | `bt` | show call-stack when crashed |
| `quit` | `q` | leave gdb |

Try it now:

```gdb
(gdb) start
(gdb) n        # step once
(gdb) b UpdatePlayer
(gdb) c        # run until the breakpoint
(gdb) p player # inspect player struct
```

---
## 3.  VS Code Graphical Debugger (Optional)

1. Press `Ctrl+Shift+D` to open the **Run & Debug** panel.
2. Click *create `launch.json`* and choose **C/C++ (gdb/lldb)**.
3. Make sure the generated JSON contains:

```jsonc
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/ascii_explorer",
            "args": [],
            "stopAtEntry": true,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "MIMode": "gdb",
            "setupCommands": [
                { "text": "-enable-pretty-printing" }
            ]
        }
    ]
}
```

4. Set a breakpoint by clicking in the gutter next to a code line.
5. Press the green ► **Start Debugging** button.

You can now inspect variables in the *Variables* pane, watch expressions, and step through the program with the toolbar.

---
## 4.  Post-mortem: Investigating a Crash

Suppose the game crashes with a segmentation fault.  Run it inside gdb:

```bash
gdb ./ascii_explorer -ex run
```

When it crashes you'll see something like:

```gdb
Program received signal SIGSEGV, Segmentation fault.
0x0000555555558b42 in UpdateEnemyAI (enemy=0x0) at enemies.c:42
42          if (enemy->state == ENEMY_IDLE) {
(gdb) bt
#0  UpdateEnemyAI (enemy=0x0) at enemies.c:42
#1  UpdateGame    () at main.c:135
#2  main          () at main.c:55
```

1. `bt` (backtrace) shows how we got here – note the null pointer.
2. `frame 0` then `p enemy` confirms `enemy = 0x0`.
3. `up` / `down` navigate stack frames to examine caller variables.

Use this method any time you get *"Segmentation fault (core dumped)"*.

---
## 5.  Cheat-Sheet (print and tape to your monitor!)

| Goal                 | gdb Command        |
|----------------------|--------------------|
| Start program        | `start`            |
| Continue             | `c`                |
| Break at line 120    | `b 120`            |
| Break in function f  | `b f`              |
| Next line (step over)| `n`                |
| Step into function   | `s`                |
| Print variable       | `p var`            |
| Modify variable      | `set var = 42`     |
| List source lines    | `list`             |
| Show call-stack      | `bt`               |

---
## 6.  Summary Checklist

- [x] Can compile with `-g -O0`
- [x] Able to set breakpoints and step through code
- [x] Know how to inspect variables and call-stack
- [ ] **Challenge:** purposely insert an out-of-bounds array access and use gdb to locate it

Proceed to **Lesson 11: Creating Game Worlds** (file `11-game-world-part1.md`). 