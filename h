#!/usr/bin/env zsh
set -euo pipefail
export LC_ALL=C

# Follow loader: if ~/.zshrc sources another file, parse that instead
ZSHRC="$HOME/.zshrc"
if [[ -f "$ZSHRC" ]]; then
  _src=$(grep -m1 '^source ' "$ZSHRC" 2>/dev/null | awk '{print $2}')
  _src="${_src/#\~/$HOME}"
  [[ -n "$_src" && -f "$_src" ]] && ZSHRC="$_src"
  unset _src
fi
ZHIST="$HOME/.zsh_history"

usage() {
  echo "h - shell alias helper"
  echo "  h           fuzzy-browse aliases (Enter copies to clipboard)"
  echo "  h suggest   surface forgotten aliases + suggest new ones from history"
}

# Parse aliases from zshrc: outputs "section\talias\texpansion" lines
parse_aliases() {
  local section=""
  while IFS= read -r line; do
    if [[ "$line" =~ '^# [─A-Za-z]' ]]; then
      section="${line#\# }"
      section="${section#─── }"
      section="${section%% ─*}"
    elif [[ "$line" =~ "^alias ([a-zA-Z0-9._-]+)=[\"'](.*)[\"']$" ]]; then
      printf '%s\t%s\t%s\n' "$section" "${match[1]}" "${match[2]}"
    elif [[ "$line" =~ '^([a-z_][a-zA-Z0-9_]*)\(\)' ]]; then
      printf '%s\t%s\t(function)\n' "$section" "${match[1]}"
    fi
  done < "$ZSHRC"
}

browse() {
  local last_section=""
  parse_aliases | while IFS=$'\t' read -r section name expansion; do
    if [[ "$section" != "$last_section" && -n "$section" ]]; then
      echo "── $section ──"
      last_section="$section"
    fi
    printf '  %-12s →  %s\n' "$name" "$expansion"
  done | fzf --header="Enter: copy alias name" \
             --bind="enter:become(echo {1})" \
             --no-sort \
    | tr -d ' ' | pbcopy
  echo "Copied to clipboard."
}

suggest() {
  typeset -A alias_map inv_map alias_set

  while IFS=$'\t' read -r _ name expansion; do
    alias_map[$name]="$expansion"
    inv_map[$expansion]="$name"
    alias_set[$name]=1
  done < <(parse_aliases)

  # Count commands from history
  typeset -A cmd_counts
  while IFS= read -r line; do
    local cmd="${line#*;}"
    [[ -z "$cmd" ]] && continue
    # Skip continuation lines
    [[ "$cmd" =~ '^\s' ]] && continue
    cmd_counts[$cmd]=$(( ${cmd_counts[$cmd]:-0} + 1 ))
  done < "$ZHIST"

  # 1. Forgotten aliases
  local found_forgotten=0
  for expansion in "${(@k)inv_map}"; do
    local count="${cmd_counts[$expansion]:-0}"
    if (( count >= 2 )); then
      if (( !found_forgotten )); then
        echo "You already have aliases for these:"
        found_forgotten=1
      fi
      printf '  %-24s →  use: %-8s (%d times)\n' "$expansion" "${inv_map[$expansion]}" "$count"
    fi
  done

  # 2. Alias candidates
  (( found_forgotten )) && echo
  local candidates=""
  for cmd in "${(@k)cmd_counts}"; do
    local count="${cmd_counts[$cmd]}"
    (( count < 5 )) && continue
    (( ${#cmd} < 4 )) && continue
    (( ${+alias_set[$cmd]} )) && continue
    (( ${+inv_map[$cmd]} )) && continue
    local suggestion=""
    local words=(${=cmd})
    if (( ${#words} == 1 )); then
      suggestion="${cmd[1,3]}"
    elif (( ${#words} >= 2 )); then
      suggestion=""
      for w in $words; do
        suggestion+="${w[1]}"
      done
    fi
    local line=""
    if [[ -n "$suggestion" ]] && ! (( ${+alias_set[$suggestion]} )); then
      line=$(printf '  %-28s (%d times)  →  try: %s' "$cmd" "$count" "$suggestion")
    else
      line=$(printf '  %-28s (%d times)' "$cmd" "$count")
    fi
    candidates+="$line"$'\n'
  done
  if [[ -n "$candidates" ]]; then
    echo "Consider aliasing:"
    echo "$candidates" | sort -t'(' -k2 -rn
    local found_candidates=1
  else
    local found_candidates=0
  fi

  if (( !found_forgotten && !found_candidates )); then
    echo "Nothing to suggest — you're using your aliases well!"
  fi
}

case "${1:-}" in
  suggest) suggest ;;
  -h|--help) usage ;;
  "") browse ;;
  *) usage; exit 1 ;;
esac
