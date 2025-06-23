# Visual Aids

Below are Mermaid diagrams referenced by various lessons.  Feel free to embed or link them from markdown using the `create_diagram` tool or GitHub's native rendering.

---
## 1. Game Loop (used in Lesson 7 - Raylib Setup)

```mermaid
graph TD
    A[Start Application] --> B{InitWindow}
    B --> C[SetTargetFPS]
    C --> D{Game Loop}
    D -->|Handle Input| E[Update]
    E --> F[Draw]
    F --> D
    D -->|WindowShouldClose| G[CloseWindow]
    G --> H[Exit]
```

---
## 2. Memory Layout (used in Lesson 4 - Pointers & Memory)

```mermaid
flowchart LR
    subgraph Stack
        A[var x]
        B[return addr]
    end
    subgraph Heap
        C[malloc block]
    end
    subgraph Static/Data
        D[global var]
    end
    subgraph Code
        E[text segment]
    end
```

---
## 3. Simple Collision (Axis-Aligned Bounding Box)

```mermaid
graph LR
    subgraph Scene
        player[(Player Box)] -- overlap? --> enemy[(Enemy Box)]
    end
``` 