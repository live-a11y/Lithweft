#!/bin/bash
set -euo pipefail
repo_dir="$(cd "$(dirname "$0")/../.." && pwd)"
module_build="$DERIVED_FILE_DIR/LithweftBlocks"
case "${1:-}" in
    build)
        juce_modules="${JUCE_MODULES_DIR:-$repo_dir/.deps/JUCE/modules}"
        header="$juce_modules/juce_core/system/juce_StandardHeader.h"
        if [[ ! -f "$header" ]]; then
            echo "error: JUCE 7.0.4 is missing. Follow README.md to install it in .deps/JUCE, or set JUCE_MODULES_DIR in Xcode." >&2
            exit 1
        fi
        version="$(awk '{sub(/\r$/, "")} $2 == "JUCE_MAJOR_VERSION" {a=$3} $2 == "JUCE_MINOR_VERSION" {b=$3} $2 == "JUCE_BUILDNUMBER" {c=$3} END {print a "." b "." c}' "$header")"
        if [[ "$version" != "7.0.4" ]]; then
            echo "error: Lithweft requires JUCE 7.0.4; found $version." >&2
            exit 1
        fi
        BLOCK_OUTPUT_DIR="$module_build" BUILT_PRODUCTS_DIR="" \
            /bin/bash "$repo_dir/BlockMaker/LithweftBlocks/compile.sh"
        ;;
    bundle)
        destination="$TARGET_BUILD_DIR/$UNLOCALIZED_RESOURCES_FOLDER_PATH/Blocks"
        mkdir -p "$destination"
        for library in "$module_build/"*.dylib; do cp "$library" "$destination/"; done
        ;;
    *) echo "Usage: xcode-blocks.sh build|bundle" >&2; exit 1 ;;
esac
