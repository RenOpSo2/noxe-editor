# Noxe - Architecture Overview

## Project Structure
```

src/
├── global.h    # Shared types, enums, structs
├── term.c/h    # Terminal raw mode I/O
├── nodes.c/h   # Text buffer (doubly-linked list)
├── file.c/h    # File read/write operations
├── cmd.c/h     # Command parser & executor
├── input.c/h   # Modal input handler (state machine)
├── draw.c/h    # Terminal rendering
├── editor.c/h  # Core orchestrator (init/update/deinit)
└── main.c      # Entry point
```




## Module Descriptions

### `global.h`
**What:** Shared definitions for all modules
**Contains:** Enums (`result`, `bool`, `mode`), structs (`node`, `nodes`, `term`, `global`)
**Used by:** Every module
**Dependencies:** `<stddef.h>`, `<stdint.h>`, `<sys/ioctl.h>`

---

### `term.c` / `term.h`
**What:** Terminal abstraction layer
**Does:**
- Enable/disable raw mode (`term_init`/`term_deinit`)
- Read keyboard input (`term_read`)
- Track window size (`term_update`)
**Dependencies:** `global.h`
**Note:** Uses direct ioctl syscalls, no ncurses

---

### `nodes.c` / `nodes.h`
**What:** Text buffer implementation
**Does:**
- Store text as doubly-linked list
- Pre-allocated array pool (262K nodes)
- Free list for O(1) alloc/dealloc
- Cursor navigation (line begin/end, left/right)
- Insert, delete, clear operations
- Serialize to string (`nodes_to_str`)
**Key struct:** `struct nodes` holds cursors (`insert_selector`, `cmd_selector`, `message_selector`)
**Dependencies:** `global.h`

---

### `file.c` / `file.h`
**What:** File system operations
**Does:**
- `file_read()` - Load file into buffer
- `file_write()` - Save buffer to file
**Note:** Single-byte read/write, creates files with 0644
**Dependencies:** `global.h`, `nodes.h`

---

### `cmd.c` / `cmd.h`
**What:** Command mode handler
**Commands:**
- `:open <file>` - Load file
- `:save <file>` - Save file
- `:exit` / `:quit` / `:q` - Exit editor
**Does:** Parse command string, dispatch to file operations, set status messages
**Dependencies:** `global.h`, `nodes.h`, `file.h`

---

### `input.c` / `input.h`
**What:** Input state machine
**Modes:**
- **Normal:** `h` `j` `k` `l` move cursor, `i` insert, `:` command
- **Insert:** Type text, `<BS>` delete, `<ESC>` back to normal
- **Cmd:** Type command, `<Enter>` execute, `<ESC>` back to normal
**Main function:** `input_update()` - reads stdin, dispatches by mode
**Internal functions:** All mode handlers are `static`
**Dependencies:** `global.h`, `nodes.h`, `cmd.h`, `term.h`

---

### `draw.c` / `draw.h`
**What:** Screen rendering
**Does:**
- Clear screen (ANSI escapes)
- Draw status line (mode indicator + messages + command)
- Draw text buffer with scroll offset
- Show cursor as `|`
**Main function:** `draw_update()` - full frame render
**Dependencies:** `global.h`, `nodes.h`

---

### `editor.c` / `editor.h`
**What:** Main orchestrator
**Does:**
- `editor_init()` - Initialize terminal + buffer
- `editor_update()` - Process input → update size → render
- `editor_deinit()` - Cleanup terminal + screen
**Dependencies:** `term.h`, `nodes.h`, `input.h`, `draw.h`

---

### `main.c`
**What:** Program entry point
**Does:**
- Allocate static `struct global`
- Call `editor_init()`
- Main loop: `editor_update()` with 10ms sleep
- Call `editor_deinit()` on exit
**Dependencies:** `editor.h`, `<unistd.h>`

