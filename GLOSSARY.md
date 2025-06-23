# Glossary – Key Terms in This Tutorial

> Bookmark or open side-by-side while you read lessons.

| Term | Definition |
|------|------------|
| **address** | Numeric location of a byte in RAM.  Obtained with `&var`. |
| **array** | Contiguous block of elements accessed by index `[i]`.  Zero-based in C. |
| **ASCII** | 7-bit character encoding; we draw the entire game using ASCII symbols. |
| **build** | Process of compiling+linking source code into an executable. |
| **camera** | In 2-D games, a viewport that shows a sub-region of the world. |
| **clamp** | Limit a value so it stays within `[min,max]`. |
| **compile** | Translate C source (`.c`) into machine code (`.o`).  Performed by `gcc`. |
| **delta-time (`dt`)** | Time elapsed since the last frame; used for frame-rate-independent movement. |
| **dereference** | Using `*ptr` to access the value stored at the memory address inside `ptr`. |
| **dynamic memory** | Heap space you request with `malloc` and release with `free`. |
| **frame** | One iteration of the game loop: *input → update → draw*. |
| **FPS** | Frames per second; the frequency at which frames are processed. |
| **header (`.h`)** | File containing function prototypes, structs, and defines—no code bodies. |
| **heap** | Region of memory for dynamic allocation (lifetime controlled by the programmer). |
| **initialisation** | Giving a variable its first value at the time of declaration. |
| **JSON** | Text format (`{ "key": 123 }`) useful for hierarchical data files. |
| **library** | Pre-compiled code providing reusable functionality (e.g., Raylib). |
| **linking** | Step that combines object files and libraries into a final executable. |
| **linear interpolation (`lerp`)** | Smoothly move from value **A** to **B** based on parameter `t` in `[0,1]`. |
| **loop** | Structure that repeats code (`for`, `while`). |
| **malloc** | Standard C function `void* malloc(size)` that allocates heap memory. |
| **pointer** | Variable that stores a memory *address*, declared with `*`. |
| **prototype** | Forward declaration of a function so it can be called before its definition. |
| **Raylib** | Simple, beginner-friendly C framework for graphics, input, and audio. |
| **stack** | Memory area that stores local variables and function call data; freed automatically on return. |
| **static** | 1) In variables: lifetime = entire program. 2) In functions: internal linkage (file-local). |
| **struct** | Aggregate data type that groups variables under one name. |
| **vector** | Mathematics: quantity with direction & magnitude; in code: pair/tuple of numbers like `(x,y)`. |
| **window** | OS-level surface where graphics are displayed (created by `InitWindow`). |

*Have a term you'd like added?  Open an issue or PR!* 