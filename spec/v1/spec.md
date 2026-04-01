# VNE (Visual Novel Engine) Specification v1.0

This specification defines the syntax and deterministic execution rules for the VNE script format. It is strictly limited to sequential logic, asset triggering, and markdown script branching. Global variable definitions and persistent state schemas are handled externally (e.g., via TOML) and are outside the scope of this file format.

## 1. Core Parsing Principles

- **Version Header:** Every script file must declare its format version on line 1 as `<!-- v1 -->`. This header is metadata only and is never treated as an executable instruction.
- **Entry Point:** `main.md` is the required entry point for a script package. A fresh run begins at the top of `main.md`, immediately after the version header, and then follows normal top-to-bottom execution.
- **Line-Based Tokenization:** Every instruction, conditional, or text block is evaluated on a single-line basis (separated by `\n`).
- **Top-to-Bottom Execution:** The instruction pointer (IP) increments sequentially unless explicitly mutated by a `jmp` command or halted by a yield state (Choices, Waits, or User Input).
- **Whitespace:** Leading and trailing whitespace on a line is ignored by the lexer. Empty lines are discarded.

---

## 2. Lexical Elements & Syntax

**A. Labels (Jump Targets)**
Defines a hashable entry point in the script. The C++ parser maps the kebab-case string to the line number of the script file.

- **Syntax:** `# some-scene` or `## some-branch`
- **Engine Action:** Registers the target in the active jump table. No execution cost during runtime.

**B. Dialogue & Narration**
Text rendered to the dialogue box. Execution yields after pushing the string, waiting for user advancement.

- **Narration Syntax:** Standard string (e.g., `The sun is setting.`)
- **Dialogue Syntax:** `**Bob**: "Hello!"`
- **Interpolation:** Substrings wrapped in `{}` (e.g., `{coins}`) are preserved as raw strings during parsing. The UI rendering pipeline queries the state manager to replace them immediately before glyph generation.

**C. Choices (Branching)**
Aggregated into a UI block. When the parser encounters the first choice, it enters a `ChoiceCollection` state. While collecting, it continues consuming additional choice lines plus any surrounding `> if` / `> endif` branches that gate choice visibility. The block ends when a non-choice, non-conditional token is hit.

- **Syntax:** `[Some Choice](#target-label)`
- **Engine Action:** Halts the IP, renders the collected choices whose surrounding conditionals evaluated to `true`, and updates the IP to the target label upon user selection.

---

## 3. Command Instruction Set (`>`)

Lines prefixed with `>` bypass the dialogue buffer and are routed directly to the engine's command dispatcher. Arguments are space-delimited.

### Control Flow & State Mutation

- `> jmp [](#target-label)`: Immediately sets the IP to the specified label. Does not yield.
- `> jmp [](another-scenario.md)`: Loads another markdown script file and sets the IP to its first executable line after the version header. Does not yield.
- `> inc some_var`: Increments the specified state variable by 1.
- _(Note: Operations like `dec`, `set`, or `toggle` can follow this exact signature)._

**Conditionals (`if` / `endif`)**
Evaluates state to gate script execution.

- `> if coins >= 10`
- `> endif`
- **Engine Action:** If the expression evaluates to `false`, the parser sets an internal `is_skipping` flag. All subsequent lines (dialogue, commands, choices) are ignored until an `> endif` token is parsed, which clears the flag. This same gating mechanism is valid inside a choice collection, allowing conditional branches to add or omit individual choices without breaking the block.

**Visuals & Audio**
Triggers engine subsystem calls. Optional arguments dictate asynchronous rendering transformations.

- `> bg [action] [asset_id] [transition?] [duration?]`
  - _Example:_ `> bg set street_evening fade 1.5s`
- `> sprite [action] [character_id] [expression] [position] [transition?] [duration?] [easing?]`
  - _Example:_ `> sprite show bob happy left slide_in 0.5s ease_out`
  - _Example:_ `> sprite set bob surprised left` (Updates existing sprite state).
- `> audio [action] [asset_id] [loop_flag?]`
  - _Example:_ `> audio play bgm_calm loop`
  - _Example:_ `> audio stop bgm_calm`

### Timing

- `> wait [duration]s`
  - _Example:_ `> wait 2.0s`
  - **Engine Action:** Halts the IP and yields script execution to the engine's timer subsystem. Resumes automatically without user input when the duration expires.

---

## 4. Execution State Machine Summary

To parse this cleanly in C++, the lexer only needs to check the first few characters of a `std::string_view` for each line:

1. Validate that line 1 of every file is exactly `<!-- v1 -->`.
2. Start fresh execution from `main.md`.
3. `#` -> Map label.
4. `>` -> Route to Command Dispatcher.
   - If `if` -> Evaluate. If false, enter Skip State until `endif`.
   - If `jmp` to another `.md` file -> Switch active script file and resume from its first executable line.
   - If `wait` -> Enter Engine Yield State.
5. `[` -> Enter Choice Collection State. Continue consuming choice lines and any surrounding `if` / `endif` branches. Yield on completion.
6. `**` -> Extract Speaker and Dialogue. Enter User Yield State.
7. _(Fallback)_ -> Push Narration. Enter User Yield State.
