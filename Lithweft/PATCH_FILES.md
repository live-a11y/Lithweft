# Lithweft patch files

Use **File → New Patch / Open Patch / Save / Save As**.

| Shortcut | Action |
| --- | --- |
| ⌘N | Create a new patch |
| ⌘O | Open a `.lithweft` or historical `.maxpatch` file |
| ⌘S | Save; choose a location on the first save |
| ⇧⌘S | Save as |

A patch stores node expressions and creation arguments, positions and sizes, connections, number/slider values, message-box text, and editing/performance mode. Empty and unresolved nodes remain editable.

An asterisk in the window title indicates unsaved changes. Creating a new patch, opening another file or quitting prompts you to save when needed. A failed load leaves the current patch intact. Saving writes a temporary file before replacing the destination.

## Format and compatibility

New files default to `.lithweft`, with the JSON format identifier `Lithweft Patch` and version `1`. The reader also accepts the historical format identifier used by this prototype in `.maxpatch` files. Save As defaults to the new extension. Reading an old patch does not rewrite it; explicitly saving may update its format identifier.

The historical extension is specific to this prototype. These files are not Cycling '74 Max patch files.

The JSON contains `format`, `version`, `mode`, `nodes` and `connections`. Each node has an ID, expression, resolution status, bounds and supported UI state. Connections reference node IDs and zero-based outlet/inlet indices.

Dynamic nodes still require their matching block libraries. Patches do not embed external audio files, live audio, oscillator phase or transient module state. Unsupported versions and malformed graphs are rejected.

For a starting point, open [the arithmetic example](../Examples/arithmetic.lithweft).

The Debug diagnostic flag `--test-patch-persistence` runs the persistence checks in `Tools/Tests/PatchPersistenceTests.h`. Add `--patch-file /path/to/file.lithweft` to verify a file without modifying it.
