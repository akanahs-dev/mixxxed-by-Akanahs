# AGENTS.md — Mixxx Project Instructions

See [README.md](README.md) for a project overview, and
[CONTRIBUTING.md](CONTRIBUTING.md) for build instructions, code style,
pre-commit setup, Git workflow, and pull request guidelines.

## Key Architecture

- **ControlObject/ControlProxy**: `[Group], key_name` inter-component communication.
- **Engine thread**: Real-time audio — no allocations, no locks, may emit Qt signals but cannot receive them.
- **parented_ptr/make_parented**: Qt object-tree ownership. Object must get a parent before `parented_ptr` destructs.

## Project Layout

```text
src/          C++ source (engine/, controllers/, library/, mixer/, effects/, qml/, preferences/, util/, test/)
res/          Resources (controllers/ JS/XML, skins/, qml/)
cmake/        CMake modules
tools/        Python helper scripts
```

## Documentation Source of Truth

This project uses Repo Memory for cross-agent continuity.

[`docs/README.md`](docs/README.md) contains the Canonical Ownership Map. Update the mapped owner for any changed capability. Do not duplicate mutable project facts in this file.

When starting or resuming work:
1. Read [`docs/README.md`](docs/README.md).
2. Run the validator (`python3 /Users/ipad/.gemini/config/skills/repo-memory/scripts/validate-docs.py --project-docs /Users/ipad/github/mixxxed-by-Akanahs --adoption-level continuity --strict` when installed) to automatically check if documentation has drifted.
3. Follow the Canonical Ownership Map to the project overview, architecture, decision, contract, setup, and feature owners relevant to the task.
4. Review `docs/intake/` if it contains raw brainstorms, project notes, or plans relevant to the work, then promote accepted facts into the mapped owner.
5. Read [`docs/feature-registry.md`](docs/feature-registry.md); when no task is assigned, pick the first `ready` row in `Next Work Queue`.
6. Read the active `docs/features/<feature-slug>.md` before making changes.

When making changes:
- Update the active feature doc as the work changes.
- Update the `Next Work Queue` when priority, readiness, or pickup instructions change.
- Place companion plans/specs only in `docs/superpowers/plans/` or `docs/superpowers/specs/` (or `docs/designs/`).
- Update the mapped canonical owner for changed decisions, contracts, commands, architecture, runtime signals, or security posture.
- Put durable project facts in their mapped owner, not only in agent-specific instruction files or chat history.
- Keep any agent-specific instruction files short and aligned to the same docs entrypoints.

Before stopping:
- Run the validator (`python3 /Users/ipad/.gemini/config/skills/repo-memory/scripts/validate-docs.py --project-docs /Users/ipad/github/mixxxed-by-Akanahs --adoption-level continuity --strict`) and fix any warnings, errors, or plan-placement drift.
- Update `docs/features/<feature-slug>.md`, especially `Implementation Status`, `Validation`, `Resume Context`, `Next Agent Handoff`, and `Exact Next Prompt` when present.
- Update the mapped implementation-history owner for meaningful landed work.
- Update the mapped decision owner when a durable technical choice changed.
- Update [`docs/doc-health.md`](docs/doc-health.md) when docs were verified, corrected, found stale, or when duplicate ownership was removed.

