# YU Assembly Compiler

A full compiler front-end for **YU Assembly** — a custom pseudo-assembly language developed as a term project for CSE 351: Programming Languages at Yeditepe University.This project implements a complete compiler front-end for the language, consisting of a lexical analyzer built with Flex, a syntax analyzer built with Bison, and a loop unrolling optimizer.

The toolchain consists of three tightly integrated components: a **lexer** (Flex), a **parser** (Bison/YACC), and a **loop unrolling optimizer** — taking raw `.asm` source through tokenization, syntax validation, AST construction, and finally code optimization.

---

## Features

- **Lexical Analysis** — Tokenizes YU Assembly source files, recognizing instructions, registers (`R0`–`R7`), memory addressing forms, labels, and integer immediates. Rejects invalid input such as out-of-range registers and unknown characters with descriptive error messages.
- **Syntax Analysis** — Enforces the full YU Assembly grammar via a Bison-generated parser. Builds an **Abstract Syntax Tree (AST)** from the token stream and catches syntax errors including missing commas, incorrect operand types, and misused labels.
- **Loop Unrolling Optimizer** — Walks the AST to detect loops using **backward branch detection**: a loop is identified when a branch instruction targets a label defined earlier in the program. Detected loops are unrolled by a factor of 2 — the loop body is duplicated, memory offsets in the duplicate are incremented by 1, and increment instructions are doubled — reducing the total number of branch and counter-update operations at runtime.

---

## Project Structure

```
YU-Assembly/
├── bin/
│   └── yuasm               # Compiled binary
├── bnf/
│   └── grammar.bnf         # Formal BNF grammar for YU Assembly
├── src/
│   ├── lexer.l             # Flex lexer specification
│   ├── parser.y            # Bison grammar and semantic actions
│   ├── ast.c / ast.h       # AST node definitions and construction
│   ├── unroll.c / unroll.h # Loop unrolling optimizer
│   └── main.c              # Entry point
├── tests/
│   ├── valid1.asm          # Valid test programs
│   ├── valid2.asm
│   ├── valid3.asm
│   ├── invalid1.asm        # Programs with intentional errors
│   ├── invalid2.asm
│   └── invalid3.asm
├── Makefile
├── LICENSE
└── Project Report.pdf
```

---

## Prerequisites

- `gcc` (or compatible C compiler)
- `flex` (fast lexical analyzer generator)
- `bison` (GNU parser generator)
- `make`

On Debian/Ubuntu:
```bash
sudo apt install gcc flex bison make
```

On macOS (via Homebrew):
```bash
brew install flex bison
```

---

## Build & Run

**Build the project:**
```bash
make all
```

**Run against test files:**
```bash
# Valid programs
make run FILE=valid1.asm
make run FILE=valid2.asm
make run FILE=valid3.asm

# Invalid programs (error detection)
make run FILE=invalid1.asm
make run FILE=invalid2.asm
make run FILE=invalid3.asm
```

**Clean build artifacts:**
```bash
make clean   # Remove object files and generated C sources
make clear   # Remove the compiled binary
```

---

## Language Overview

YU Assembly is a pseudo-assembly language with a reduced, regular instruction set. Key language constructs include:

| Construct | Example | Description |
|---|---|---|
| Register | `R0` – `R7` | Eight general-purpose registers |
| Immediate | `#5` | Integer literal operand |
| Label definition | `LOOP:` | Named jump target |
| Branch | `BNZ LOOP` | Branch to label if not zero |
| Arithmetic | `ADD R1, R2, R3` | `R1 = R2 + R3` |
| Memory load | `LOAD R1, [R2]` | Load from memory address in R2 |
| Memory store | `STORE R1, [R2]` | Store R1 to memory address in R2 |

The full formal grammar is available in [`bnf/grammar.bnf`](bnf/grammar.bnf).

---

## Loop Unrolling — How It Works

The optimizer performs a single pass over the constructed AST:

1. **Label tracking** — As the AST is traversed, each defined label and its position is recorded.
2. **Backward branch detection** — When a branch instruction is encountered targeting a previously-seen label, a loop is identified.
3. **Unrolling by factor 2** — The loop body is duplicated immediately following the original body. In the duplicate:
   - Memory offset operands are incremented by 1.
   - Increment/decrement instructions are doubled.
   - The loop counter update is folded into a single operation.

The result is fewer branch instructions and reduced loop overhead at the cost of a larger instruction sequence.

---

## Implementation Details

| Component | Technology |
|---|---|
| Lexer | Flex (`.l`) |
| Parser | Bison/YACC (`.y`) |
| AST | Hand-written in C |
| Optimizer | Recursive AST walker in C |
| Build system | GNU Make |

The build process generates `lex.yy.c` and `parser.tab.c`/`parser.tab.h` into the `build/` directory before compilation.

## License

This project is licensed under the [MIT License](LICENSE).
