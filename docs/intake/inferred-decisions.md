---
doc_type: intake-inferred-decisions
status: draft
last_updated: 2026-06-02
last_verified: 2026-06-02
confidence: medium
mode: bootstrapper
canonical: false
---

# Inferred decisions

These are reconstructed architectural/product decisions based on code and repo
artifacts. They are **not confirmed maintainer intent** unless and until they
are reviewed and promoted.

## Decision table

| Reconstructed decision | Confidence | Evidence | Why this is inferred |
| --- | --- | --- | --- |
| Use `CoreServices` as the central lifecycle/composition root for runtime services. | High | `src/coreservices.h`, `src/coreservices.cpp`, `src/main.cpp` | Construction order and explicit teardown sequencing strongly indicate intentional central orchestration. |
| Keep both legacy QWidget/skin UI and QML UI active during an ongoing migration rather than performing a flag day rewrite. | High | `src/main.cpp`, `src/qml/qmlapplication.cpp`, `CONTRIBUTING.md`, `res/qml/` | The code contains parallel startup paths and contributor docs explicitly mention an in-progress QML rewrite. |
| Use the ControlObject / ControlProxy key-based control bus as the primary inter-component signaling/state bridge. | High | `src/control/controlobject.h`, `src/control/controlproxy.h`, `AGENTS.md` | The APIs and comments explicitly position these types as cross-thread/cross-component control abstractions. |
| Persist user state locally in a per-profile SQLite DB plus config file, instead of requiring an external service. | High | `src/database/mixxxdb.cpp`, `src/main.cpp`, `src/config.h.in` | File-based storage is hard-coded into startup and DB setup. |
| Keep schema evolution in-repo and app-driven through an embedded schema history resource. | High | `src/database/mixxxdb.cpp`, `src/database/schemamanager.h`, `res/schema.xml` | The runtime explicitly upgrades schema from the embedded resource. |
| Treat controller integrations as first-class shipped assets, combining data-driven descriptors and executable JS behavior. | High | `res/controllers/`, `res/controllers/README.txt`, `src/controllers/scripting/controllerscriptenginebase.h` | The repo ships a large controller catalog and a dedicated script engine. |
| Prioritize real-time audio safety by pushing slow/allocating work away from the audio callback thread where possible. | High | `src/effects/effectsmessenger.h`, `src/soundio/sounddeviceportaudio.cpp`, `src/control/controlobject.h` | Comments explicitly justify designs in terms of avoiding audio glitches and respecting thread constraints. |
| Maintain a substantial in-repo automated test suite with a dedicated test binary and optional benchmark mode. | High | `CMakeLists.txt`, `src/test/main.cpp`, `src/test/mixxxtest.h`, `src/test/` | Build and test entrypoints make this explicit. |
| Treat multi-platform packaging/distribution as a core concern, not a downstream-only activity. | High | `.github/workflows/build.yml`, `packaging/`, platform buildenv scripts referenced in `CONTRIBUTING.md` | CI builds installers/packages across several targets directly from the repo. |
| Keep internal and external music collections distinct while synchronizing modifications through a manager layer. | Medium | `src/library/trackcollectionmanager.h`, `res/schema.xml`, `src/library/dao/trackdao.h` | The interfaces point this way, but the full operational intent was not verified through every integration implementation. |

## Potential decision clusters

### Runtime composition

Likely deliberate choices:

- centralized service composition
- deterministic init/finalize ordering
- subsystem construction in dependency order

### Product extensibility

Likely deliberate choices:

- controller mappings live in-repo
- controller scripting is part of the supported runtime surface
- UI resources and controller resources are shipped artifacts, not just test data

### Persistence strategy

Likely deliberate choices:

- schema history kept alongside application code
- per-user local state rather than server-backed state
- richer in-memory track object layered over DB-backed record structures

## Questions for validation

- Which of these are still active design decisions versus historical accident?
- Which decisions should be recorded as durable rationale in a future
  `docs/decision-log.md`?
- Are there existing ADRs, Zulip threads, wiki pages, or PRs that already own
  some of this rationale and should be linked instead of re-authored?