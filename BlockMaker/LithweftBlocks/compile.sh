#!/bin/bash
set -euo pipefail

# Each block exports the same factory, so build each as a separate library.
cd "$(dirname "$0")"
block_output="${BLOCK_OUTPUT_DIR:-$PWD}"
mkdir -p "$block_output"

compiler="$(xcrun --find clang++)"
flags=(-std=c++17 -fPIC -dynamiclib -isysroot "${SDKROOT:-$(xcrun --sdk macosx --show-sdk-path)}")
if [[ -n "${ARCHS:-}" ]]; then
    for arch in $ARCHS; do
        flags+=(-arch "$arch")
    done
fi
if [[ -n "${MACOSX_DEPLOYMENT_TARGET:-}" ]]; then
    flags+=("-mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET")
fi
if [[ "${CONFIGURATION:-Debug}" == "Release" ]]; then
    flags+=(-O2 -DNDEBUG)
else
    flags+=(-O0 -g)
fi

while read -r source library; do
    echo "Building $library.dylib"
    "$compiler" "${flags[@]}" "$source.cpp" -o "$block_output/$library.dylib"
    # The Xcode console demo can find modules beside its executable as well.
    if [[ -n "${BUILT_PRODUCTS_DIR:-}" ]]; then
        mkdir -p "$BUILT_PRODUCTS_DIR/Blocks"
        cp "$block_output/$library.dylib" "$BUILT_PRODUCTS_DIR/Blocks/$library.dylib"
    fi
done <<'BLOCKS'
MyBlock myBlock
Print print
Multiple multiple
Add add
Divide divide
Subtract subtract
Osc_tilde osc~
Dac_tilde dac~
Multiple_tilde multiple_tilde
Metro metro
Delay delay
Send send
Receive receive
Buffer buffer
Wave_Tilde wave~
BLOCKS
