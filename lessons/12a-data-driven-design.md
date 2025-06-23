# Lesson 12a: Data-Driven Design – Loading Game Content from Files

Hard-coding every map, item, and NPC quickly becomes painful.  A *data-driven* approach moves this information out of your C source files and into external text formats that the game loads at run-time.

---
## Why Go Data-Driven?
1. **Iteration speed** – Change a text file, press *R* to reload, done.  No recompile.
2. **Separation of concerns** – Designers edit game content, programmers write systems.
3. **Scalability** – 3 maps today, 300 tomorrow.

---
## 1  Tiny Key-Value Format (`.ini` style)
Before tackling JSON, let's start with the simplest possible parser—one you can write in ~30 lines of C.

`goblin.enemy`:
```
name = Goblin
symbol = g
max_hp = 30
damage = 5
```

Parsing idea:
```c
// pseudo
FILE* f = fopen(path, "r");
while (fgets(line, sizeof line, f)) {
    if (line[0] == '#' || line[0] == '\n') continue; // comment or blank
    char *eq = strchr(line, '=');
    *eq = '\0';
    char *key = trim(line);
    char *value = trim(eq + 1);
    applyToEnemyStruct(key, value);
}
```
> **Exercise** – Implement `trim` and `applyToEnemyStruct`.

This primitive method is great for flat *key=value* data like stats.

---
## 2  Moving to JSON for Nested Data
Complex objects (quests → stages → objectives) need nesting.  The [cJSON](https://github.com/DaveGamble/cJSON) library is lightweight and easy to build.

### Installing cJSON (one-time)
```bash
git clone https://github.com/DaveGamble/cJSON.git
cd cJSON && mkdir build && cd build
cmake .. && make
sudo make install       # installs <cjson/cJSON.h> and libcjson.a
```

### Loading a Map File
`overworld.map.json` (snippet):
```json
{
  "width": 20,
  "height": 15,
  "tiles": "####################\n#..................#\n#....@..............#\n#..................#\n####################"
}
```

```c
#include <cjson/cJSON.h>
#include "map.h"   // your Map struct

Map* load_map(const char *path) {
    char *text = read_text_file(path);       // helper from 18a-save-load.md
    cJSON *root = cJSON_Parse(text);
    if (!root) { printf("Bad JSON: %s\n", cJSON_GetErrorPtr()); return NULL; }

    Map *m = malloc(sizeof(Map));
    m->width  = cJSON_GetObjectItem(root, "width")->valueint;
    m->height = cJSON_GetObjectItem(root, "height")->valueint;

    const char *tiles = cJSON_GetObjectItem(root, "tiles")->valuestring;
    m->tiles = malloc(strlen(tiles)+1);
    strcpy(m->tiles, tiles);

    cJSON_Delete(root);
    free(text);
    return m;
}
```

### Hot-Reload in Engine
Add a key press (`F5`) that frees the old world and calls `load_map()` again.  Designers will love you. 😉

---
## 3  File Structure Suggestions
```
assets/
  maps/
    overworld.json
    dungeon1.json
  enemies/
    goblin.ini
    orc.ini
  items/
    potion.ini
```

Keep parsing code tiny by writing *one loader per asset type*; avoid a mega-parser.

---
## 4  Exercises
1. **INI Items** – Design an `.ini` format for items with fields `name`, `symbol`, `heal`, `value`.  Write `Item* load_item(const char*)`.
2. **Map JSON** – Extend the `Map` JSON to include a list of enemy spawn positions, then update the loader.
3. **Runtime Reload** – Add a check in your main loop: if file `assets/reload.flag` exists, reload all assets then delete the flag.

---
## Commit Your Progress
```bash
git add lessons/12a-data-driven-design.md
git commit -m "Add Lesson 12a: Data-driven design and file loading"
```

---
## Next Steps
Data-driven techniques will shine in **Lesson 21 (Quests)** and beyond.  Feel free to front-load the loaders now so later systems become trivial! 