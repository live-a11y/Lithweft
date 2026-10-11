# Third-party licenses

Checked on **2026-10-11**. This document records the dependencies used for this archival build; it does not relicense them.

## JUCE 7.0.4

The project was restored and verified against **JUCE 7.0.4**, tag commit `4e68af7fde8a0a64e808fb1a37999a690873c9de`. JUCE is not vendored into this repository.

- [Official JUCE 7.0.4 license file](https://github.com/juce-framework/JUCE/blob/7.0.4/LICENSE.md)
- [Official JUCE 7 license terms](https://juce.com/legal/juce-7-license/)
- Module headers identify the GPLv3 open-source option. `juce_audio_basics`, `juce_audio_devices`, `juce_core` and `juce_events` are ISC-licensed in this version.
- JUCE's own bundled third-party libraries retain their respective notices; keep those notices with any JUCE source or binary redistribution.

The project-specific `JuceLibraryCode` include wrappers and configuration header are retained to reproduce the existing Xcode build. They include externally obtained JUCE modules; they are not a copy of the JUCE framework.

## Project code and AGPL

Copyright (c) 2023–2026 Liu Hao, for author-written project code. That code is available under AGPL-3.0-only as stated in README.md and LICENSE. Original third-party notices remain authoritative for third-party code.

GPLv3 and AGPLv3 section 13 provide permission to link or combine code under those licenses; this does not change the license of JUCE's covered portions. See the [GNU license compatibility explanation](https://www.gnu.org/licenses/license-compatibility.en.html) and [GPLv3 text](https://www.gnu.org/licenses/gpl-3.0.html).

For another JUCE version, consult the [official JUCE repository](https://github.com/juce-framework/JUCE) and the license shipped with that version. A newer license policy is not substituted for the pinned 7.0.4 license. A future fork changing JUCE versions must review the chosen version's license and dependencies.
