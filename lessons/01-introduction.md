# Lesson 1: Introduction and Setup

Welcome to your journey into game development! In this first lesson, we'll explore what we're building, why ASCII games are perfect for learning, and get you thinking like a game developer.

## What is an ASCII RPG?

ASCII (pronounced "ask-ee") stands for American Standard Code for Information Interchange. It's a character encoding standard that includes letters, numbers, and symbols. An ASCII game uses these text characters to create graphics!

Here's what our game will look like:
```
####################
#..................#
#...@..........g...#
#..................#
#...####...####....#
#...#  #...#  #....#
#...#  +...+  #....#
#...#  #...#  #....#
#...###+####+###...#
#..................#
#......!...../.....#
#..................#
####################

HP: 10/10  Gold: 0
```

In this example:
- `#` represents walls
- `@` is you, the player
- `g` is a goblin enemy
- `+` represents doors
- `!` is a health potion
- `/` is a sword
- `.` is empty floor space

## Why Learn with ASCII Games?

1. **Focus on Logic, Not Art**: You don't need to be an artist or learn complex graphics
2. **See Everything**: The game's state is visible as simple text
3. **Quick Results**: You'll have something playable fast
4. **Retro Cool**: ASCII games have a unique aesthetic charm
5. **Foundation for More**: The concepts apply to all games

## What Makes a Game?

Before we dive into code, let's understand the core components of any game:

### 1. The Game Loop
Every game runs in a continuous loop:
```
while (game is running) {
    1. Handle Input (what keys did the player press?)
    2. Update Game State (move characters, check collisions)
    3. Draw Everything (show the current state on screen)
}
```

### 2. Game State
This is all the information about your game at any moment:
- Where is the player?
- How much health do they have?
- Where are the enemies?
- What items are on the ground?

### 3. Input Handling
Games respond to player actions:
- Keyboard keys for movement
- Mouse clicks for menus
- Controller buttons for actions

### 4. Rendering (Drawing)
This is how we show the game state to the player. In our ASCII game, we'll draw characters to represent everything.

## What is Raylib?

Raylib is a simple and easy-to-use library for game development. Think of it as a toolkit that provides:
- Window creation
- Keyboard and mouse input
- Drawing text and shapes
- Sound playing
- Much more!

Without Raylib, we'd have to write hundreds of lines of complex code just to open a window!

## What is C?

C is a programming language created in 1972. It's:
- **Fast**: Games need speed, and C delivers
- **Direct**: You have precise control over everything
- **Foundational**: Many other languages are built on C
- **Still Relevant**: Used in game engines, operating systems, and embedded devices

Don't worry if you've never programmed before - we'll learn C step by step!

## Your Game Development Mindset

As you begin this journey, adopt these principles:

1. **Start Small**: Don't try to build everything at once
2. **Test Often**: Run your code frequently to catch errors early
3. **Break Problems Down**: Big problems are just many small problems
4. **Embrace Errors**: They're not failures, they're learning opportunities
5. **Be Creative**: Once you understand the basics, make the game your own!

## What You'll Need

Before the next lesson, make sure you have:
1. A text editor (we recommend Visual Studio Code - it's free!)
2. A C compiler (we'll show you how to install one)
3. About 30-60 minutes per lesson
4. A curious mind!

## A Peek at the Journey Ahead

Here's what we'll build step by step, following the structure from the main README:

**Foundation (Lessons 2-6)**: Learn C basics, pointers, and build a text-based game.
**Graphics and Game Basics (Lessons 7-10)**: Get started with Raylib, render ASCII, and handle player movement.
**Core Game Systems (Lessons 11-15)**: Create game worlds, collision, and basic enemies.
**Advanced Systems (Lessons 16-20)**: Implement combat, inventory, and NPCs.
**Polish and Completion (Lessons 21-25)**: Add quests, game states, and finish the game.

## Think Like a Game Designer

Before we start coding, let's think about our game:

1. **What's the player's goal?** (Defeat enemies? Collect treasure? Reach the exit?)
2. **What makes it challenging?** (Enemies? Puzzles? Limited resources?)
3. **What makes it fun?** (Discovery? Strategy? Character progression?)

Write down your ideas - we'll refer back to them as we build!

## ASCII Art Exercise

Try creating your own ASCII art! Open a text editor and draw:
- A house using #, -, |, and other characters
- A tree using ^, |, and *
- Your own character designs

This helps you think visually with text characters.

## Commit Your Progress

You've completed the first lesson! This is a perfect time to save a snapshot of your project. Open your terminal and run these commands:

```bash
git add .
git commit -m "Complete Lesson 1: Introduction"
```

This command saves all your files. The message clearly states what you've accomplished. Making this a habit is one of the best things you can do as a developer.

## Summary

In this lesson, you learned:
- What ASCII games are and why they're great for learning
- The core components of any game
- What Raylib and C are
- The mindset for successful game development

You're now ready to start your game development journey!

## Next Steps

In the next lesson, we'll dive into C programming basics. Don't worry - we'll take it slow and explain everything clearly. You'll write your first C program and understand how code becomes a running application.

Ready? Let's go to [Lesson 2: C Basics Part 1](02-c-basics-part1.md)!

## Challenge

Before moving on, try to design on paper:
1. A simple 10x10 game level using ASCII characters
2. Three different enemy types using single characters
3. Five different item types using symbols

This will get you thinking creatively about ASCII representation!