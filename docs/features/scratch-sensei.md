# Feature: scratch-sensei

Doc type: feature
Feature slug: scratch-sensei
Status: active
Owner: current-agent-or-team
Priority: High
Last updated: 2026-06-08
Last verified: 2026-06-08
Verified against: `docs/adr/0005-scratch-sensei-v2-tensorflow.md`, `docs/decision-log.md` (DL-010), `docs/feature-registry.md`
Confidence: high
Canonical source: `docs/features/scratch-sensei.md`
Related docs: `../feature-registry.md`, `../decision-log.md`, `../doc-health.md`, `../adr/0001-scratch-sensei-native-rebuild.md`, `../adr/0002-scratch-sensei-required-essentia.md`, `../adr/0003-scratch-sensei-v1-scope.md`, `../adr/0004-scratch-sensei-analysis-summary-only.md`, `../adr/0005-scratch-sensei-v2-tensorflow.md`, `../superpowers/plans/scratch-sensei/README.md`, `../superpowers/plans/scratch-sensei/spec.md`, `../superpowers/plans/scratch-sensei/design.md`, `../superpowers/plans/scratch-sensei/implementation-plan.md`
Validation status: V1 fully implemented, compiling successfully, and all unit tests passing. V2 TensorFlow integration in progress (ADR 0005 accepted).
Next safe step: Rebuild Essentia with `--with-tensorflow`, update CMakeLists.txt, expand protobuf schema, extend Essentia provider with TF model loading.

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

## V2 Decisions (TensorFlow Integration)

- Essentia must be recompiled with `--with-tensorflow` (see ADR 0005).
- TensorFlow model `.pb` files live at `~/.mixxx/models/` — not in the source tree.
- Models load lazily on the async analyzer worker thread.
- Graceful fallback to V1 classic Essentia analysis if TF models are absent or fail to init.
- `ScratchSenseiTrackAnalysis` protobuf schema extended with ML classification vectors.
- V1 cache entries remain valid; V2 output is additive.

---

## Implementation Status

- [x] Create the Scratch Sensei V1 source-of-truth spec.
- [x] Keep later lesson references outside V1 scope.
- [x] Keep future curriculum references as non-V1 reference material.
- [x] Review and approve V1 spec.
- [x] Write implementation plan.
- [x] Implement V1 from the approved spec.
- [x] All V1 unit tests passing (14 tests).
- [x] Build verified on ARM64 macOS (only linker warnings, no errors).
- [ ] V2: Rebuild Essentia with `--with-tensorflow`.
- [ ] V2: Update `FindEssentia.cmake` and `CMakeLists.txt` for TF detection.
- [ ] V2: Expand `scratchsenseitrackanalysis.proto` with ML classification fields.
- [ ] V2: Extend `ScratchSenseiEssentiaProvider` with `TensorflowPredict` algorithm(s).
- [ ] V2: Implement graceful fallback when TF models are absent.
- [ ] V2: Manual smoke test with a real MTG `.pb` model.

---

## Exact Next Prompt

Run `brew reinstall mtg/essentia/essentia --with-tensorflow`. Then update `cmake/modules/FindEssentia.cmake` to detect TF support, gate V2 features in `CMakeLists.txt`, expand `scratchsenseitrackanalysis.proto` with ML classification fields, and extend `ScratchSenseiEssentiaProvider` to load and run a `TensorflowPredict`-family algorithm lazily on the worker thread with graceful fallback.
