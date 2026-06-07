# Scratch Sensei V1 Spec

Doc type: feature-spec
Status: draft-for-review
Owner: current-agent-or-team
Last updated: 2026-06-07
Last verified: 2026-06-07
Verified against: user-reviewed V1 scope decisions in chat
Confidence: medium
Canonical source: `docs/superpowers/plans/scratch-sensei/spec.md`
Related docs: [`Scratch Sensei feature`](../../../features/scratch-sensei.md), [`folder index`](./README.md), [`design`](./design.md), [`ADR 0001`](../../../adr/0001-scratch-sensei-native-rebuild.md), [`ADR 0002`](../../../adr/0002-scratch-sensei-required-essentia.md), [`ADR 0003`](../../../adr/0003-scratch-sensei-v1-scope.md), [`ADR 0004`](../../../adr/0004-scratch-sensei-analysis-summary-only.md)

This is the single source of truth for Scratch Sensei V1.

Implementation agents should use this spec and the linked ADRs as their starting point.

Use [`design.md`](./design.md) for flow diagrams, state diagrams, UI layout, and promotion behavior.

---

## Consultant Summary

V1 should be small, trustworthy, and musically credible.

Keep:

- Essentia-backed analysis.
- Loaded-deck context.
- Prepare Track only.
- Visual structure suggestions.
- Manual promotion into normal Mixxx cues/hotcues.

Cut from V1:

- Library-row context.
- Practice Skills.
- Lesson catalog.
- Track-pair workflows.
- Generated cue rows.
- Cue `source` / `confidence` schema changes.
- External DJ export.
- Automatic deck control.
- Automatic background surprises.

---

## Product Goal

Scratch Sensei V1 helps a DJ understand the structure of the currently loaded deck track.

It should answer:

- Where are the likely energy changes?
- Where are likely section or phrase changes?
- Which moments are worth reviewing or turning into user cues?

It should not pretend to be a perfect music oracle. The UI should use cautious language like "possible drop", "energy lift", "possible breakdown", and "phrase change".

---

## V1 User Flow

1. User loads a track to a Mixxx deck.
2. User opens Scratch Sensei.
3. Scratch Sensei uses the active/selected deck as context.
4. User clicks **Analyze with Scratch Sensei** if no current summary exists, or **Re-analyze** if they want to replace the summary.
5. Essentia extracts audio descriptors and structure candidates.
6. Scratch Sensei stores an analysis summary, not cue rows.
7. The UI shows a simple visual timeline with energy and suggestions.
8. User clicks a suggestion to inspect it.
9. User can jump to the suggestion, hide it from the current view, or promote it to a normal Mixxx cue/hotcue.
10. Re-analysis replaces only the Scratch Sensei analysis summary. It never edits or deletes user cues.

---

## Context Rules

V1 is loaded-deck only.

No library-row context. No empty multi-source chooser. No track-pair context.

Rules:

- If one deck is loaded, use that deck.
- If multiple decks are loaded, use the currently focused/active deck when Mixxx exposes one reliably.
- If active deck is ambiguous, show a small Deck 1 / Deck 2 selector.
- If no deck has a track, show one empty state: "Load a track to a deck to use Scratch Sensei."

This avoids trying to preserve library selection across sibling library views.

---

## Analysis Backend

Essentia is required for V1.

Reason:

- Mixxx's existing analysis stack is not enough for credible Scratch Sensei structure suggestions.
- A hand-rolled RMS/crossover analyzer is too weak for the product promise.
- Essentia provides a stronger MIR toolbox for spectral, rhythmic, tonal, energy, onset, and high-level descriptors.

Build rule:

- `SCRATCH_SENSEI=ON` by default in this fork.
- When Scratch Sensei is enabled and Essentia is missing, configuration fails loudly with install/build guidance.
- `SCRATCH_SENSEI=OFF` may exist only as an explicit developer escape hatch.
- Essentia is the only V1 analysis backend.

