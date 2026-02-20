# tools

Everyday QoL CLI tools. macOS + zsh.

## Tools

- **`h`** — fuzzy-browse shell aliases (`fzf`, Enter copies to clipboard); `h suggest` surfaces forgotten aliases + candidates from history
- **`merge-pdf`** `<out.pdf> <input.pdf|dir>...` — merge PDFs; dirs expand to sorted `*.pdf` within

## Setup

Add to PATH:

```sh
export PATH="$HOME/code/tools:$PATH"
```

### Dependencies

- [fzf](https://github.com/junegunn/fzf) (`brew install fzf`) — for `h`
- [uv](https://github.com/astral-sh/uv) (`brew install uv`) — for `merge-pdf`
