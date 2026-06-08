# Documentation Health

Doc type: doc-health
Owner: current-agent-or-team
Status: active
Last updated: 2026-06-08
Last verified: 2026-06-08
Verified against: docs/, `docs/features/scratch-sensei.md`, `docs/adr/`, `docs/decision-log.md`, `docs/feature-registry.md`
Confidence: high
Canonical source: `docs/doc-health.md`
Related docs: `README.md`, `feature-registry.md`, `decision-log.md`
Last full audit: 2026-06-08
Known stale areas: none
Open doc conflicts: none

This file tracks the freshness, verification matrices, open conflicts, and renames across the canonical Mixxx documentation set.

---

## Health Summary

- **Last full audit**: 2026-06-08
- **Current overall confidence**: high
- **Known stale areas**: none
- **Open doc conflicts**: none

---

## Verification Matrix

| Doc | Last verified | Verified against | Confidence | Known drift or action |
| --- | --- | --- | --- | --- |
| `README.md` | 2026-06-02 | `docs/intake/recommended-canonical-docs.md`, `README.md`, `CONTRIBUTING.md`, `CHANGELOG.md` | high | None |
| `architecture.md` | 2026-06-02 | `src/main.cpp`, `src/coreservices.cpp`, `src/control/controlobject.h`, `res/schema.xml` | high | None |
| `data-model.md` | 2026-06-02 | `res/schema.xml`, `src/track/track.h`, `src/track/trackrecord.h`, `src/library/trackcollectionmanager.h` | high | None |
| `testing-strategy.md` | 2026-06-02 | `src/test/main.cpp`, `CMakeLists.txt` | high | None |
| `decision-log.md` | 2026-06-08 | Scratch Sensei V1 ADRs, spec, ADR 0005 V2 TF decision | high | DL-010 added for V2 TF integration |
| `feature-registry.md` | 2026-06-08 | `docs/features/scratch-sensei.md`, `docs/adr/0005-scratch-sensei-v2-tensorflow.md` | high | V1 marked complete; V2 TensorFlow integration added as rank-1 in-progress work |
| `features/vibe-coding-experiments.md` | 2026-06-02 | `docs/` scaffolding progress | high | None |
| `features/scratch-sensei.md` | 2026-06-08 | `docs/adr/0005-scratch-sensei-v2-tensorflow.md`, `docs/decision-log.md` (DL-010) | high | V2 decisions section added; implementation checklist extended with V2 tasks; Exact Next Prompt updated |
| `superpowers/plans/scratch-sensei/README.md` | 2026-06-07 | Scratch Sensei plan folder contents | high | Fresh folder index with implementation plan |
| `superpowers/plans/scratch-sensei/spec.md` | 2026-06-07 | User-approved V1 direction | medium | V1 spec; V2 spec will be a separate doc |
| `superpowers/plans/scratch-sensei/design.md` | 2026-06-07 | `spec.md` | medium | Flows, diagrams, state model, and UI behavior added |
| `superpowers/plans/scratch-sensei/implementation-plan.md` | 2026-06-07 | `spec.md`, `design.md`, `CONTRIBUTING.md`, `AGENTS.md` | medium | V1 complete; plan doc accurate |
| `adr/0001-scratch-sensei-native-rebuild.md` | 2026-06-07 | `spec.md` | high | Native Mixxx feature decision |
| `adr/0002-scratch-sensei-required-essentia.md` | 2026-06-07 | `spec.md` | high | Essentia required for V1 |
| `adr/0003-scratch-sensei-v1-scope.md` | 2026-06-07 | `spec.md` | high | Loaded-deck Prepare Track only |
| `adr/0004-scratch-sensei-analysis-summary-only.md` | 2026-06-07 | `spec.md`, `design.md`, `AnalysisDao` shape | high | Suggestions stay out of cue rows; `track_analysis` cache used for analysis contract |
| `adr/0005-scratch-sensei-v2-tensorflow.md` | 2026-06-08 | `decision-log.md` (DL-010), user approval | high | New ADR for V2 TF integration; lazy loading, graceful fallback, model path conventions |

---

## Conflicts and Corrections

- 2026-06-08: V1 fully implemented, compiled, and all 14 unit tests passing on ARM64 macOS. Docs updated to reflect V1 complete state.
- 2026-06-08: ADR 0005 added: Scratch Sensei V2 TensorFlow integration. Essentia TF rebuild required; lazy model loading; graceful fallback; protobuf schema extension; model files stored at `~/.mixxx/models/`.
- 2026-06-08: DL-010 added to decision-log for V2 TF decision.
- 2026-06-08: `feature-registry.md` updated: Scratch Sensei V1 marked `complete`; V2 TensorFlow added as `in_progress` rank-1 item.
- 2026-06-08: `features/scratch-sensei.md` updated: V2 Decisions section added; implementation checklist V2 tasks added; Exact Next Prompt updated to Essentia TF rebuild.
- 2026-06-07: Established the Scratch Sensei V1 source-of-truth spec.
- 2026-06-07: Kept the active Scratch Sensei docs to the V1 spec, folder index, four ADRs, and future lesson references.
- 2026-06-07: Recorded four V1 decisions: native Mixxx feature, required Essentia, loaded-deck Prepare Track only, and analysis-summary-only suggestions.
- 2026-06-07: Added future training-platform reserve constraints: `ScratchSenseiTrackAnalysis`, stable suggestion IDs, musical anchors, descriptor timelines, evidence fields, capability flags, and analyzer/curriculum separation.
- 2026-06-07: Added Scratch Sensei V1 design companion with product flow, data flow, analysis job state machine, UI layout, suggestion model, promotion flow, and future training-platform path diagrams.
- 2026-06-07: Added track-analysis implementation design: reuse `track_analysis` / `AnalysisDao`, treat existing beatgrid/BPM/key/loudness/waveform data as context, keep future descriptors inside the serialized `ScratchSenseiTrackAnalysis` payload.
- 2026-06-07: Added Scratch Sensei implementation-agent setup guidance: local Mixxx docs and contribution rules are required; external Qt/C++/CMake/audio skills are optional review lenses only.
- 2026-06-07: Clarified Scratch Sensei spec review status by separating resolved V1 decisions from implementation-plan decisions.
- 2026-06-07: Added Scratch Sensei V1 clean-room implementation plan with build, analysis contract, DAO storage, analyzer, UI, cue promotion, and validation phases.
- 2026-06-07: Patched Scratch Sensei implementation-plan review findings: CMake target placement, test-only fake provider boundary, Essentia install path, analysis memory guard, re-analysis replacement semantics, and sample-rate-safe cue creation.
- 2026-06-07: Kept broad DJ skill and curated practice references as later-version reference material, not V1 scope.

---

## Renames and Supersessions

| Old path | New path | Reason |
| --- | --- | --- |
| `docs/adr/0002-scratch-sensei-optional-essentia-gate.md` | `docs/adr/0002-scratch-sensei-required-essentia.md` | Essentia is required for V1 rather than optional planning context. |
