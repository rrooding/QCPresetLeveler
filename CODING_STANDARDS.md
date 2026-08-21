# Coding Standards

These are binding for all contributions unless a PR discussion explicitly revisits one.

## Language

- **C++23**, no exceptions. All three CI platforms must build with a C++23-conformant
  compiler (GCC 13+, Clang 16+, MSVC 19.36+). If JUCE's own headers turn out not to fully
  support C++23 cleanly, that gets resolved as part of [#1](https://github.com/rrooding/QCPresetLeveler/issues/1)
  rather than by quietly downgrading the standard.
- Prefer standard-library facilities over JUCE equivalents when both exist and neither has a
  real advantage (e.g. `std::optional` over a sentinel value), but use JUCE's types when
  interoperating with JUCE APIs (`juce::String` at UI boundaries, etc.) rather than converting
  back and forth needlessly.
- Use `std::expected<T, E>` for fallible operations outside the audio thread (MIDI parsing,
  device enumeration, session file I/O). Reserve exceptions for truly exceptional,
  non-real-time paths (e.g. malformed config file), and catch them close to the call site —
  never let one cross into audio-thread code.

## Real-time audio thread safety (non-negotiable)

Anything running on the audio callback (`processBlock` / equivalent) must not:

- allocate or free memory (no `new`, no container growth that can reallocate, no
  `std::string`/`juce::String` construction)
- take a lock or mutex
- log, print, or otherwise do blocking I/O
- throw or catch exceptions

Communication between the audio thread and the message/UI thread goes through lock-free
structures (`std::atomic`, `juce::AbstractFifo`, or a single-producer/single-consumer ring
buffer) — never a mutex-guarded shared object. Any PR touching the audio path should call
out in its description how thread-safety was preserved.

## Ownership & memory

- RAII everywhere. No raw owning pointers — `std::unique_ptr` for exclusive ownership,
  `juce::ReferenceCountedObjectPtr` only where a JUCE API demands reference counting.
- No `new`/`delete` directly; use `std::make_unique` or JUCE's equivalent factory helpers.
- Pass non-trivial types by `const&` unless ownership is actually transferred (then take by
  value and `std::move`).

## Naming & structure

- Types/classes: `PascalCase`. Functions, methods, variables: `camelCase`. Private/protected
  member variables: trailing underscore (`levelDb_`). *(This departs from JUCE's own
  no-prefix convention — flag it if you'd rather match JUCE's house style instead.)*
- One class per file where reasonable. Project namespace: `leveler`. Avoid globals and
  singletons except where a JUCE API leaves no choice (e.g. `JUCEApplication`); prefer
  passing dependencies in explicitly — it's what makes the logic testable in the first place.
- Small functions, single responsibility, early returns over nested conditionals,
  const-correct by default.

## Header-only implementation

- Implementation code lives entirely in `.hpp` files — `Thing.hpp`, no paired `Thing.cpp`
  (`.hpp`, not `.h` — that's the one file-extension convention this project deviates from
  JUCE's own headers on). Define methods in-class (implicitly `inline`) or out-of-class
  marked `inline` explicitly; free functions at namespace scope must be `inline` too, to
  avoid ODR violations across translation units.
- `#pragma once` on every header (not verbose include guards).
- `.cpp` files exist only where the toolchain genuinely requires one: the JUCE application
  entry point (`Main.cpp`, for `START_JUCE_APPLICATION`) and Catch2 test binaries.
- **Known tradeoff, flagged rather than silently absorbed:** header-only means every
  translation unit that includes a changed header gets recompiled, which will make full and
  incremental builds slower than a `.h`/`.cpp` split as the codebase grows — noticeable on
  top of JUCE's already-substantial build times across a 3-platform CI matrix. If that
  becomes painful, the mitigation (precompiled headers or a unity/jumbo build, not reverting
  to split files) is worth revisiting in the build epic rather than assumed now.

## Comments & documentation

- No comments that restate what the code does — names should already say that.
- A comment is warranted only for the non-obvious *why*: a hidden constraint, a protocol
  quirk (e.g. "CC#43 value 0-7 selects Scene A-H per the CorOS MIDI spec"), a workaround for
  a specific bug.
- No mandatory Doxygen blocks on every function. Brief header-level summaries are fine on
  genuine module boundaries (e.g. the MIDI protocol layer) where a reader can't infer intent
  from the file alone.

## Formatting

- `.clang-format`, LLVM base style, 4-space indent, ~110 column limit (exact width settled
  when the config lands in [#3](https://github.com/rrooding/QCPresetLeveler/issues/3)).
- Enforced in CI — a PR that isn't clang-format-clean fails the build, no manual style
  nitpicking in review.

## Static analysis

- `clang-tidy` enabled in CI (see `.clang-tidy`, run via `scripts/run-clang-tidy.sh`) with
  `cppcoreguidelines-*`, `performance-*`, `bugprone-*`, and `modernize-*` check groups.
  Warnings are errors.
- Suppressions require a `// NOLINT(check-name): reason` comment — a bare `NOLINT` doesn't
  pass review.
- Four checks are disabled project-wide, decided by actually running the ruleset against
  real code in #44 rather than guessing upfront:
  - `cppcoreguidelines-avoid-magic-numbers` / `readability-magic-numbers` — too noisy against
    UI layout code (`setSize(900, 600)` and friends).
  - `modernize-use-trailing-return-type` — a style preference, not a real modernization; not
    worth enforcing.
  - `cppcoreguidelines-special-member-functions` — flags every class using JUCE's
    `JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR` for not also declaring a destructor/move
    ops. A user-declared (even deleted) copy constructor already implicitly deletes the move
    operations per the language rules, so there's no actual behavioral gap — only a
    boilerplate tax on nearly every JUCE-derived class in this codebase.
  - `cppcoreguidelines-owning-memory` — assumes `gsl::owner<>` annotations that JUCE's own API
    doesn't use anywhere (`setContentOwned(new X(), true)`, the `START_JUCE_APPLICATION`
    macro). Enforcing it would mean `NOLINT`-ing essentially every JUCE interaction point.
  - `bugprone-pointer-arithmetic-on-polymorphic-object` — crashes clang-tidy itself when
    analyzing one of JUCE's own headers (`juce_Ranges.h`); a toolchain bug, not a finding.
- On macOS, `clang-tidy` (Homebrew/LLVM) doesn't resolve the SDK sysroot the way AppleClang
  does implicitly — `scripts/run-clang-tidy.sh` adds `-isysroot` explicitly via `xcrun` so
  this doesn't need rediscovering locally.

## Testing

- **Catch2**, via CMake `FetchContent`, run through CTest.
- Test files mirror the source tree (`src/Foo/Bar.hpp` → `tests/Foo/BarTests.cpp`) — tests are
  the one place `.cpp` files are expected, since Catch2 test binaries need a compiled entry
  point.
- Anything with real logic — MIDI message construction, level/dBFS calculation, session
  state, the leveling target/tolerance logic — gets unit tests as part of the same PR that
  introduces it, not a follow-up.
- Audio-thread and DSP code is tested by decoupling the algorithm from
  `AudioDeviceManager`/JUCE's callback plumbing: pure functions/classes that take buffers in
  and produce results out, exercised directly in tests, with the JUCE audio callback reduced
  to a thin adapter around them.
- **Coverage is gated in CI** at **85%** minimum, measured via `llvm-cov`/`gcovr`, scoped to
  non-UI code. `juce::Component` painting/layout code is excluded from the gate — pixel-level
  GUI testing isn't practical — but any *logic* embedded in a component (event handling,
  state transitions) should be extracted so it's testable rather than left untested inside
  `paint()`/`resized()`.
- Sanitizers (ASan + UBSan) run in CI on at least one platform's test suite.

## Build

- Warnings-as-errors on all platforms: `-Wall -Wextra -Wpedantic -Werror` (GCC/Clang),
  `/W4 /WX` (MSVC).

## Commit messages

[Conventional Commits](https://www.conventionalcommits.org/), enforced in CI — this is what
lets a changelog get generated automatically later instead of hand-written.

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

- **Type** is one of: `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `build`,
  `ci`, `chore`, `revert`.
- **Scope** is optional but encouraged, and should match the area labels used on issues:
  `build`, `connectivity`, `midi`, `audio`, `ui`, `session`, `platform`, `docs` — e.g.
  `feat(midi): send CC#43 for scene switching`. Keeps commit history and the eventual
  changelog organized along the same lines as the roadmap.
- **Breaking changes**: mark with `!` after the type/scope (`feat(midi)!: ...`) and/or a
  `BREAKING CHANGE:` footer explaining the impact.
- Reference the story issue a commit addresses in the footer where it applies (`Refs #9`,
  `Closes #9`).
- Description is imperative mood, lower case, no trailing period (`add`, not `Added`/`adds`).

## Open questions to flag rather than assume

- Exact clang-format column width and brace style details — settled when #3 is picked up.
- Whether `cppcoreguidelines-owning-memory`/similar stricter guideline-checking modes are
  worth the friction once real code exists — revisit after a few PRs.
