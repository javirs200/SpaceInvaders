# SpaceInvaders Refactor

This repository is the starting point for refactoring a legacy Space Invaders game project from a 2015 bachelor degree assignment about interfaces.

## Purpose

- Preserve the original code as a reference point.
- Begin refactoring the current spaghetti code into a cleaner, maintainable structure.
- Store the refactored project on GitHub.

## Build / Compile Notes

The project currently uses a VS Code build task in `.vscode/tasks.json` with:

- Compiler: `C:\msys64\ucrt64\bin\g++.exe`
- Flags: `-fdiagnostics-color=always -g`
- Source file: `${file}`
- Output: `${fileDirname}\build\${fileBasenameNoExtension}.exe`

It also resolves headers via `.vscode/c_cpp_properties.json`:

- `compilerPath`: `C:\msys64\ucrt64\bin\gcc.exe`
- `cStandard`: `c17`
- `cppStandard`: `gnu++17`
- `includePath`: `C:\Program Files (x86)\freeglut\include`

## Libraries

This project links against:

- `-lfreeglut`
- `-lopengl32`
- `-lglu32`

At runtime, the FreeGLUT shared library must be available, typically `freeglut.dll` on Windows. Place it in the same folder as the built executable or ensure it is on the system `PATH`.


## Next Steps

- Review `main.cpp` and identify interface-related responsibilities.
- Move platform-specific or rendering code into clear modules.
- Add a proper build system and tests as refactoring progresses.
