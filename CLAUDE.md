# CLAUDE.md

## Repository Overview

**gittest** is a small experimental/learning repository owned by **mokkurikaeshi**, licensed under MIT. It serves as a sandbox for practicing Git workflows (branching, merging, pull requests).

## Project Structure

```
.
├── CLAUDE.md        # Guidelines for AI assistants
├── LICENSE          # MIT License
├── README.md        # Project readme
├── funcB.cpp        # C++ source file
└── test1.txt        # Test text file
```

## Key Files

- **funcB.cpp** — C++ source file added via a feature branch (`functionB`). Currently contains placeholder text; no build system is configured.
- **test1.txt** — Simple test file.
- **README.md** — Minimal project readme.

## Development Workflow

- **Default branch:** `main` (remote) / `master` (local)
- **Branching model:** Feature branches merged via pull requests.
- **Naming convention observed:** Feature branches are named descriptively (e.g., `functionB`).

### Git Conventions

1. Create a feature branch for each change.
2. Open a pull request to merge into `main`.
3. Use clear, descriptive commit messages (e.g., "Add funcB", "modify readme").

## Build & Test

- No build system, test framework, or CI/CD pipeline is currently configured.
- C++ files have no associated `Makefile`, `CMakeLists.txt`, or similar.

## Coding Conventions

- No formal coding style or linter is enforced.
- When adding C++ code, follow standard C++ conventions and keep files small and focused.

## Notes for AI Assistants

- This is a lightweight sandbox repo — keep changes simple and proportional.
- Always branch off `main` for new work and open PRs rather than pushing directly.
- No dependencies or package managers are in use.
