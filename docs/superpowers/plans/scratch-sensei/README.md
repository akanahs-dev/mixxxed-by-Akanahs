# Scratch Sensei Plan Folder

Doc type: plan-index
Status: active
Owner: current-agent-or-team
Last updated: 2026-06-07
Last verified: 2026-06-07
Verified against: folder contents
Confidence: high
Canonical source: `docs/superpowers/plans/scratch-sensei/README.md`
Related docs: [`Scratch Sensei feature`](../../../features/scratch-sensei.md), [`spec`](./spec.md), [`design`](./design.md), [`implementation plan`](./implementation-plan.md)

This folder contains the Scratch Sensei V1 source-of-truth docs.

Start here:

1. Read [`docs/README.md`](../../../README.md) for the repository documentation router.
2. Read [`docs/feature-registry.md`](../../../feature-registry.md) for queue status.
3. Read [`docs/features/scratch-sensei.md`](../../../features/scratch-sensei.md) for canonical feature state.
4. Read [`spec.md`](./spec.md) for the single Scratch Sensei V1 source of truth.
5. Read [`design.md`](./design.md) for flows, diagrams, and UI behavior.
6. Read [`implementation-plan.md`](./implementation-plan.md) only after the spec/design are approved.

---

## Implementation Agent Setup

The repo docs are the source of truth for implementation rules. Before coding,
agents should use:

- [`CONTRIBUTING.md`](../../../../CONTRIBUTING.md) for C++ style, formatting, build,
  test, and pre-commit rules.
- [`AGENTS.md`](../../../../AGENTS.md) for Mixxx-specific guardrails, especially
  `ControlObject` / `ControlProxy`, real-time audio thread constraints, and
  Qt object ownership with `parented_ptr` / `make_parented`.
- [`docs/architecture.md`](../../../architecture.md) for runtime boundaries,
  worker-thread expectations, and UI/audio/database separation.
- [`docs/interfaces-and-contracts.md`](../../../interfaces-and-contracts.md) for
  control bus and presentation contracts.

External skills are optional review lenses only. Do not let a generic Qt, C++,
CMake, or audio skill override Mixxx's local contribution guide or architecture
docs. If extra skill support is useful during implementation, the best-fit
review lenses are:

- Qt/C++ docs for QObject ownership, signals/slots, and model/view design.
- CMake docs for Essentia/build integration.
- Real-time audio architecture docs to confirm Scratch Sensei never performs
  analysis, allocation, locking, GUI work, or file I/O on the engine callback.

No external skill is required before implementation.

---

## Folder Map

| Path | Role | Current use |
| --- | --- | --- |
| [`spec.md`](./spec.md) | Current spec | The V1 source of truth for design, scope, and implementation planning. |
| [`design.md`](./design.md) | Design companion | Flows, diagrams, state model, and UI behavior rules. |
| [`implementation-plan.md`](./implementation-plan.md) | Implementation plan | Clean-room phased build plan with file map, decisions, and validation gates. |
| [`references/dj-skill-reference.md`](./references/dj-skill-reference.md) | Later reference | Broad DJ skill inventory for future Practice Skills work. Not V1. |
| [`references/proposed-curated-practice-list.md`](./references/proposed-curated-practice-list.md) | Later reference | Starter curriculum notes for future lessons. Not V1. |

---

## Boundary

Use [`spec.md`](./spec.md), [`design.md`](./design.md), and the linked ADRs as
the implementation source. Use [`implementation-plan.md`](./implementation-plan.md)
as the execution checklist after user approval.

The out-of-folder telemetry/HUD plan is not part of Scratch Sensei V1.
