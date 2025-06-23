# Lesson 4a: Working with Strings Safely

Strings show up everywhere—from player names to dialogue boxes—so it's worth learning how to handle them *safely* in C.

> If you only remember one rule from this lesson: **never write past the end of an array!** Most security bugs in C programs boil down to that single mistake.

---
## Learning Objectives
By the end of this mini-lesson you should be able to:
1. Explain the difference between a *character array* and a *pointer to char*.
2. Copy text without overflowing the destination buffer.
3. Read user input into a fixed-size buffer without crashing.
4. Use `strlen`, `strncpy`, and `snprintf` correctly.

---
## 1. What *is* a string in C?
C has no dedicated `string` type.  Instead, a string is **just an array of `char` ending with a `\0` byte** (called the *null terminator*).

```c
char name[8] = "ALICE";  // occupies 6 bytes: 'A' 'L' 'I' 'C' 'E' '\0'
```

Because arrays don't remember their own length, you—the programmer—must always track how large the buffer is.

---
## 2. Common string functions
| Function | What it does | Danger |
|----------|--------------|--------|
| `strlen(s)` | Returns the length *not counting* the `\0` | – |
| `strcpy(dst, src)` | Copies until `src`'s `\0` | **No bounds check!** |
| `strncpy(dst, src, n)` | Copies *at most* `n` bytes | If `src` ≥ `n` bytes, no `\0` is added |
| `strncat(dst, src, n)` | Appends up to `n` bytes | Same lack-of-terminator risk |
| `snprintf(dst, size, fmt, …)` | printf-style formatting that **knows buffer size** | Best choice |

### Safer copy example
```c
char playerName[16];
strncpy(playerName, inputName, sizeof(playerName) - 1);
playerName[sizeof(playerName) - 1] = '\0';  // Always add terminator
```

### Prefer snprintf for formatted strings
```c
char msg[32];
int hp = 87;
snprintf(msg, sizeof(msg), "HP: %d/100", hp);  // Never overflows
```

---
## 3. Reading strings from the user
`scanf("%s", buf)` is dangerous—it has no idea how big `buf` is.  Use `fgets` instead:

```c
char command[64];
printf("> ");
if (fgets(command, sizeof(command), stdin) != NULL) {
    // fgets keeps the trailing '\n' – strip it if you want
}
```

---
## 4. Dynamically-sized strings
When you truly need an *arbitrary* length (e.g., loading dialogue from file), allocate exactly as much memory as you need:

```c
size_t len = strlen(src) + 1;      // +1 for '\0'
char *copy = malloc(len);
if (copy) {
    memcpy(copy, src, len);
}
```
Remember to `free(copy);` when done.

---
## 5. Practice Exercises
1. **Name Tag** – Ask the player for their name (max 15 chars).  Store it safely and then greet them.
2. **HP Bar** – Using `snprintf`, build a string like `"HP [#####     ] 50%"` based on current/maximum HP.
3. **Dynamic Dialogue** – Write a function `char* duplicate(const char *text)` that returns a *heap-allocated* copy of any string. Make sure to check for `NULL`.

---
## Commit Your Progress
```bash
git add lessons/04a-safe-strings.md
git commit -m "Add Lesson 4a: Safe string handling"
```

---
## Next Steps
Return to [Lesson 5](05-functions-headers.md) when you're comfortable.  From now on the tutorial will **only** use the safe string patterns you just learned. 