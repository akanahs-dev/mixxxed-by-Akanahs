# Decision Log

Doc type: decision-log
Owner: current-agent-or-team
Status: active
Last updated: 2026-06-02
Last verified: 2026-06-02
Verified against: docs/intake/inferred-decisions.md
Confidence: high
Canonical source: `docs/decision-log.md`
Related docs: `README.md`, `architecture.md`, `data-model.md`
Decision ID format: DL-###
Confidence rule: Defined based on codebase evidence availability
Supersession rule: New decisions replace older entries with status `superseded`

This document records the foundational product and architectural decisions of the Mixxx codebase, adopted and confirmed for this disconnected fork.

---

## DL-001: Centralized Service Orchestration via `CoreServices`

Status: active
Confidence: high

### Decision
All major application subsystems (settings, database connection pools, mixing engine, sound interface routing, tracks collection, library database mapping, and controller scripts) are constructed, wired, and finalized via a single central class: `mixxx::CoreServices`.

### Rationale
Provides a deterministic, sequential setup and teardown sequence. This avoids segmentation faults, thread-racing during startup, and uninitialized references across independent components.

### Evidence
- `src/coreservices.h`
- `src/coreservices.cpp`

### Consequences
- Adding a major runtime service requires writing `initialize()` and `finalize()` bindings inside `CoreServices`.
- Services must not attempt to bypass `CoreServices` for application-level lifecycle events.

---

## DL-002: Parallel Legacies and Modern UI Stack (Coexistence Path)

Status: active
Confidence: high

### Decision
Both the legacy skin layout engine (QWidget-based `MixxxMainWindow`) and the modern QML presentation layout engine (`QmlApplication`) are kept active. The application will choose its UI startup path at boot time.

### Rationale
Avoids a risky "flag day" rewrite by allowing features to be ported to QML progressively while maintaining a stable fallback UI.

### Evidence
- Parallel initialization branches inside `src/main.cpp`.
- Coexistence of `src/qml/qmlapplication.cpp` and legacy widgets in `src/widget/`.

### Consequences
- Features modifying user interfaces must consider and verify behavior under both skin engines if target surfaces overlap.
- Subsystem signals must bind cleanly to both QWidget hooks and QML property wrappers.

---

## DL-003: Control Object Bus for Inter-Component Signaling

Status: active
Confidence: high

### Decision
Subsystems communicate control state asynchronously using key-addressed controls (`[Group], key_name`) routed via the thread-safe `ControlObject` / `ControlProxy` bus.

### Rationale
Decouples UI drawing, Javascript controller parsing, and real-time audio callback sequences. Ensures parameters can be updated from any thread safely without lock contention.

### Evidence
- `src/control/controlobject.h`
- `src/control/controlproxy.h`
- Instruction annotations in `AGENTS.md`.

### Consequences
- Components must not expose raw mutable variables directly to other threads.
- Parameters must register unique control keys to participate in the control loop.

---

## DL-004: In-Repo Schema Version Control

Status: active
Confidence: high

### Decision
SQL library database migration steps and tables are defined in-repo using the XML descriptor `res/schema.xml` compiled as an in-app Qt resource (`:/schema.xml`).

### Rationale
Guarantees schema integrity by packaging migration history directly inside the application binary, preventing out-of-sync queries.

### Evidence
- `src/database/mixxxdb.cpp`
- `res/schema.xml`

### Consequences
- Modifying SQLite structure requires updating `res/schema.xml` and bumping the required database schema version.
- SchemaManager automates upgrade migrations on startup based on the parsed XML history.

---

## DL-005: Strict Real-Time Audio Constraints

Status: active
Confidence: high

### Decision
The PortAudio callback sequence running on the audio thread is treated as real-time constrained. It must avoid blocking actions (e.g. file operations, Qt event scheduling) and heap allocations/deallocations.

### Rationale
Prevents buffer underruns, clicks, and dropouts in live DJ environments.

### Evidence
- `src/effects/effectsmessenger.h` (deferring allocating tasks to the main loop).
- Real-time priority adjustments in PortAudio setup.

### Consequences
- Allocating operations in the audio signal path must be moved to queues or processed on helper threads.
- Mutexes must not be held in the audio callback.
