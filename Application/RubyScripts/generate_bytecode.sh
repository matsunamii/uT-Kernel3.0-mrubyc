#!/usr/bin/env bash
set -eu

script_dir=$(cd "$(dirname "$0")" && pwd)
app_dir=$(cd "$script_dir/.." && pwd)

mrbc_bin="$app_dir/bin/mrbc"
input_dir="$script_dir"
output_dir="$script_dir/generated"

if [[ ! -f "$mrbc_bin" ]]; then
  echo "Error: mrbc not found: $mrbc_bin" >&2
  exit 1
fi

mkdir -p "$output_dir"

for input_file in "$input_dir"/*.rb; do
  [ -e "$input_file" ] || continue

  base_name=$(basename "$input_file" .rb)
  symbol_name=$(printf '%s' "$base_name" | sed 's/[^A-Za-z0-9_]/_/g')
  output_file="$output_dir/${base_name}.c"

  echo "Generate: $input_file -> $output_file"

  "$mrbc_bin" --remove-lv -B"${symbol_name}" -o "$output_file" "$input_file"
done