# AGENTS.md — Lithweft development handoff

This file provides repository-specific instructions for coding agents and developers continuing **Lithweft**. Read `README.md`, `docs/ARCHIVE.md`, `docs/BUILD.md`, `Lithweft/PATCH_FILES.md`, and `THIRD_PARTY_NOTICES.md` before making significant changes.

## Quick rules

- **Archive preservation is the default.** Treat Lithweft as a historical experimental source archive, not an active product roadmap.
- **Confirm scope before editing.** Work only on the requested files and behavior; propose substantial changes before implementing them.
- **Do not silently upgrade JUCE 7.0.4**, replace the GUI/build system, or alter third-party licensing.
- **Preserve patch and module compatibility.** Keep `.lithweft` version-1 semantics, historical reader support, and existing module names unless a breaking change is explicitly approved.
- **Verify and report honestly.** List tests actually run; never commit, tag, push, or publish without explicit authorization.

## 1. Project identity and current status

Lithweft is an **experimental Max/MSP-like visual programming platform for audio**, implemented in **C++17 with JUCE 7.0.4**. Its core model is a patch: visual nodes with message and audio-signal connections, interactive UI controls, and dynamically loaded block libraries. It is an architectural exploration, **not** a feature-complete or file-compatible reimplementation of Cycling '74 Max.

The experiment began in late 2023; further work followed in early 2024. In October 2026, it was prepared for source archival publication: macOS builds were restored, patch saving/loading was added, module aliases and paths were corrected, and the project was renamed Lithweft. The published source is **not an untouched 2023 snapshot**. Preserve the historical provenance and original source-date comments.

**Repository policy:** this is a source archive. The author does not promise maintenance, support, issue triage, or PR review. Do not present an experimental subsystem as production-ready.

## 2. Operating modes — determine scope before editing

**Archive-preservation mode (default for this repository):** Work only on a clearly requested, bounded fix, reproduction issue, documentation correction, or test. Preserve existing behavior and dependencies. Prefer a narrow change with a reproducible test. Do **not** initiate a redesign, migration, mass formatting, or roadmap on your own.

**Continuation mode (only when explicitly requested, ideally in a fork or feature branch):** New features and architectural changes are permitted, but first propose their expected impact on patch files, DSP/message semantics, dynamic module interfaces, and the user experience. Obtain approval before breaking compatibility or changing major dependencies.

Never modify unrelated projects or files outside this repository. Confirm the repository root before running scripts or any command that writes, removes, resets, or stages files. Do not use destructive Git commands (`reset --hard`, `clean -fdx`, forced pushes) without explicit approval.

## 3. Repository map

| Path | Responsibility |
| --- | --- |
| `Lithweft/Source/Main.cpp` | JUCE application and main window startup. |
| `Lithweft/Source/MainComponent.*` | Main application component, menus, audio device integration/callback entry points. |
| `Lithweft/Source/PatchController.*` | Patch canvas/UI interaction plus orchestration of node instances and connections; **UI and engine responsibilities are currently intertwined**. |
| `Lithweft/Source/BlockComponent.*`, `PortComponent.h`, `LineComponent.h` | Node widgets, input/output ports, and visible connections. |
| `Lithweft/Source/NumberBox.*`, `Slider.h`, `Scope.h`, `Trigger.h`, `ParameterBox.h` | Interactive patch UI elements. |
| `Lithweft/Source/BlockChain.*`, `BufferPool.*`, `BlockInterface.h` | Host-side block graph, audio processing, and data/interface definitions. |
| `Lithweft/Source/PatchDocument.h`, `PatchPersistence.cpp` | Patch-document integration, serialization, validation, and restoration. |
| `BlockMaker/LithweftBlocks/` | Dynamically loaded block implementations, compilation script, shared interfaces, and module path lookup. |
| `Lithweft/Builds/MacOSX/`, `Lithweft/JuceLibraryCode/`, `Lithweft/Lithweft.jucer` | Xcode/Projucer project, required resource, and JUCE include wrappers. |
| `Tools/Archive/`, `Tools/Tests/` | Reproducible build scripts and focused regression checks. |
| `Examples/arithmetic.lithweft` | Minimal, audio-free introductory patch. |
| `docs/screen-recording/lithweft_demo.gif` | README demonstration animation; not a reason to include local raw recordings or still images in a commit. |

The application and block-library trees contain related and sometimes duplicated core interface/graph definitions. **Inspect both copies and their actual usage before changing shared data types or contracts.** Do not assume a change in one copy propagates automatically to the other.

## 4. Contracts and compatibility to preserve

