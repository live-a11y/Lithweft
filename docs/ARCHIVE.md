# Archival publication scope

Lithweft is intended as a source archive of an experiment begun in 2023, with preparation changes made in 2026. It does not promise continued development, maintenance or pull-request review.

## Included

- `Lithweft/Source`: GUI prototype source.
- Projucer/Xcode projects, Info plist and required menu resource.
- `JuceLibraryCode`: small module include wrappers and project configuration, without the JUCE framework.
- `BlockMaker/LithweftBlocks`: dynamic block source, compile script and console demo.
- `Tools/Archive` and `Tools/Tests`: build scripts and focused checks.
- `Examples`: a small patch that can be opened without external audio.
- README, LICENSE, third-party notices and documentation.

## Excluded

Git ignores build products, apps, dynamic libraries, downloaded SDKs, Xcode user state, editor files, environment files and generated source bundles. The separate Code Scanner utility, original scanning script, old notes/backups and personal media are outside this archive. Ignoring these files does not delete local copies.

## Preparation changes

The original C++ implementation and JUCE 7.0.4 are retained. Preparation restored builds, added patch persistence, fixed operator-alias loading, removed developer-specific module/audio paths, bundled modules in the app, removed personal signing settings, and unified names under Lithweft. Public documentation and patch dialogs/errors use English. Historical source comments and dates are retained.

## Verification

On 2026-10-11, source-only copies were built in a fresh directory with spaces. After naming the project Lithweft, Debug, Release and the console project compiled successfully. Nine module-path checks and 70 persistence checks passed. Persistence checks also passed after copying the app away from its source tree. An existing historical patch was verified without modifying it. After the English documentation and dialogs were prepared, Debug and Release were rebuilt; the 9 path and 70 persistence checks passed again. The arithmetic example passed file verification, and editing its input from 42 to 50 in the GUI updated the output from 44 to 52.

Direct Xcode builds were also corrected and verified: after providing JUCE at the default dependency path, Cmd-R builds and bundles all modules automatically. The build settings and module hooks are retained in the Projucer project. The direct Debug app passed the same 9 path and 70 persistence checks, and the Release archive script passed with the regenerated project.

See the README for the tested environment and limitations. These checks establish build reproduction and patch persistence, not production audio reliability.

## Publication steps

1. Review the source manifest and create the initial commit with its actual date.
2. Create the public `Lithweft` repository and push the source.
3. Tag `v0.1-archive` and create a source-only release using [the release notes](RELEASE.md).
4. Choose whether to disable Issues and mark the repository as read-only on GitHub. Forks remain the intended route for continued work.

Suggested GitHub description:

> An archived node-based audio prototype built with C++ and JUCE through conversations with ChatGPT, begun in 2023.

No binary installer, additional platform, support schedule or PR response commitment is planned.
