# Feature Registry

Doc type: feature-registry
Owner: current-agent-or-team
Status: active
Last updated: 2026-06-08
Last verified: 2026-06-08
Verified against: `docs/features/scratch-sensei.md`, `docs/adr/0005-scratch-sensei-v2-tensorflow.md`, `docs/superpowers/plans/scratch-sensei/implementation-plan.md`
Confidence: high
Canonical source: `docs/feature-registry.md`
Related docs: `README.md`, `doc-health.md`

This document tracks all planned, ongoing, and completed features of this disconnected fork. It houses the ranked next-work queue used to route developer and agent efforts.

---

## Next Work Queue

| Rank | Work item | Type | Status | Ready | Why next | Next safe step | Canonical doc | Last verified |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | Scratch Sensei V2 (TensorFlow) | feature | `in_progress` | `ready` | V1 fully implemented, compiled, and tested. V2 ADR accepted. Essentia needs TF rebuild; proto schema expansion ready to begin. | Rebuild Essentia with `--with-tensorflow`, update CMake, expand protobuf schema, extend provider | [`features/scratch-sensei.md`](./features/scratch-sensei.md) | 2026-06-08 |
| 2 | Vibe Coding Experiments | feature | `in_progress` | `ready` | First entry for fork experimentation | Run documentation validation tool to verify structural layout | [`features/vibe-coding-experiments.md`](./features/vibe-coding-experiments.md) | 2026-06-02 |

---

## Feature List

| Feature | Slug | Status | Priority | Last updated | Notes |
| --- | --- | --- | --- | --- | --- |
| Scratch Sensei V1 | `scratch-sensei` | `complete` | High | 2026-06-08 | [Feature Doc](./features/scratch-sensei.md), [Spec](./superpowers/plans/scratch-sensei/spec.md), [Implementation Plan](./superpowers/plans/scratch-sensei/implementation-plan.md). V1 implemented, compiled, and all unit tests pass. |
| Scratch Sensei V2 (TensorFlow) | `scratch-sensei-v2` | `in_progress` | High | 2026-06-08 | [ADR 0005](./adr/0005-scratch-sensei-v2-tensorflow.md). Essentia TF rebuild + proto schema expansion + lazy model loading. |
| Vibe Coding Experiments | `vibe-coding-experiments` | `in_progress` | High | 2026-06-02 | [Feature Doc](./features/vibe-coding-experiments.md) |
