# Documentation Health

Doc type: doc-health
Owner: current-agent-or-team
Status: active
Last updated: 2026-06-07
Last verified: 2026-06-07
Verified against: docs/, `docs/features/scratch-sensei.md`, `docs/adr/`, `docs/superpowers/plans/scratch-sensei/spec.md`, `docs/superpowers/plans/scratch-sensei/design.md`, `docs/superpowers/plans/scratch-sensei/README.md`, `docs/superpowers/plans/scratch-sensei/implementation-plan.md`
Confidence: high
Canonical source: `docs/doc-health.md`
Related docs: `README.md`, `feature-registry.md`, `decision-log.md`
Last full audit: 2026-06-02
Known stale areas: none inside the fresh Scratch Sensei V1 docs
Open doc conflicts: none inside the fresh Scratch Sensei V1 docs

This file tracks the freshness, verification matrices, open conflicts, and renames across the canonical Mixxx documentation set.

---

## Health Summary

- **Last full audit**: 2026-06-02
- **Current overall confidence**: high for the fresh Scratch Sensei V1 docs shape
- **Known stale areas**: none inside the fresh Scratch Sensei V1 docs
- **Open doc conflicts**: none inside the fresh Scratch Sensei V1 docs

---

## Verification Matrix

| Doc | Last verified | Verified against | Confidence | Known drift or action |
| --- | --- | --- | --- | --- |
| `README.md` | 2026-06-02 | `docs/intake/recommended-canonical-docs.md`, `README.md`, `CONTRIBUTING.md`, `CHANGELOG.md` | high | None |
| `architecture.md` | 2026-06-02 | `src/main.cpp`, `src/coreservices.cpp`, `src/control/controlobject.h`, `res/schema.xml` | high | None |
| `data-model.md` | 2026-06-02 | `res/schema.xml`, `src/track/track.h`, `src/track/trackrecord.h`, `src/library/trackcollectionmanager.h` | high | None |
| `testing-strategy.md` | 2026-06-02 | `src/test/main.cpp`, `CMakeLists.txt` | high | None |
| `decision-log.md` | 2026-06-07 | Scratch Sensei V1 ADRs and spec | high | Fresh V1 decisions recorded |
| `feature-registry.md` | 2026-06-07 | `docs/features/scratch-sensei.md`, `docs/superpowers/plans/scratch-sensei/spec.md`, `docs/superpowers/plans/scratch-sensei/implementation-plan.md` | high | Scratch Sensei now points to fresh V1 spec and implementation plan |
| `features/vibe-coding-experiments.md` | 2026-06-02 | `docs/` scaffolding progress | high | None |
| `features/scratch-sensei.md` | 2026-06-07 | `docs/superpowers/plans/scratch-sensei/spec.md`, `docs/superpowers/plans/scratch-sensei/implementation-plan.md` | high | Planned; implementation intentionally absent |
| `superpowers/plans/scratch-sensei/README.md` | 2026-06-07 | Scratch Sensei plan folder contents | high | Fresh folder index with implementation plan |
| `superpowers/plans/scratch-sensei/spec.md` | 2026-06-07 | User-approved V1 direction | medium | Implementation plan written; awaiting user review/approval before code |
| `superpowers/plans/scratch-sensei/design.md` | 2026-06-07 | `spec.md` | medium | Flows, diagrams, state model, and UI behavior added |
| `superpowers/plans/scratch-sensei/implementation-plan.md` | 2026-06-07 | `spec.md`, `design.md`, `CONTRIBUTING.md`, `AGENTS.md`, `docs/architecture.md`, `AnalysisDao`, analyzer and library feature source anchors | medium | Clean-room phased implementation plan ready for user review |
| `adr/0001-scratch-sensei-native-rebuild.md` | 2026-06-07 | `spec.md` | high | Native Mixxx feature decision |
| `adr/0002-scratch-sensei-required-essentia.md` | 2026-06-07 | `spec.md` | high | Essentia required for V1 |
| `adr/0003-scratch-sensei-v1-scope.md` | 2026-06-07 | `spec.md` | high | Loaded-deck Prepare Track only |
| `adr/0004-scratch-sensei-analysis-summary-only.md` | 2026-06-07 | `spec.md`, `design.md`, `AnalysisDao` shape | high | Suggestions stay out of cue rows; `track_analysis` cache used for analysis contract |

---

## Conflicts and Corrections

- 2026-06-07: Established the Scratch Sensei V1 source-of-truth spec.
- 2026-06-07: Kept the active Scratch Sensei docs to the V1 spec, folder index, four ADRs, and future lesson references.
- 2026-06-07: Recorded four V1 decisions: native Mixxx feature, required Essentia, loaded-deck Prepare Track only, and analysis-summary-only suggestions.
- 2026-06-07: Added future training-platform reserve constraints: `ScratchSenseiTrackAnalysis`, stable suggestion IDs, musical anchors, descriptor timelines, evidence fields, capability flags, and analyzer/curriculum separation.
- 2026-06-07: Added Scratch Sensei V1 design companion with product flow, data flow, analysis job state machine, UI layout, suggestion model, promotion flow, and future training-platform path diagrams.
- 2026-06-07: Added track-analysis implementation design: reuse `track_analysis` / `AnalysisDao`, treat existing beatgrid/BPM/key/loudness/waveform data as context, keep future descriptors inside the serialized `ScratchSenseiTrackAnalysis` payload.
- 2026-06-07: Added Scratch Sensei implementation-agent setup guidance: local Mixxx docs and contribution rules are required; external Qt/C++/CMake/audio skills are optional review lenses only.
- 2026-06-07: Clarified Scratch Sensei spec review status by separating resolved V1 decisions from implementation-plan decisions.
- 2026-06-07: Added Scratch Sensei V1 clean-room implementation plan with build, analysis contract, DAO storage, analyzer, UI, cue promotion, and validation phases.
- 2026-06-07: Kept broad DJ skill and curated practice references as later-version reference material, not V1 scope.

---

## Renames and Supersessions

| Old path | New path | Reason |
| --- | --- | --- |
| `docs/adr/0002-scratch-sensei-optional-essentia-gate.md` | `docs/adr/0002-scratch-sensei-required-essentia.md` | Essentia is required for V1 rather than optional planning context. |
