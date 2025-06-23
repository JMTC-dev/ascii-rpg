# Lesson 0b: Saving Your Progress with Git

Welcome to one of the most important lessons for any developer! Before we write game code, we're going to learn how to use **Git**.

## What is Git?

Think of Git as a **super-powered save system for your code**.

Imagine you're writing an essay. You save it as `essay_v1.doc`. You make changes and save `essay_v2.doc`, then `essay_final.doc`, and finally `essay_really_final.doc`. It gets messy, right?

Git solves this. It lets you:
- **Save "snapshots" (called commits)** of your entire project at any time.
- **Go back to any previous snapshot** if you make a mistake.
- **See exactly what you changed** between snapshots.
- Eventually, collaborate with others (though we'll focus on using it for yourself).

Learning Git is not optional in modern software development. It's a fundamental tool, and learning it now will make you a better programmer.

## Step 1: Check if Git is Installed

Git comes pre-installed with the macOS and Linux setup tools we used in the previous lesson. For Windows, it's often included with other tools like Git Bash, but we'll make sure.

Open your terminal in VS Code (`Ctrl+` ` or `Cmd+` `) and type:
```bash
git --version
```
If you see a version number (e.g., `git version 2.34.1`), you're all set!

**If you get a "command not found" error:**
- **Windows:** Go to [https://git-scm.com/download/win](https://git-scm.com/download/win) and run the installer. The default options are all fine. Restart VS Code and the terminal afterwards.
- **macOS/Linux:** You should have Git already. If not, refer to the setup in Lesson 0 for installing `build-essential` or `xcode-select`.

## Step 2: Initialize Your Project (Create a "Repository")

A project that uses Git is called a "repository" (or "repo" for short). Let's turn our `c-tutorial` folder into one.

1.  Make sure your terminal is open inside your project folder.
2.  Type this command and press Enter:

```bash
git init
```

You should see a message like `Initialized empty Git repository in /path/to/your/c-tutorial/.git/`.

That's it! Your project is now a Git repository. A hidden `.git` folder has been created to track all your changes.

## Step 3: The "Commit" - Your First Snapshot

Saving a snapshot in Git is called making a **commit**. It's a two-step process:

1.  **Staging (git add):** You tell Git *which files* you want to include in your next snapshot.
2.  **Committing (git commit):** You take the snapshot and add a descriptive message.

Let's do it.

### 1. Stage Your Files

In the terminal, type:
```bash
git add .
```
The `.` means "add all files in the current directory and subdirectories." You're telling Git to get everything ready for the snapshot.

### 2. Commit Your Files

Now, type this command:
```bash
git commit -m "Initial commit: Project setup and Hello World"
```
The `-m` flag lets you provide a **commit message**. This message is a short description of what you changed. Good messages are very important!

You've just made your first commit! You've saved the initial state of your project.

## The Workflow You'll Use

From now on, at the end of every lesson, you will repeat this process:
1.  **Save your work:**
    ```bash
    git add .
    ```
2.  **Create a snapshot with a message:**
    ```bash
    git commit -m "Complete Lesson X: Added player movement"
    ```

This creates a clean history of your project, one lesson at a time. If you ever get stuck or your code breaks, you can always look back at the commit from the previous lesson to see a working version.

## Summary

- **Git** is a version control system to save snapshots of your code.
- A project tracked by Git is a **repository**.
- `git init` turns a folder into a repository.
- `git add .` stages all your files for the next snapshot.
- `git commit -m "message"` saves the snapshot with a descriptive message.

This might seem like an extra step now, but trusting us on this one: it will save you from headaches later. It's the professional way to work.

## Next Steps

Now that your project is set up and you know how to save your progress, let's continue on our journey.

Return to [Lesson 1: Introduction](01-introduction.md). 