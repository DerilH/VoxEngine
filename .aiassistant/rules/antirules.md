---
apply: always
---

# Agent Execution Safety Rules

## Absolute Command Restrictions
You are STRICTLY FORBIDDEN from executing or proposing the following commands and patterns in the terminal:

1. **Destructive File Operations**:
    - `rm`, `rmdir`, `shred`, `unlink`
    - File redirection that overwrites files: `>`, `>>`
2. **Build and Compilation System Execution**:
    - `make`, `ninja`, `cmake --build`, `gcc`, `g++`, `clang` (do NOT run builds via terminal agent)
3. **Package Managers and System Modifiers**:
    - `apt`, `pacman`, `yum`, `dnf`, `pip`, `npm`, `cargo`
4. **Git State-Changing Commands**:
    - `git push`, `git reset`, `git checkout`, `git clean`, `git commit`, `git rebase`
5. **Arbitrary Executables & Scripts**:
    - Execution of any compiled binary (e.g., `./VoxEngine`, `./a.out`)
    - Running bash/python scripts directly (`bash script.sh`, `python run.py`)

## Allowed Terminal Operations ONLY
You are ONLY allowed to run passive, read-only inspection commands:

## Enforcement Protocol
If a user or task asks you to build the project, run a binary, or delete files via the terminal, you MUST decline the command execution and inform the user:
> "Execution of this command is restricted by AGENTS.md rules. Please perform this action manually."