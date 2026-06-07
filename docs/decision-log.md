# Decision Log

Doc type: decision-log
Owner: current-agent-or-team
Status: active
Last updated: 2026-06-06
Last verified: 2026-06-06
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

---

## DL-006: Scratch Sensei Native V1

Status: active
Confidence: high

### Decision

Implement Scratch Sensei V1 as a native Mixxx feature, not as a separate app or external companion tool.

### Rationale

This keeps Scratch Sensei integrated with Mixxx's deck, analysis, library, and cue systems while keeping V1 small enough to review before implementation.

### Evidence

- `docs/superpowers/plans/scratch-sensei/spec.md`
- `docs/adr/0001-scratch-sensei-native-rebuild.md`

### Consequences

- Implementation planning starts from the V1 spec and linked ADRs.
- Scope decisions belong in the V1 spec before code is written.

---

## DL-007: Essentia Required For Scratch Sensei V1

Status: active
Confidence: high

### Decision

Scratch Sensei V1 requires Essentia for credible structure and energy suggestions.

### Rationale

Mixxx's existing analysis stack and a hand-rolled RMS/crossover analyzer are not enough for the feature promise. Essentia provides a stronger MIR toolbox for spectral, rhythmic, energy, onset, and high-level descriptors.

### Evidence

- `docs/adr/0002-scratch-sensei-required-essentia.md`

### Consequences

- `SCRATCH_SENSEI=ON` is the fork default.
- Missing Essentia fails loudly when Scratch Sensei is enabled.
- `SCRATCH_SENSEI=OFF` is only an explicit developer escape hatch.
- Essentia is the only V1 analysis backend.

---

## DL-008: Loaded-Deck Prepare Track Only In V1

Status: active
Confidence: high

### Decision

Scratch Sensei V1 only inspects a loaded deck track in a Prepare Track workspace. It does not support library-row context, Practice Skills, track pairs, lessons, export, or automatic deck control.

### Rationale

Loaded-deck context matches how the DJ uses Mixxx and avoids brittle sibling-view library selection state. Cutting lessons/export/pairs keeps V1 reviewable.

### Evidence

- `docs/adr/0003-scratch-sensei-v1-scope.md`
- `docs/superpowers/plans/scratch-sensei/spec.md`

### Consequences

- If no deck is loaded, show one empty state.
- If multiple decks are loaded and active deck is ambiguous, show a Deck 1 / Deck 2 selector.
- Practice Skills remains future reference material only.

---

## DL-009: Scratch Sensei Suggestions Stay In Analysis Metadata

Status: active
Confidence: high

### Decision

Scratch Sensei V1 stores generated suggestions in a versioned `ScratchSenseiTrackAnalysis` artifact, not in Mixxx cue rows.

### Rationale

Generated suggestions are analysis output, not user intent. Keeping them out of cue rows avoids schema churn, regeneration hazards, external export confusion, and accidental damage to user cue work. Reusing `track_analysis` / `AnalysisDao` aligns Scratch Sensei with Mixxx's existing per-track analysis cache.

### Evidence

- `docs/adr/0004-scratch-sensei-analysis-summary-only.md`

### Consequences

- Re-analysis replaces Scratch Sensei analysis metadata only.
- Re-analysis never edits or deletes user cues.
- The only cue-writing path is explicit user promotion into a normal Mixxx cue/hotcue.
- No cue `source` or `confidence` schema columns are needed in V1.
- The artifact reserves future training-platform use through stable suggestion IDs, musical anchors, descriptor timelines, evidence fields, and capability flags.
- Future descriptor expansion should evolve the serialized payload, not introduce SQL columns by default.

---
