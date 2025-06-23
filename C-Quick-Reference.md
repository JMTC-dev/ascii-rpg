# C Quick Reference (One-Pager)

> Print it, stick it next to your monitor, and update it as you learn new tricks!

---
## 1. Compilation
```
# Single file
gcc main.c -o main

# Multiple files + warnings + debug info
gcc -Wall -Wextra -g file1.c file2.c -o program

# With Raylib (Linux/macOS)
gcc game.c -o game -lraylib -lm -lpthread -ldl
```

---
## 2. Types & Limits (stdint.h)
```
int8_t  / uint8_t   //   8-bit  signed / unsigned
int16_t / uint16_t  //  16-bit
int32_t / uint32_t  //  32-bit
int64_t / uint64_t  //  64-bit
size_t             //  unsigned, big enough to index any array
```

---
## 3. Format Specifiers (printf)
```
%d  int          %ld long          %f  double
%u  unsigned     %lld long long   %c  char
%p  pointer      %s  string       %%  percent sign
```

---
## 4. Operators
```
Arithmetic   +  -  *  /  %
Logical      && || !
Bitwise      &  |  ^  ~  << >>
Comparison   == != < > <= >=
Shorthand    += -= *= /= ++ --
```

---
## 5. Control Flow Skeletons
```c
if (cond) { } else if () { } else { }

for (int i = 0; i < n; i++) { }

while (cond) { }

do { } while (cond);

switch (value) {
    case 1: ...; break;
    default: ...;
}
```

---
## 6. Memory
```c
void *ptr = malloc(bytes);
ptr = realloc(ptr, newSize);
free(ptr); ptr = NULL;
```

---
## 7. String Helpers
```c
strlen(s)                 // length
strncpy(dst, src, n)      // safe copy
snprintf(buf, size, fmt)  // safe formatting
fgets(buf, size, stdin)   // safe input
```

---
## 8. Useful Headers
```
stdio.h   // I/O  (printf, scanf)
stdlib.h  // Memory, rand, atoi, exit
string.h  // strcpy, strlen, memcmp
stdbool.h // bool, true, false
math.h    // sin, cos, sqrt, pow
ctype.h   // isdigit, toupper
assert.h  // assert()
```

---
## 9. Debugging Workflow
1. Compile with `-g -Wall -Wextra`  
2. Run in `gdb`: `gdb ./program`  
3. Use Valgrind for memory: `valgrind --leak-check=full ./program`

---
## 10. Git Basics
```
git init        git add file.c     git commit -m "Msg"
git status      git log --oneline  git diff
```

Happy coding! 🎉 