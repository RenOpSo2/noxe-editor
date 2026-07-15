# Noxe-Editor

Noxe-Editor is a very efficient text editor for terminal users.

## Why use noxe-editor?

noxe-editor is considered very efficient for writing everyday code and is friendly for users using shortcuts like vscode.

## Features

- **Fast and efficient**: Built with C for maximum performance
- **Auto-indent**: Automatically preserves indentation when pressing Enter
- **Undo/Redo**: Full undo/redo support with Ctrl+U and Ctrl+Y
- **Syntax highlighting**: C syntax highlighting for better code readability
- **Large file support**: Paged Gap Buffer for handling massive documents
- **Clipboard**: Copy, cut, and paste functionality
- **Selection**: Shift+arrow keys for text selection
- **Search**: Find text with Ctrl+F, navigate with Ctrl+N (next) / Ctrl+P (previous)
- **Line Numbers**: Display line numbers in left gutter
- **Screen Refresh**: Ctrl+R to fix rendering issues
- **Memory efficient**: Uses arena allocator for optimized memory management

## Installation

### Building from source

```bash
git clone <repository-url>
cd noxe-editor
make
```

The compiled binary will be available at `bin/noxe`.

### Running

```bash
./bin/noxe [filename]
```

Or use the make target:

```bash
make run
```

## Usage

### Basic Usage

```bash
# Open a file
./bin/noxe myfile.c

# Start with empty file
./bin/noxe
```

### Keybindings

#### Editing
- **Arrow keys**: Move cursor up/down/left/right
- **Enter**: Insert new line (with auto-indent)
- **Backspace/Delete**: Delete character
- **Tab**: Insert tab character

#### File Operations
- **Ctrl+S**: Save file
- **Ctrl+Q**: Quit editor

#### Edit Operations
- **Ctrl+C**: Copy selection
- **Ctrl+X**: Cut selection  
- **Ctrl+V**: Paste clipboard
- **Ctrl+A**: Select all

#### Undo/Redo
- **Ctrl+U**: Undo last action
- **Ctrl+Y**: Redo undone action

#### Search
- **Ctrl+F**: Start search
- **Ctrl+N**: Next match
- **Ctrl+P**: Previous match
- **ESC**: Cancel search

#### View
- **Ctrl+R**: Refresh screen

#### Selection
- **Shift+Arrow keys**: Extend selection

### Example Session

```bash
# Open a C file
./bin/noxe example.c

# Edit your code (auto-indent works automatically)
# Use Ctrl+S to save
# Use Ctrl+Q to quit
```

## Testing

Run the unit tests:

```bash
make test
```

## Building

Available make targets:

```bash
make           # Build the project
make run       # Build and run
make test      # Build and run tests
make clean     # Remove build artifacts
make format    # Format source code
make help      # Show available targets
```

## Architecture

- **Paged Gap Buffer**: Efficient text storage and manipulation
- **Arena Allocator**: Optimized memory management for nodes
- **Render Buffer**: Double-buffered terminal rendering
- **Syntax Highlighting**: Token-based C syntax highlighting

## Version

```bash
./bin/noxe --version
```

## License

MIT License

Copyright (c) 2026 Revan Aprian

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

