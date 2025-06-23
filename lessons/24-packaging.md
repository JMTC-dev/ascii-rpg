# Lesson 24: Packaging Your Build – Share Your Game With the World

You just finished **Lesson 23 – Complete Game**.  Congratulations!  Now let's turn your developer build into something friends can download and run without a compiler or libraries installed.

You've built a complete game. Now it's time for the most exciting part: sharing it with the world! Packaging is the process of turning your developer project into a simple, double-clickable application that anyone can run, even if they don't have a compiler or Raylib installed.

> **Goal:** Produce a distributable package for your operating system that you can send to a friend.

---
## 1. The "Why": Release vs. Debug Builds

So far, we've been creating **debug builds**. These are perfect for development because they include extra information that helps us find bugs (`-g` flag).

Now, we'll create a **release build**. This version is optimized for players:
| Aspect | Debug Build (`-g`) | Release Build (`-O2 -DNDEBUG`) |
|---|---|---|
| **Size** | Larger | Smaller and more compact |
| **Speed** | Slower | Highly optimized by the compiler to run faster |
| **Symbols** | Includes debug info | Stripped of debug info |

The `-O2` flag tells the compiler to apply level-2 optimizations, and `-DNDEBUG` disables assertions and other debug-only code.

Create a `Makefile` target called `release`:

```makefile
release:
	gcc -O2 -DNDEBUG main.c *.c -o ascii_rpg \
	    -lraylib -lm -lpthread -ldl -static
```

* `-static` attempts to include all required libraries in one executable (Linux & Windows).  macOS uses `.app` bundles instead.

---
## 2.  Packaging on Windows – Zip It Up

1. Build in an **MSYS2 MinGW 64-bit** shell:

```bash
make release
strip ascii_rpg.exe       # remove debug symbols, shrink size
```

2. Copy `ascii_rpg.exe` plus the `raylib.dll` (if static link failed) into a folder `AsciiRPG`.
3. Add a `README.txt` with controls and licence notice.
4. Right-click → *Send to → Compressed (zipped) folder* → `AsciiRPG-v1.0.zip`.

Share the ZIP – users just unzip and double-click `ascii_rpg.exe`.

---
## 3.  Packaging on Linux – AppImage in Two Commands

```bash
sudo apt install appimagetool
appimagetool --appdir DistDir ascii_rpg.AppImage
```

Where `DistDir` contains:

```
DistDir/
  usr/bin/ascii_rpg      # your release build
  ascii_rpg.desktop
  ascii_rpg.png          # 256×256 icon
```

The resulting `ascii_rpg.AppImage` runs on any modern distro: `chmod +x` then `./ascii_rpg.AppImage`.

---
## 4.  Packaging on macOS – .app Bundle

```bash
clang -O2 -DNDEBUG -framework CoreVideo -framework IOKit \
       -framework Cocoa -framework GLUT -framework OpenGL \
       -o ascii_rpg main.c *.c -lraylib
```

Create the bundle structure:

```
AsciiRPG.app/
  Contents/
    MacOS/        ascii_rpg      # executable
    Resources/    icon.icns
    Info.plist
```

Use a helper like `create-dmg` to make a nice drag-and-drop installer.

---
## 5.  Cross-Platform CI Automation (Optional)

Set up **GitHub Actions** with three jobs:

* `ubuntu-latest` → builds AppImage, uploads artefact
* `windows-latest` → builds & zips `ascii_rpg.exe`
* `macos-latest`   → builds `.app` and creates `.dmg`

Every push to `main` builds nightly artefacts; tagged releases upload to *GitHub Releases*.

---
## 6.  Checklist Before You Hit Publish

- [x] Release build passes all **unit tests** (`./run_tests.sh`)
- [x] Game starts without terminal output/errors
- [x] README includes controls, known issues, licence
- [x] Version number bumped (`v1.0.0 → v1.0.1`)
- [x] Tested on a fresh VM with no compiler installed

> ✅  Your ASCII RPG is now one double-click away for your players.

Proceed to **Lesson 25 – Performance Tuning** to squeeze extra FPS out of the game. 