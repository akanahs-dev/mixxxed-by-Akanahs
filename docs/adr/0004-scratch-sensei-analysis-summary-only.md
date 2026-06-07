# Scratch Sensei V1 stores suggestions in analysis summaries, not cue rows

Status: accepted

Scratch Sensei V1 should store generated structure suggestions in a versioned `ScratchSenseiTrackAnalysis` artifact, not in Mixxx cue rows.

The artifact should be stored through Mixxx's existing `track_analysis` / `AnalysisDao` cache with a Scratch Sensei-specific analysis type. V1 should not add a dedicated Scratch Sensei SQL table.

The only cue-writing path in V1 is explicit user promotion. A suggestion can become a normal Mixxx cue or hotcue only when the user chooses that action.

The analysis artifact should be designed as a reusable contract for later training features, with stable suggestion IDs, typed suggestion kinds, analyzer version, descriptor availability, musical anchors, evidence fields, and human-facing labels.

## Consequences

- Re-analysis replaces Scratch Sensei analysis metadata only.
- Re-analysis never deletes, retimes, recolors, relabels, or overwrites user cues.
- V1 does not need cue `source` or `confidence` schema columns.
- V1 does not export generated suggestions to external DJ libraries.
- Confidence can remain internal and be shown as plain-language labels such as Strong, Maybe, or Weak.
- Later Practice Skills can consume the summary without reading Prepare Track UI state.
- `TrackStructureSummary` can be a section inside the artifact, but it is not the whole contract.
- Stored analysis should bind to track identity, analyzer version, audio fingerprint or staleness key, duration, and beatgrid snapshot rather than deck history.
- Existing Mixxx beatgrid/BPM data should provide anchors and prerequisites; Essentia should provide Scratch Sensei descriptor timelines and suggestion evidence.
- Existing waveform/wave-summary data should be optional context only. It can help align Scratch Sensei markers with the visible waveform and provide coarse band-energy cross-checks, but it should not replace source-audio analysis or block analysis when absent.
- Existing key analysis should be optional context only. It can support future harmonic training, but V1 should not recompute key, overwrite key, or require key to produce energy and structure suggestions.
- Adding future descriptors should usually evolve the serialized payload, not add SQL columns.
- Scratch Sensei V1 should not overwrite default Mixxx BPM, beatgrid, key, ReplayGain, waveform, or silence analysis.