License note:

- This fork is not a commercial product, so the AGPL/non-commercial posture is acceptable for this work.
- Packaging/license details still need to be documented before merge.

---

## Data Model

V1 stores Scratch Sensei output in track analysis metadata only.

Required artifact:

- `ScratchSenseiTrackAnalysis`

Recommended storage:

- Existing `track_analysis` cache through `AnalysisDao`.
- New `AnalysisDao` analysis type such as `TYPE_SCRATCHSENSEI_TRACK_ANALYSIS`.
- Versioned serialized payload.

`ScratchSenseiTrackAnalysis` should contain:

- Analyzer version.
- Track identity.
- Audio fingerprint or equivalent staleness key.
- Track duration.
- Beat/tempo prerequisite snapshot.
- Capability/readiness flags.
- Normalized descriptor timelines.
- Derived suggestions.
- Human-readable labels and explanation text for UI.

`TrackStructureSummary` may exist as one section inside `ScratchSenseiTrackAnalysis`, but it should not be the whole artifact. The future training platform will need more than structure labels.

### Descriptor Timelines

V1 should store normalized descriptor timelines separately from derived suggestions.

Candidate timelines:

- Energy.
- Spectral bands.
- Spectral flux.
- Onset density.
- Beat strength.
- Segment-level stats.

The UI may show only one energy curve in V1. The stored contract can still expose normalized descriptors for future training use.

### Suggestions

Each suggestion should include:

- Stable suggestion ID.
- Type, such as `energy_lift`, `energy_dip`, `section_change`, or `phrase_change`.
- Human-facing label, such as "Possible drop".
- `timeSeconds`.
- Sample/frame position when available.
- Duration or analysis window when available.
- Beat, bar, or phrase index when available.
- Confidence score for internal ranking.
- Plain-language confidence bucket for UI: Strong, Maybe, Weak.
- Evidence fields, such as energy delta, spectral flux, onset density, beat strength, and confidence reason.

Future lessons need musical anchors like beat, bar, and phrase, not only wall-clock seconds.

### Capabilities

The analysis artifact should expose explicit readiness/capability flags:

- `hasEnergyCurve`
- `hasPhraseCandidates`
- `hasBeatgridSnapshot`
- `hasReliableTempo`
- `hasSuggestionEvidence`
- `analysisStale`
- `lowConfidenceReason`

Future Practice Skills should gate on capabilities rather than reading UI state.

Not in V1:

- New cue table columns.
- Scratch Sensei-specific SQL tables.
- Machine-owned cue rows.
- Generated Structure Marker cue rows.
- Export-specific fields.

Existing Mixxx analysis data should be used as context, not copied wholesale. Beatgrid/BPM provide musical anchors; key and loudness may become optional capabilities; waveform cache should not be required for Scratch Sensei analysis.

V1 input classification:

| Input | V1 decision | Rationale |
| --- | --- | --- |
| Decoded source audio for Essentia | Required | Primary source for credible MIR descriptors and suggestion evidence. |
| Track identity, duration, and staleness key | Required | Needed to know whether a stored analysis still belongs to the current audio. |
| Beatgrid / BPM | Required for beat/bar/phrase-anchored suggestions | Do not invent precise musical anchors when the track has no reliable beat snapshot. |
| Waveform / wave summary | Optional context | Useful as coarse low/mid/high energy context and UI alignment, but too compressed to be the analysis source of truth. |
| Key | Optional context | Existing Mixxx key can support future harmonic training, but V1 should not recompute it or block without it. |
| ReplayGain / EBU R128 | Optional context | Useful loudness reference, but V1 descriptors still come from Essentia/source audio. |
| Existing cues / hotcues | Optional context | Useful for display and avoiding confusing promotion choices; user promotion remains the only cue-writing path. |

Scratch Sensei V1 should not overwrite Mixxx's default BPM, beatgrid, key, ReplayGain, waveform, or silence analysis. If Essentia produces conflicting evidence, store that evidence inside `ScratchSenseiTrackAnalysis`.