- **Patch format:** New documents use `.lithweft` and the JSON identifier `"Lithweft Patch"` (version 1). The loader also recognizes the historical identifier `"Max-liked Patch"` in files produced by this prototype. Do **not** remove historical reader support simply to complete the rename. A legacy `.maxpatch` suffix does **not** imply compatibility with Cycling '74 Max files. Review `Lithweft/PATCH_FILES.md`.
- **Patch round-tripping:** Node expressions and operator aliases, node bounds, supported UI values, connections, and unresolved/editable nodes must survive save/load where supported. The loader validates a staged graph before replacing the active patch. Preserve this failure-atomic behavior.
- **Block naming:** Operator aliases (e.g. `/`) and module identifiers (e.g. `osc~`, `dac~`, `multiple_tilde`) are user-facing patch contracts. A module rename requires compatibility handling and tests.
- **Dynamic module model:** The prototype loads local `.dylib` blocks; inspect `BlockLibraryPaths.h`, the factory/loader in `PatchController.cpp`, and `BlockInterface.h` before modifying lifecycle, entry points, ownership, or symbol names. The interface uses C++ types such as `std::variant` and `std::vector<double>`; do **not** assume a stable cross-toolchain binary ABI.
- **Audio processing:** Changes must distinguish message/control operations from audio-callback execution. Avoid introducing allocations, blocking locks, filesystem access, logging, or expensive graph changes into the real-time path without an explicit, measured design. Existing real-time behavior remains experimental; do not claim it is safe merely because it compiles.
- **Licenses:** Preserve `LICENSE`, `THIRD_PARTY_NOTICES.md`, and third-party notices. The project's original code is described in the repository as AGPL-3.0-only. **JUCE 7.0.4 is not thereby relicensed under AGPL:** its licensing provides a GPLv3 open-source route, ISC licensing for `juce_core`, `juce_audio_devices`, `juce_audio_basics`, and `juce_events`, and commercial terms. GPLv3/AGPLv3 section 13 permits combining covered works while leaving each part's original obligations in place. Follow the pinned-version notices; do not silently change licenses or upgrade JUCE.
- **Portability:** Never add absolute developer-machine paths, assumed `/Users/...` locations, or hardcoded SDK paths. Runtime module lookup and build-time JUCE lookup are different concerns.

## 5. Build and verification baseline

The archive pins **JUCE 7.0.4** and uses **C++17** in its documented build workflow. For the historically verified macOS/Xcode environment, deployment target, platform limitations, and test results, consult `README.md`, `docs/BUILD.md`, and `docs/ARCHIVE.md` rather than copying version numbers from this instruction file. The source archive does **not** bundle the JUCE SDK.

From the **Lithweft repository root**:

```sh
mkdir -p .deps
git clone --depth 1 --branch 7.0.4 https://github.com/juce-framework/JUCE.git .deps/JUCE
bash Tools/Archive/build.sh Debug
bash Tools/Archive/check.sh
bash Tools/Archive/build.sh Release
```

To use an existing JUCE 7.0.4 installation with the archive build script, set `JUCE_DIR="/path/to/JUCE-7.0.4"`. The direct Xcode project has a separate documented dependency setup; read `docs/BUILD.md` rather than assuming `JUCE_DIR` applies to every build route.

The focused checks exercise module path lookup and patch persistence; the exact recorded counts and outcomes live in `docs/ARCHIVE.md`. They are **not** comprehensive DSP, threading, fuzzing, device, UI, compatibility, or production-reliability tests. For changes to patch loading, add positive and malformed-input cases. For new audio features, propose separate audio tests and state what was actually measured.

After any code edit, report which commands **actually ran** and their outcomes; distinguish direct test results from historical reports. If unable to run Xcode/macOS, say so rather than implying success. Avoid staging build products, `.deps`, generated scans, local SDKs, or unrelated media such as raw `.mov` recordings and still images.

## 6. Known engineering risks; treat as investigation topics, not solved facts

1. **Graph/UI coupling:** `PatchController` mixes canvas behavior with block creation, connection management, and runtime orchestration. A future separation of document, graph/runtime, and canvas could help—but is a substantial, opt-in redesign, not an automatic cleanup.
2. **Real-time correctness:** Review buffer ownership, copies/allocations, synchronization, sample rates, scheduling, and callback lifetime in `BlockChain`, block implementations, and the audio host. Avoid changing behavior without an executable regression plan.
3. **Dynamic library ownership and ABI:** Audit factory symbols, destruction, `dlopen`/`dlclose` order, module versioning, and compiler compatibility before extending the external block API.
4. **WAV and wavetable boundaries:** Inspect empty buffers, sample indices, malformed files, and `wave~` behavior before claiming audio-file nodes are reliable.
5. **Persistence evolution:** Version any future format changes; keep version-1 and historical prototype fixtures readable wherever feasible; never imply Max patch-file compatibility without a converter and tests.
6. **Timing and thread lifetime:** The timer/message behavior is experimental. Validate shutdown, graph mutation, and race conditions before making real-time guarantees.

These are **candidate investigation areas**, not promises of maintenance or a prioritized roadmap.

## 7. Workflow for an AI coding agent

**Before changes:**

1. Confirm the exact user request and whether the work belongs in archive-preservation or continuation mode.
2. Read the relevant implementation **and** tests; identify existing patch/module contracts and both sides of any duplicated interfaces.
3. Give a compact plan naming affected files, potential breaking changes, and how success will be verified. Ask before changing JUCE version, build system, platform scope, public APIs, licenses, or serialized format.

**During changes:**

4. Make the smallest change that satisfies the approved scope. Preserve provenance comments unless technically necessary to change them.
5. Include a regression test or minimal reproduction when practicable. Do not convert historical files to a new format as a side effect.
6. Keep changes confined to this repository and the approved branch; do not touch the user's other projects.

**When delivering:**

7. Summarize behavior changed, files touched, compatibility consequences, and remaining limitations.
8. State exact builds/tests run and results; say explicitly which tests could not be run.
9. Do not commit, tag, push, publish releases, or create external issues/PRs unless the user requests that action.

## 8. Completion report template

```text
Goal:
Mode: archive preservation / continuation
Files changed:
Behavior and compatibility:
Builds/tests actually run:
Known limitations / unverified areas:
Further work (optional, not automatically authorized):
```

This guidance helps a future contributor or AI agent work with Lithweft's **actual experimental Max/MSP-like architecture**, rather than treating it as an already mature generic audio product.
