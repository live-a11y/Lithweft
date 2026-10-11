#!/bin/bash
set -euo pipefail
repo_dir="$(cd "$(dirname "$0")/../.." && pwd)"
configuration="${1:-Debug}"
case "$configuration" in Debug|Release) ;; *) echo "Usage: bash Tools/Archive/build.sh [Debug|Release]" >&2; exit 1 ;; esac

juce_dir="${JUCE_DIR:-$repo_dir/.deps/JUCE}"
header="$juce_dir/modules/juce_core/system/juce_StandardHeader.h"
if [[ ! -f "$header" ]]; then
    echo "JUCE 7.0.4 was not found. Set JUCE_DIR to an existing SDK or follow README.md." >&2
    exit 1
fi
version="$(awk '{sub(/\r$/, "")} $2 == "JUCE_MAJOR_VERSION" {a=$3} $2 == "JUCE_MINOR_VERSION" {b=$3} $2 == "JUCE_BUILDNUMBER" {c=$3} END {print a "." b "." c}' "$header")"
if [[ "$version" != "7.0.4" ]]; then
    echo "This archive was verified with JUCE 7.0.4; found $version. No SDK migration is performed." >&2
    exit 1
fi
juce_dir="$(cd "$juce_dir" && pwd)"
build_dir="$repo_dir/build/archive/$configuration"
architecture="$(uname -m)"
mkdir -p "$build_dir"

echo "Building the JUCE app ($configuration)…"
xcodebuild -project "$repo_dir/Lithweft/Builds/MacOSX/Lithweft.xcodeproj" \
    -scheme 'Lithweft - App' -configuration "$configuration" \
    -derivedDataPath "$build_dir/DerivedData" \
    "JUCE_MODULES_DIR=$juce_dir/modules" "CONFIGURATION_BUILD_DIR=$build_dir" \
    "ARCHS=$architecture" MACOSX_DEPLOYMENT_TARGET=13.0 CODE_SIGNING_ALLOWED=NO build

app="$build_dir/Lithweft.app"
codesign --force --deep --sign - "$app"
codesign --verify --deep --strict "$app"
echo "Built app: $app"