---

## Future Training Platform Reserve

The long-term product direction is a Scratch Sensei training platform. V1 does not build that platform, but it must avoid architectural choices that would block it.

Reserve these seams now:

- **Analysis output is reusable:** `ScratchSenseiTrackAnalysis` should be useful outside the Prepare Track UI. Future lessons should consume the analysis contract without depending directly on Essentia internals.
- **Suggestions have stable IDs:** every suggestion should have an ID, time range or time point, type, confidence, and display label so later lessons can reference it safely.
- **Labels are cautious but typed:** UI can say "Possible drop", while the internal type can remain stable, such as `energy_lift`, `section_change`, or `phrase_change`.
- **Track context is a boundary:** V1 resolves one loaded deck, but the internal context object should be able to grow later to support track pairs without rewriting analysis.
- **Promotion is an explicit user action:** future lessons can observe promoted cues as user decisions, but V1 should not create hidden machine-owned cue state.
- **Readiness is explicit:** the summary should say which prerequisites and descriptors exist, such as beatgrid available, energy curve available, phrase candidates available, and analyzer version.
- **UI state is separate from analysis:** hiding a suggestion, selecting a marker, or focusing a deck should not corrupt the analysis payload.
- **Lessons consume capabilities, not UI widgets:** future Practice Skills should ask for capabilities like "has phrase candidates" or "has energy curve", not depend on the Prepare Track widget implementation.
- **Deck context stays out of stored analysis:** V1 can resolve a loaded deck in the UI, but stored analysis should bind to track identity, analyzer version, audio fingerprint/duration, and beatgrid snapshot.
- **Analyzer describes music, not curriculum:** V1 must not encode lesson names, scoring rules, or curriculum assumptions into the analyzer.

Do not reserve future space by adding V1 screens, database tables, or lesson engines early. Reserve it through clean data contracts and module boundaries.

---

## Cue Safety

Scratch Sensei V1 never writes generated suggestions into Mixxx cue rows.

The only cue-writing path is explicit user promotion:

- User selects a suggestion.
- User clicks **Promote to Hotcue** or **Create Cue**.
- Mixxx creates a normal user-owned cue/hotcue.

Re-analysis:

- Replaces the Scratch Sensei summary.
- Does not delete cues.
- Does not retime cues.
- Does not recolor cues.
- Does not relabel cues.
- Does not touch loops.

This means V1 does not need cue ownership metadata.

---

## UI Design

V1 has one workspace: **Prepare Track**.

No Practice Skills tab in V1.

Required layout:

- Header: active deck and track title.
- Analyze/Re-analyze button.
- Status line: not analyzed, analyzing, analyzed, failed, stale.
- One horizontal timeline.
- One energy curve.
- Suggestion markers.
- Suggestion detail panel.

Suggestion detail panel:

- Label, e.g. "Possible drop".
- Time position.
- Confidence as plain language: Strong, Maybe, Weak.
- Actions:
  - Jump
  - Create Cue
  - Promote to Hotcue
  - Hide suggestion

Keep the UI visual-first. Avoid tables for V1.

`Create Cue` should be the primary promotion action. `Promote to Hotcue` is secondary and explicit because hotcues are performance decisions.

---

## Language Rules

Do not overclaim.

Use:

- Possible drop.
- Possible breakdown.
- Energy lift.
- Energy dip.
- Phrase change.
- Section change.

Avoid:

- Definite "Drop" unless the user promoted/named it.
- Numeric confidence in the main UI.
- Expert-only MIR labels in the main UI.

---

## Out Of Scope For V1

- Practice Skills.
- Lesson catalog.
- ControlObserver lesson checks.
- Two-deck or track-pair workflows.
- Library-row context.
- Generated cue rows.
- Cue `source` and `confidence` schema.
- External DJ library export.
- Preference panels.
- Waveform scrims in the main scrolling waveform.
- Automatic deck manipulation.
- Automatic background analysis unless explicitly re-approved.

