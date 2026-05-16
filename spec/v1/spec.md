# VNE (Visual Novel Engine) Specification v1

This specification defines the syntax and deterministic execution rules for the VNE script format. The Markdown script is the source of truth for sequential logic, asset triggering, branching, variable creation, variable mutation, and runtime state used by the story.

## 1. Core Parsing Principles

- **Version Header:** Every script file must declare its format version on line 1 as `<!-- v1 -->`. This header is metadata only and is never treated as an executable instruction.
- **Entry Point:** `main.md` is the required entry point for a script package. A fresh run begins at the top of `main.md`, immediately after the version header, and then follows normal top-to-bottom execution.
- **Required Font:** The script package root must include `font.ttf` next to `main.md`. The runtime loads this file as `font.ttf` relative to the process working directory for dialogue, narration, and choice text.
- **Line-Based Tokenization:** Every instruction, conditional, or text block is evaluated on a single-line basis (separated by `\n`).
- **Top-to-Bottom Execution:** Execution advances sequentially unless a `jmp` command changes the next line or execution pauses for Choices, Waits, or User Input.
- **Whitespace:** Leading and trailing whitespace on a line is ignored. Empty lines are discarded.
- **Path Resolution:** All file and asset paths are resolved relative to the process working directory.

---

## 2. Lexical Elements & Syntax

**A. Labels (Jump Targets)**
Defines a named jump destination in the script.

- **Syntax:** One or more leading `#` characters followed by a label name using letters, digits, `_`, or `-`, e.g. `# some-scene`, `## some_branch`, or `### Branch3`.
- **Behavior:** Labels are not displayed and do not pause execution.

**B. Dialogue & Narration**
Text rendered to the dialogue box. Execution waits for user advancement after the line is displayed.

- **Narration Syntax:** Standard string (e.g., `The sun is setting.`)
- **Dialogue Syntax:** `**Bob**: "Hello!"`
- **Interpolation:** Substrings wrapped in `{}` (e.g., `{coins}`) are replaced with the current state value when the text is displayed. Missing variables resolve to `0`.

**C. Choices (Branching)**
Consecutive choice lines form a single choice block. `> if` / `> endif` branches may wrap individual choice lines to control which choices are shown. The block ends at the first line that is neither a choice nor a conditional belonging to the block.

- **Syntax:** `[Some Choice](#target-label)` or `[Some Choice](another-scenario.md)`
- **Behavior:** Execution pauses and displays the visible choices. Selecting a choice continues execution at the target label or at the first executable line after the version header in the target markdown script file.

---

## 3. Command Instruction Set (`>`)

Lines prefixed with `>` are commands and are not displayed as dialogue. Arguments are space-delimited.

### Control Flow & State Mutation

- `> jmp [](#target-label)`: Continues execution at the specified label. Does not pause.
- `> jmp [](another-scenario.md)`: Continues execution in another markdown script file at its first executable line after the version header. Does not pause.
- `> set some_var 5`: Sets the specified state variable to the integer value.
- `> add some_var 5`: Adds the integer value to the specified state variable.
- `> add some_var -5`: Adds the signed integer value to the specified state variable.
- **Variables:** Variables are created and owned by the script at runtime. `set` creates or replaces a variable value. `add` creates a missing variable with value `0` before applying the addition.

**Conditionals (`if` / `endif`)**
Evaluates state to gate script execution.

- `> if coins >= 10`
- `> endif`
- **Behavior:** If the expression is true, the lines until the matching `> endif` execute normally. If the expression is false, those lines are skipped. Conditionals may wrap choices to control whether those choices are shown. Nested `> if` blocks are invalid; an `> if` block must end before another `> if` begins.

**Visuals & Audio**
Starts, changes, or stops audio and scene visuals. Optional transition arguments define how visual changes are shown.

- **Transitions:** Valid transition names are `fade`, `slide_in_top`, `slide_in_bottom`, `slide_in_left`, and `slide_in_right`.
- **Sprite Positions:** Valid sprite positions are `left`, `center`, and `right`.

- `> audio play [path] [loop_flag?]`
- `> audio resume`
- `> audio pause`
- `> audio stop`
  - _Example:_ `> audio play bgm_calm.ogg loop`
- `> bg show [path] [transition?] [duration?]`
- `> bg hide`
  - _Example:_ `> bg show image.png fade 1.5s`
- `> sprite show [position] [path] [transition?] [duration?]`
- `> sprite hide [position]`
  - _Example:_ `> sprite show left bob.png slide_in_left 0.5s`

### Timing

- `> wait [duration][s?]`
  - _Example:_ `> wait 2.0s`
  - The `s` suffix is optional for all duration arguments.
  - **Behavior:** Execution pauses for the specified duration, then resumes automatically without user input.

---

## 4. Execution Summary

Execution is determined by each line's prefix:

1. Validate that line 1 of every file is exactly `<!-- v1 -->`.
2. Start fresh execution from `main.md`.
3. `#` -> Define a label.
4. `>` -> Execute a command.
   - `if` evaluates a single-level conditional block.
   - `jmp` changes the next executed label or script file.
   - `wait` pauses execution for the requested duration.
5. `[` -> Start or continue a choice block. Continue through choice lines and any single-level `if` / `endif` branches that wrap choices, then pause for selection.
6. `**` -> Display speaker dialogue and wait for user advancement.
7. _(Fallback)_ -> Display narration and wait for user advancement.
