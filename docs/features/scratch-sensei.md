# Feature: scratch-sensei

Doc type: feature
Feature slug: scratch-sensei
Status: planned
Owner: current-agent-or-team
Priority: High
Last updated: 2026-06-07
Last verified: 2026-06-07
Verified against: `docs/superpowers/plans/scratch-sensei/spec.md`, `docs/superpowers/plans/scratch-sensei/implementation-plan.md`
Confidence: medium
Canonical source: `docs/features/scratch-sensei.md`
Related docs: `../feature-registry.md`, `../decision-log.md`, `../doc-health.md`, `../adr/0001-scratch-sensei-native-rebuild.md`, `../adr/0002-scratch-sensei-required-essentia.md`, `../adr/0003-scratch-sensei-v1-scope.md`, `../adr/0004-scratch-sensei-analysis-summary-only.md`, `../superpowers/plans/scratch-sensei/README.md`, `../superpowers/plans/scratch-sensei/spec.md`, `../superpowers/plans/scratch-sensei/design.md`, `../superpowers/plans/scratch-sensei/implementation-plan.md`
Validation status: V1 spec and clean-room implementation plan ready for user review
Next safe step: review and approve `docs/superpowers/plans/scratch-sensei/implementation-plan.md`, then execute Task 0 and Task 1

---

## Goal

Implement Scratch Sensei V1 as a Mixxx-native loaded-deck track preparation workspace powered by Essentia.

V1 should help the DJ inspect likely structure and energy moments for the loaded deck track, then manually promote useful suggestions into normal Mixxx cues/hotcues.

---

## Source Of Truth

Use this order:

1. [`docs/README.md`](../README.md) for documentation ownership.
2. [`docs/feature-registry.md`](../feature-registry.md) for queue status.
3. This feature doc for canonical feature state.
4. [`docs/superpowers/plans/scratch-sensei/README.md`](../superpowers/plans/scratch-sensei/README.md) for the folder map.
5. [`docs/superpowers/plans/scratch-sensei/spec.md`](../superpowers/plans/scratch-sensei/spec.md) for the V1 source of truth.
6. [`docs/superpowers/plans/scratch-sensei/design.md`](../superpowers/plans/scratch-sensei/design.md) for flows, diagrams, and UI behavior.
7. [`docs/superpowers/plans/scratch-sensei/implementation-plan.md`](../superpowers/plans/scratch-sensei/implementation-plan.md) for the clean-room execution checklist.

---

## V1 Decisions

- Essentia is required.
- Implementation starts from the approved V1 spec.
- Loaded-deck context only.
- Prepare Track only.
- Suggestions live in `ScratchSenseiTrackAnalysis`, not cue rows.
- `ScratchSenseiTrackAnalysis` is stored through existing `track_analysis` / `AnalysisDao`.
- User promotion is the only cue-writing path.
- No Practice Skills in V1.
- No external DJ export in V1.
- No automatic deck control.
- Future training-platform support is reserved through data contracts and module boundaries, not V1 screens.

---

## Implementation Status

- [x] Create the Scratch Sensei V1 source-of-truth spec.
- [x] Keep later lesson references outside V1 scope.
- [x] Keep future curriculum references as non-V1 reference material.
- [ ] Review and approve V1 spec.
- [x] Write implementation plan.
- [ ] Implement V1 from the approved spec.

---

## Exact Next Prompt

Review `docs/superpowers/plans/scratch-sensei/implementation-plan.md`. If approved, execute Task 0 and Task 1 without referencing old Scratch Sensei implementation code.