---

## Self-Grill Answers

**Should V1 include Practice Skills because the end goal is training?**

No. That would split the first implementation across two products. V1 should create the analysis contract that future Practice Skills can trust.

**Is loaded-deck-only too narrow?**

No. It is the right V1 constraint. The data layer should still store analysis by track, but the V1 UI should avoid library-row and track-pair context complexity.

**Does cutting generated cue rows block the future training platform?**

No. It helps. Future lessons can use analysis suggestions and user-promoted cues as separate concepts. Mixing generated suggestions into cue rows too early would make training logic harder to trust.

**Does requiring Essentia make V1 harder?**

Yes, but it is the right hard thing. The product promise depends on credible audio descriptors. The spec should keep V1 small elsewhere to pay for this dependency cost.

**What architectural mistake would hurt V2 most?**

Hard-coding lesson assumptions into the analyzer or Prepare Track UI. V1 should produce a reusable analysis summary; later training features should consume that summary through a separate practice layer.

**Is `TrackStructureSummary` the right top-level artifact?**

No. It is too narrow. V1 should define `ScratchSenseiTrackAnalysis` as the top-level contract, with structure summary, descriptor timelines, suggestions, capabilities, and evidence inside it.

---

## Implementation Phases

### Phase 0: Spec Approval

- Review and approve this spec before writing code.
- Write an implementation plan from this spec.
- Keep all implementation tasks traceable to a V1 requirement or ADR.

### Phase 1: Essentia Build Gate

- Add Essentia discovery/build support.
- Document AGPL/non-commercial posture.
- Add `SCRATCH_SENSEI` build option.
- Fail loudly when enabled and Essentia is unavailable.

### Phase 2: Analysis Summary

- Add `ScratchSenseiTrackAnalysis` payload.
- Add `AnalysisDao` type/storage using the existing `track_analysis` cache.
- Implement Essentia-backed extraction behind a clear provider boundary.
- Store suggestions only in analysis metadata.
- Model analysis as an async job with progress, failure, cancellation, and cacheability.

### Phase 3: Prepare Track UI

- Add Scratch Sensei sidebar entry.
- Resolve loaded deck context.
- Show empty/loading/error/analyzed states.
- Render simple timeline, energy curve, and suggestions.

### Phase 4: Promotion

- Let the user promote a suggestion into a normal Mixxx cue/hotcue.
- Ensure re-analysis never changes user cues.

### Phase 5: Verification

- Unit test summary parsing and storage.
- Unit test cue safety.
- Unit test Essentia provider boundaries with fixtures or fakes.
- Manual smoke only after design and implementation review pass.

---

## Implementation Plan Decisions

Resolved for V1:

- If active deck is ambiguous, show an explicit Deck 1 / Deck 2 selector.
- `Create Cue` is primary; `Promote to Hotcue` is secondary.
- Hidden suggestions are session-only in V1 unless a later user-overlay design is accepted.
- Minimum Essentia scope should cover energy, spectral bands, flux/onsets, rhythm/beat strength, and segment statistics.

Decide during the implementation plan:

- Which Mixxx signal or focus state should define the active deck when Mixxx exposes it reliably.
- Whether `ScratchSenseiAnalysisJob` should register with existing analyzer infrastructure or run as a separate manual worker job.
- The exact Essentia algorithms and descriptor normalization names for the first analysis contract.
- The acceptable macOS build/install path for Essentia in this fork.
- The first UI integration surface for the Scratch Sensei sidebar entry.

---

## Exact Next Prompt

Review `docs/superpowers/plans/scratch-sensei/implementation-plan.md` against this spec and `docs/superpowers/plans/scratch-sensei/design.md`. If approved, execute Task 0 and Task 1 clean-room: do not reference old Scratch Sensei implementation code.
