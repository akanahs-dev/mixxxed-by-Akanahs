---
doc_type: intake-evidence-map
status: draft
last_updated: 2026-06-02
last_verified: 2026-06-02
confidence: medium
mode: bootstrapper
canonical: false
---

# Mixxx evidence map

This map ties repository artifacts to observable project behavior. It is meant
to support review of the intake drafts, not to replace future canonical docs.

## Evidence table

| Area | Behavior / claim | Primary evidence | Supporting evidence | Confidence |
| --- | --- | --- | --- | --- |
| Application bootstrap | `mixxx` starts in `src/main.cpp`, parses args, chooses QWidget/skin or QML path, and enters the Qt event loop. | `src/main.cpp` | `CMakeLists.txt` | High |
| Composition root | `mixxx::CoreServices` initializes settings, logging, DB, effects, engine, sound, recording, players, library, controllers, and teardown order. | `src/coreservices.cpp`, `src/coreservices.h` | `src/main.cpp` | High |
| Config persistence | Preferences/config are stored in `mixxx.cfg` via `ConfigObject` / `SettingsManager`. | `src/config.h.in`, `src/main.cpp`, `src/preferences/settingsmanager.h` | `src/preferences/configobject.h` (referenced), `src/coreservices.cpp` | High |
| Library persistence | The main user library is persisted in a per-profile SQLite DB named `mixxxdb.sqlite`. | `src/database/mixxxdb.cpp` | `src/database/mixxxdb.h`, `src/database/schemamanager.h` | High |
| Schema migration | DB schema upgrades are driven by `res/schema.xml`, embedded as a Qt resource, with required schema version `40`. | `src/database/mixxxdb.cpp`, `res/schema.xml` | `src/database/schemamanager.h` | High |
| Track domain model | Track state spans DB-backed metadata plus in-memory cues, beats, waveform data, dirty/export flags, and signals. | `src/track/track.h`, `src/track/trackrecord.h` | `src/library/dao/trackdao.h` | High |
| Internal + external collections | `TrackCollectionManager` coordinates the internal collection and external collections and is responsible for synchronized modifying operations. | `src/library/trackcollectionmanager.h` | `src/library/dao/trackdao.h`, `src/coreservices.cpp` | High |
| Control bus | `ControlObject` / `ControlProxy` act as a shared control/state bus across subsystems with explicit thread-safety caveats. | `src/control/controlobject.h`, `src/control/controlproxy.h` | `AGENTS.md` | High |
| Real-time audio constraints | Audio-thread paths should avoid blocking/heap work; some operations are explicitly moved to the main thread or side channels. | `src/effects/effectsmessenger.h`, `src/soundio/sounddeviceportaudio.cpp` | `src/control/controlobject.h` | High |
| Controller architecture | Mixxx ships controller definitions and scripts in-repo and exposes a JavaScript controller engine. | `res/controllers/`, `res/controllers/README.txt`, `src/controllers/scripting/controllerscriptenginebase.h` | `CONTRIBUTING.md` | High |
| QML UI path | There is a dedicated QML application path, singleton registration, QML import/plugin loading, and `res/qml/main.qml`. | `src/qml/qmlapplication.cpp`, `res/qml/` | `src/main.cpp` | High |
| Legacy UI path | The non-QML startup path still creates `MixxxMainWindow` and uses skin loading. | `src/main.cpp` | `src/coreservices.cpp` | High |
| Build matrix | CI builds, tests, and packages Ubuntu, macOS, Windows, Android, and Flatpak targets. | `.github/workflows/build.yml` | `CONTRIBUTING.md` | High |
| Test strategy in code | The repo uses a dedicated `mixxx-test` binary, GoogleTest fixtures, and optional benchmark mode. | `CMakeLists.txt`, `src/test/main.cpp`, `src/test/mixxxtest.h` | `src/test/`, benchmark includes in `src/test/*` | High |

## Representative tests and fixtures

The test tree suggests broad subsystem coverage rather than a narrow unit-only
strategy. Examples visible during intake:

- Core/runtime: `src/test/coreservicestest.cpp`
- Schema/data: `src/test/schemamanager_test.cpp`, `src/test/trackdao_test.cpp`
- Engine/audio: `src/test/enginebuffertest.cpp`, `src/test/enginemixertest.cpp`
- Controllers: `src/test/controller_mapping_validation_test.cpp`,
  `src/test/controllerscriptenginelegacy_test.cpp`
- UI/QML: `src/test/themeqml_test.cpp`, `src/test/qmlcontrolproxytest.cpp`
- Metadata/import: `src/test/trackmetadataexport_test.cpp`,
  `src/test/itunesxmlimportertest.cpp`, `src/test/seratotagstest.cpp`

## Important code patterns worth preserving in canonical docs

- **Composition root + ordered teardown**
  - Evidence: `src/coreservices.cpp`
- **Thread-local DB connections**
  - Evidence: `src/coreservices.cpp`, references to `DbConnectionPooled`
- **Control bus indirection via control keys**
  - Evidence: `src/control/controlobject.h`, `src/control/controlproxy.h`
- **Real-time-safe messaging / callback discipline**
  - Evidence: `src/effects/effectsmessenger.h`, `src/soundio/sounddeviceportaudio.cpp`
- **Shipped resource-driven integrations**
  - Evidence: `res/controllers/`, `res/qml/`, `res/schema.xml`

## Evidence gaps

- No single existing architecture document currently owns subsystem boundaries.
- No obvious in-repo security/privacy owner was found.
- No explicit canonical operations/runbook document was found.
- Product-roadmap intent for the QML transition is not directly documented in
  the files reviewed here.