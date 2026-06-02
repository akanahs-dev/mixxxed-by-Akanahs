# Testing Strategy

Doc type: testing-strategy
Owner: current-agent-or-team
Status: active
Last updated: 2026-06-02
Last verified: 2026-06-02
Verified against: src/test/main.cpp, CMakeLists.txt
Confidence: high
Canonical source: `docs/testing-strategy.md`
Related docs: `README.md`, `architecture.md`, `decision-log.md`

This document defines the automated test infrastructure, fixture classifications, and manual verification procedures used in the Mixxx project.

---

## Automated Test Infrastructure

Automated tests are compiled into a standalone test executable target named `mixxx-test`.

### 1. Toolchain
- **Framework**: GoogleTest (GTest) and GoogleMock (GMock) for unit and integration testing.
- **Build configuration**: Governed by `CMakeLists.txt` (defines the `mixxx-test` target and sources).
- **CI integration**: Executed automatically on every pull request or commit via GitHub Actions (`.github/workflows/build.yml`).

### 2. Execution Command
From the build directory:
```bash
./mixxx-test
```
You can run specific test suites using GTest filtering:
```bash
./mixxx-test --gtest_filter=CoreServicesTest.*
```

---

## Subsystem Test Classifications

Automated test files live under the `src/test/` directory. They are organized to mimic the runtime service graph:

### 1. Core Services and Setup
Verifies the ordered construction, initialization, and teardown behavior of `CoreServices`.
- **Primary file**: `src/test/coreservicestest.cpp`

### 2. Persistence and Database
Verifies the SQLite migration engine and DAO execution paths.
- **Primary files**:
  - `src/test/schemamanager_test.cpp` (validates upgrading schema versions to schema 40)
  - `src/test/trackdao_test.cpp` (verifies track read/write statements, location queries)

### 3. Audio Engine and Processing
Validates audio DSP logic, channel mixing, and deck buffering.
- **Primary files**:
  - `src/test/enginebuffertest.cpp`
  - `src/test/enginemixertest.cpp`
- **Audio Benchmarks**: Optional performance benchmark tests exist in this space to test DSP regression performance under heavy workloads.

### 4. Controller Scripting and Mappings
Validates the loading of controller XML descriptors, execution of Javascript callbacks, and parsing of MIDI/HID interfaces.
- **Primary files**:
  - `src/test/controller_mapping_validation_test.cpp`
  - `src/test/controllerscriptenginelegacy_test.cpp`

### 5. UI and Presentation Layers
Verifies QML plugins, property linkages, and skin stylesheet parses.
- **Primary files**:
  - `src/test/themeqml_test.cpp`
  - `src/test/qmlcontrolproxytest.cpp`

### 6. Metadata Importers and Exporters
Tests tag reads/writes (Serato, iTunes, Traktor) and ID3 synchronization.
- **Primary files**:
  - `src/test/trackmetadataexport_test.cpp`
  - `src/test/itunesxmlimportertest.cpp`
  - `src/test/seratotagstest.cpp`

---

## Manual Verification Guidelines

Because Mixxx is an interactive audio application involving physical controller hardware, automated testing cannot cover every interaction gap. Contributors must perform manual verification for:

### 1. Audio Interface Output
- Configure different sample rates, buffer sizes, and API backends (e.g., PortAudio, CoreAudio, ASIO, ALSA).
- Listen to master and headphone cue paths for underruns, clicks, or signal degradation.

### 2. Controller Mapping Inspections
- Connect the target hardware controller.
- Map and test essential controls (faders, jog wheels, loops, hotcues).
- Check JS scripting console outputs to verify message parsing does not emit warnings.

### 3. UI/Skins Auditing
- Load legacy skins (varying resolutions) and QML screens.
- Verify elements remain aligned, scaling is correct across high-DPI outputs, and interactions execute cleanly.
