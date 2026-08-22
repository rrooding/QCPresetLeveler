# Changelog

All notable changes to this project are documented in this file. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/); versioning follows
[Semantic Versioning](https://semver.org/).

## [0.1.0] - 2026-08-22

Initial end-to-end proof of concept: audio/MIDI device connection, preset/scene navigation,
a synthetic reference signal, real-time level measurement, and a raise/lower instruction
readout, for a single dynamically-addable preset column.

### Added

- CMake + JUCE project skeleton (C++23, header-only, GPLv3)
- Audio device selection panel (#5)
- MIDI input/output port selection panel (#6)
- Send Program Change + Bank Select to switch QC presets (#9)
- Send CC#43 to switch scenes A-H (#10)
- Duplex audio engine: playback + capture (#15)
- Synthetic pink noise reference signal generator, wired into the audio engine (#41)
- Level measurement: peak + RMS in dBFS with attack/release ballistics (#16)
- Input channel pair selector mapped to the QC's USB channel layout (#17)
- Real-time VU meter component (#18)
- Preset column component: name, dB readout, VU meter, scene buttons A-H (#19)
- Add/remove preset columns dynamically (#21)
- Target level + raise/lower instruction readout (#20)

### Fixed

- Relaxed an overly strict commitlint subject-case rule that flagged legitimate acronyms
- Tolerated unknown clang-tidy warning options on Linux's GCC-flavored compile_commands.json
- Disabled libcurl in the Tests target to fix a Linux link failure
- Requested microphone permission on macOS so audio input actually works

### Build & CI

- Warnings-as-errors, clang-tidy, Catch2/CTest, an 85% coverage gate, and ASan+UBSan
  sanitizers (#44)
- GitHub Actions workflow enforcing format, static analysis, tests, coverage, and commit
  message style, with superseded runs auto-cancelled
- clang-format adopted project-wide

### Docs

- Coding standards established and iterated on throughout: real-time audio thread safety,
  ownership and references-vs-pointers preference, testing scope, and documented static
  analysis exemptions

[0.1.0]: https://github.com/rrooding/QCPresetLeveler/releases/tag/v0.1.0
