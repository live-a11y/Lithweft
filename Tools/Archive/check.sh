#!/bin/bash
set -euo pipefail
repo_dir="$(cd "$(dirname "$0")/../.." && pwd)"
temporary="$(mktemp -d "${TMPDIR:-/tmp}/archive-check.XXXXXX")"
trap 'rm -rf "$temporary"' EXIT
xcrun clang++ -std=c++17 -isysroot "$(xcrun --sdk macosx --show-sdk-path)" "$repo_dir/Tools/Tests/BlockLibraryPathsTests.cpp" -o "$temporary/path-tests"
"$temporary/path-tests"
binary="${1:-$repo_dir/build/archive/Debug/Lithweft.app/Contents/MacOS/Lithweft}"
if [[ ! -x "$binary" ]]; then
    echo "Build the Debug app first with bash Tools/Archive/build.sh Debug." >&2
    exit 1
fi
"$binary" --test-patch-persistence
