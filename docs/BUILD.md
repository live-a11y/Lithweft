# Reproducing the archival build

The supplied Lithweft Xcode project is the primary build recipe. This cleanup keeps JUCE 7.0.4 and the existing C++ implementation.

## One build command

Install Xcode and select its developer directory. Place JUCE 7.0.4 at `.deps/JUCE`, or set `JUCE_DIR` to an existing SDK. Then run from the repository root:

```sh
bash Tools/Archive/build.sh Debug
bash Tools/Archive/check.sh
```

The build script validates the JUCE version, compiles each block separately, builds the app, bundles the modules and signs locally. It does not download dependencies, migrate JUCE, notarize the app or configure a developer account. It builds for the architecture of the current Mac; this is not a universal-binary build.

Build products and any SDK under `.deps` are ignored by Git.

## Xcode / Projucer

First install JUCE 7.0.4 in `.deps/JUCE` as described in README.md. For an existing SDK on your Mac, you can create a local link instead:

```sh
mkdir -p .deps
ln -s "/path/to/JUCE-7.0.4" .deps/JUCE
```

Do this once before opening the project. The dependency/link is ignored by Git; other users must provide their own SDK.

Open `Lithweft/Builds/MacOSX/Lithweft.xcodeproj`, select **Lithweft - App → My Mac**, and press **⌘R**. The project automatically builds and bundles all 15 block libraries. It uses ad-hoc signing, so an Apple Developer account is not required for local builds. The default app output is `Lithweft/Builds/MacOSX/build/Debug/Lithweft.app`.

`JUCE_MODULES_DIR` defaults to the checkout's `.deps/JUCE/modules`. You can override this build setting with the `modules` directory inside another JUCE 7.0.4 installation. The archive script sets it from `JUCE_DIR`.

The `.jucer` file uses repository-relative module paths. Use Projucer **7.0.4** when regenerating. The deployment target, signing settings, quoted header paths and module build/bundle hooks are stored in that file so they survive regeneration. The module hooks run `Tools/Archive/xcode-blocks.sh`.

The separate `BlockMaker/LithweftBlocks.xcodeproj` is a command-line block demo, not the GUI app. Its libraries are compiled by `compile.sh`, not linked together into the demo. The demo accepts a module directory as its first argument.

## Module lookup

Modules keep their historical names (`divide`, `osc~`, `multiple_tilde`, etc.), because those are part of the node/patch format. Changing these identifiers is not part of publication cleanup.

The app searches in this order:

1. `LITHWEFT_BLOCKS_DIR`, if set.
2. `Contents/Resources/Blocks` inside the app.
3. A `Blocks` directory beside the executable.
4. `BlockMaker/LithweftBlocks` under ancestors of the executable or working directory.

Use the environment variable to override the module directory. Example:

```sh
LITHWEFT_BLOCKS_DIR="/path/to/custom/modules" "/path/to/App.app/Contents/MacOS/Lithweft"
```

## Checks

`Tools/Archive/check.sh` checks portable lookup, path overrides and invalid module names, then runs the Debug app's patch persistence tests. The Debug diagnostic flag `--test-patch-persistence --patch-file /path/to/file.lithweft` can additionally verify a saved patch without modifying it. This flag is excluded from Release builds.

The WAV reader, timer/audio behavior and UI are still prototype implementations. These checks cover build reproduction, lookup and persistence; they do not establish production audio reliability.
