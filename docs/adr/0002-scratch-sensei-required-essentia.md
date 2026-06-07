# Scratch Sensei requires Essentia for V1 analysis

Status: accepted

Scratch Sensei V1 should require Essentia as the analysis backend. The current Mixxx analysis stack and a hand-rolled RMS/crossover analyzer are not enough for credible structure and energy suggestions.

This fork is not a commercial product, so the AGPL/non-commercial posture is acceptable for planning this implementation. Packaging and license notes still need to be documented before merge.

## Consequences

- `SCRATCH_SENSEI=ON` should be the default for this fork.
- When Scratch Sensei is enabled and Essentia is unavailable, configuration should fail loudly with install/build guidance.
- `SCRATCH_SENSEI=OFF` may exist as an explicit developer escape hatch.
- Essentia is the only V1 analysis backend.
- The implementation plan must identify the minimum Essentia algorithm set before coding the analyzer.
