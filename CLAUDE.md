# ~/code/tools/

Everyday QoL CLI tools on PATH. Synced via `github.com/justin252/tools` (private).

- `h` — fuzzy-browse shell aliases (fzf, Enter copies to clipboard); `h suggest` surfaces forgotten aliases + candidates from history
- `merge-pdf <out.pdf> <input.pdf|dir>...` — merge PDFs; dirs expand to sorted `*.pdf` within

## Workflow

Branch naming: `<type>/<slug>` (e.g. `feat/add-grep-tool`). No direct pushes to main.

### Splitting changes
Prefer splitting logically independent changes (refactor, bug fix, feature) into stacked PRs.

**Proactive (preferred):** When I recognize a separable change while working, branch + commit it immediately before continuing. Cleanest path.

**Retroactive (at checkpoint):** If changes are already mixed, attempt to untangle (split commits, cherry-pick, rebase). If changes are too intertwined to split cleanly, ship as one PR and flag it — don't butcher the split.

Stack order: foundational changes (refactors, extractions, fixes) go first. Dependent features stack on top. Each PR targets the branch below it (or main for first).

### `checkpoint`
1. Build + test
2. Update README if changes affect it
3. Split into stacked PRs if possible (see above), or ship as one
4. Clean up commit history (squash/reword as needed)
5. Push branches, open PRs (global PR template)
6. Retro → INBOX.md

### `checkpoint yolo`
1. Build + test
2. Update README if changes affect it
3. Commit directly to main + push
4. Retro → INBOX.md
