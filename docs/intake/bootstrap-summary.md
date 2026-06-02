---
doc_type: intake-bootstrap-summary
status: draft
last_updated: 2026-06-02
last_verified: 2026-06-02
confidence: medium
mode: bootstrapper
canonical: false
---

# Mixxx bootstrap summary

This file is an initial Repo Memory intake draft for the `mixxx` repository.
It reconstructs project context from repository evidence only and should be
reviewed before any content is promoted into canonical `docs/` owners.

## Repository state at intake time

- Repo Memory adoption state appears to be **Level 0 / not adopted**.
  No `docs/` ownership map or prior Repo Memory baseline was present in the
  inspected repository.
- The repository already has strong contributor-facing documents in
  `README.md`, `CONTRIBUTING.md`, `CHANGELOG.md`, and `AGENTS.md`.
- The codebase is large and multi-platform, with C++, Qt, QML, JavaScript
  controller mappings, packaging assets, and automated CI/CD workflows.

## Confirmed facts

- Mixxx is Free DJ software intended for live DJ performance on GNU/Linux,
  Windows, and macOS.
  - Evidence: `README.md`
- The project is built primarily with C++ and Qt, and uses JavaScript for
  controller mappings.
  - Evidence: `CONTRIBUTING.md`, `CMakeLists.txt`, `res/controllers/`
- The main executable entrypoint is `src/main.cpp`.
  - Evidence: `src/main.cpp`, `CMakeLists.txt`
- Runtime composition is centered around `mixxx::CoreServices`, which creates
  settings, logging, database, audio engine, players, library, controllers,
  effects, recording, and related services.
  - Evidence: `src/coreservices.h`, `src/coreservices.cpp`
- Per-user persistence uses at least two local artifacts under the settings
  path:
  - `mixxx.cfg` for config/preferences
  - `mixxxdb.sqlite` for library/database state
  - Evidence: `src/config.h.in`, `src/main.cpp`, `src/database/mixxxdb.cpp`
- The primary local database is SQLite and is upgraded from the schema history
  embedded in `res/schema.xml`.
  - Evidence: `src/database/mixxxdb.cpp`, `src/database/schemamanager.h`,
    `res/schema.xml`
- The current required schema version is `40` and the CMake project version is
  `2.7.0` with prerelease marker `alpha`.
  - Evidence: `src/database/mixxxdb.cpp`, `CMakeLists.txt`
- The repository ships a very large in-tree controller mapping library using
  XML/HID/MIDI descriptors and JavaScript scripts.
  - Evidence: `res/controllers/`, `res/controllers/README.txt`
- The repository ships a QML UI resource tree under `res/qml/`, while the
  traditional application startup path still includes `MixxxMainWindow` and
  skin loading.
  - Evidence: `src/main.cpp`, `src/qml/qmlapplication.cpp`, `res/qml/`
- CI builds and packages the project for Ubuntu, macOS (Intel + ARM), Windows
  (x64 + ARM64), Android, and Flatpak.
  - Evidence: `.github/workflows/build.yml`
- The repo contains a dedicated `mixxx-test` test binary and a large in-repo
  test suite under `src/test/` using GoogleTest, with optional benchmarks.
  - Evidence: `CMakeLists.txt`, `src/test/main.cpp`, `src/test/mixxxtest.h`,
    `src/test/`

## Inferred facts

- `CoreServices` is acting as the practical **composition root** and lifecycle
  coordinator for most runtime subsystems.
  - Confidence: high
  - Evidence: `src/coreservices.h`, `src/coreservices.cpp`
- The codebase is in a **dual-UI transition period** where the traditional
  QWidget/skin stack and newer QML stack coexist.
  - Confidence: high
  - Evidence: `src/main.cpp`, `src/qml/qmlapplication.cpp`, `CONTRIBUTING.md`
- The architecture treats controller support as a first-class product surface,
  not a plugin afterthought.
  - Confidence: high
  - Evidence: `res/controllers/`, `src/controllers/`, `CONTRIBUTING.md`
- Audio-path code is designed with real-time constraints in mind and avoids
  blocking/allocating work in the audio callback where possible.
  - Confidence: high
  - Evidence: `src/control/controlobject.h`, `src/effects/effectsmessenger.h`,
    `src/soundio/sounddeviceportaudio.cpp`
- Library persistence spans both an internal collection and separate structures
  for external collections/integrations.
  - Confidence: medium
  - Evidence: `src/library/trackcollectionmanager.h`, `res/schema.xml`,
    `src/library/dao/trackdao.h`

## High-value evidence anchors

These files were the most useful anchors for reconstructing the repo:

- `README.md`
- `CONTRIBUTING.md`
- `CMakeLists.txt`
- `src/main.cpp`
- `src/coreservices.h`
- `src/coreservices.cpp`
- `src/control/controlproxy.h`
- `src/control/controlobject.h`
- `src/database/mixxxdb.cpp`
- `src/database/schemamanager.h`
- `src/library/trackcollectionmanager.h`
- `src/library/dao/trackdao.h`
- `src/track/track.h`
- `src/track/trackrecord.h`
- `src/qml/qmlapplication.cpp`
- `res/schema.xml`
- `.github/workflows/build.yml`

## Main unknowns to confirm with maintainers

- Whether the future canonical overview should remain in `README.md` or move to
  `docs/project-overview.md`.
- The long-term plan for legacy QWidget skins versus QML UI.
- Which external library integrations are strategic versus legacy maintenance.
- Desired canonical owner for security/privacy posture (none is obvious today).
- Desired canonical owner for operational/runbook knowledge (packaging and CI
  are present, but there is no single mapped runbook).

## Suggested next step

Promote an ownership map first (`docs/README.md`) and explicitly decide which
existing documents should remain canonical owners versus which new Repo Memory
docs should be introduced.