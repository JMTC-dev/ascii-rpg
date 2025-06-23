# Lesson 0: Setting Up Your Development Environment

Before we can start programming, we need to set up the tools. Don't worry - this is easier than it sounds! This lesson will guide you through every step.

## What You'll Need

To write C programs and create games, you need three things:
1. **A text editor** - Where you write code
2. **A C compiler** - Turns your code into programs
3. **A terminal** - Where you run commands

Let's set up each one!

## Step 1: Choosing a Text Editor

A text editor is where you'll write your code. We recommend **Visual Studio Code** (VS Code) because it's:
- Free and open source
- Works on Windows, Mac, and Linux
- Has helpful features for beginners
- Used by professional developers

### Installing VS Code

1. Go to [https://code.visualstudio.com](https://code.visualstudio.com)
2. Click the download button for your operating system
3. Run the installer and follow the prompts
4. Open VS Code when installation completes

### Setting Up VS Code for C

1. Open VS Code
2. Click the Extensions icon on the left (looks like 4 squares)
3. Search for "C/C++"
4. Install the extension by Microsoft
5. This adds syntax highlighting and other helpful features

## Step 2: Installing a C Compiler

The compiler turns your code into programs. The setup depends on your operating system.

### Windows Setup

Windows doesn't come with a C compiler, so we'll install MinGW-w64:

1. Go to [https://www.msys2.org](https://www.msys2.org)
2. Download the installer (usually `msys2-x86_64-[date].exe`)
3. Run the installer, keeping default settings
4. When installation finishes, MSYS2 will open a terminal
5. In this terminal, type:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   ```
6. Press Enter and type 'Y' when asked
7. After installation, add MinGW to your PATH:
   - Open Windows Settings
   - Search for "Environment Variables"
   - Click "Edit the system environment variables"
   - Click "Environment Variables" button
   - Under "System variables", find "Path" and click "Edit"
   - Click "New" and add: `C:\msys64\mingw64\bin`
   - Click OK on all windows

### macOS Setup

macOS needs Xcode Command Line Tools:

1. Open Terminal (find it in Applications > Utilities)
2. Type this command and press Enter:
   ```bash
   xcode-select --install
   ```
3. A popup will appear - click "Install"
4. Wait for installation to complete (may take a while)

### Linux Setup

Most Linux distributions make this easy:

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential
```

**Fedora:**
```bash
sudo dnf install gcc
```

**Arch:**
```bash
sudo pacman -S gcc
```

## Step 3: Testing Your Setup

Let's make sure everything works!

### 1. Create a Test Folder

1. Create a new folder on your Desktop called `c-tutorial`
2. Open VS Code
3. Go to File → Open Folder
4. Select the `c-tutorial` folder

### 2. Write Your First Program

1. In VS Code, click "New File" (or press Ctrl+N / Cmd+N)
2. Save it as `hello.c`
3. Type this code:

```c
#include <stdio.h>

int main() {
    printf("Hello, World!\n");
    printf("My C compiler works!\n");
    return 0;
}
```

4. Save the file (Ctrl+S / Cmd+S)

### 3. Open the Terminal

In VS Code:
- Windows/Linux: Press `Ctrl+` ` (backtick key)
- macOS: Press `Cmd+` `

This opens the integrated terminal at the bottom.

### 4. Compile Your Program

In the terminal, type:
```bash
gcc hello.c -o hello
```

Press Enter. If no errors appear, it worked!

### 5. Run Your Program

**Windows:**
```bash
./hello.exe
```

**macOS/Linux:**
```bash
./hello
```

You should see:
```
Hello, World!
My C compiler works!
```

Congratulations! You're ready to start programming!

## Next Step: Saving Your Work

With your tools working, now is the perfect time to learn about the most important tool for any programmer: **version control**. This will let you save "snapshots" of your project so you never lose your work.

**➡️ Proceed to [Lesson 0b: Saving Your Progress with Git](00b-git-basics.md)**

This is a critical step before you start writing more code.

## Troubleshooting Common Issues

### "gcc not found" or "command not found"

**Windows:** The PATH wasn't set correctly. Try:
1. Close and reopen VS Code
2. If still not working, restart your computer
3. Check that `C:\msys64\mingw64\bin` exists and contains `gcc.exe`

**macOS:** Xcode tools didn't install. Try running:
```bash
gcc --version
```
If it prompts to install tools, say yes.

**Linux:** The package didn't install. Try:
```bash
which gcc
```
If nothing appears, reinstall with your package manager.

### "Permission denied" when running

On macOS/Linux, make the file executable:
```bash
chmod +x hello
./hello
```

### Terminal doesn't open in VS Code

1. Go to View → Terminal
2. If that doesn't work, use your system's terminal:
   - Windows: Search for "Command Prompt" or "PowerShell"
   - macOS: Open Terminal from Applications
   - Linux: Use your distribution's terminal

Then navigate to your folder:
```bash
cd Desktop/c-tutorial
```

## Installing Raylib (For Later)

We'll use Raylib starting in Lesson 7. Here's how to install it now:

### Windows (MSYS2)
```bash
pacman -S mingw-w64-x86_64-raylib
```

### macOS (Homebrew)
First install Homebrew if you don't have it:
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Then:
```bash
brew install raylib
```

### Linux

**Ubuntu/Debian:**
```bash
sudo apt install libraylib-dev
```

**Fedora:**
```bash
sudo dnf install raylib-devel
```

**Arch:**
```bash
sudo pacman -S raylib
```

## Quick Reference Card

Save this for later:

**Compile a C program:**
```bash
gcc myprogram.c -o myprogram
```

**Compile with Raylib (after Lesson 7):**
```bash
gcc myprogram.c -o myprogram -lraylib
```

**Run a program:**
- Windows: `./myprogram.exe`
- Mac/Linux: `./myprogram`

**Common gcc flags:**
- `-Wall` - Show all warnings
- `-g` - Add debug information
- `-o name` - Name the output file

## You're Ready!

Great job setting everything up! Programming tools can be intimidating at first, but you've already overcome the hardest part. 

In the next lesson, we'll start learning C programming. Remember:
- **VS Code** is where you write code
- **gcc** turns your code into programs
- **Terminal** is where you run commands

Continue to the next lesson when you're ready!

## Need Help?

Setup problems are common and not your fault! If you get stuck:
1. Read the error message carefully
2. Google the exact error message
3. Check that you followed each step
4. Ask for help on programming forums with your:
   - Operating system
   - Exact error message
   - What step you're on

Remember: Every programmer has struggled with setup. You're not alone!