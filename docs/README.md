# Mixxx Repository Documentation Map

Doc type: ownership-map
Owner: current-agent-or-team
Status: active
Last updated: 2026-06-02
Last verified: 2026-06-02
Verified against: docs/intake/recommended-canonical-docs.md, README.md, CONTRIBUTING.md, CHANGELOG.md
Confidence: high
Canonical source: `docs/README.md`
Related docs: `architecture.md`, `data-model.md`, `testing-strategy.md`, `decision-log.md`, `feature-registry.md`, `doc-health.md`

Welcome to the Mixxx repository documentation map. This project adopts the Repo Memory standard to manage repository-native project context and ensure cross-agent continuity.

This document acts as the context router and defines the single canonical owner for every documentation capability.

## Canonical Ownership Map

| Capability | Canonical owner | Supporting docs / evidence | Notes |
| --- | --- | --- | --- |
| Documentation map and ownership map | [`docs/README.md`](./README.md) | `AGENTS.md` | Maps routing and capability ownership. |
| Project overview, goal, users, scope | [`README.md`](../README.md) | [`CONTRIBUTING.md`](../CONTRIBUTING.md) | Main entrypoint for high-level product details. |
| Architecture and system shape | [`docs/architecture.md`](./architecture.md) | `AGENTS.md`, `docs/intake/inferred-architecture.md` | Describes composition root, subsystem boundaries, and threading rules. |
| Interfaces and contracts | [`docs/interfaces-and-contracts.md`](./interfaces-and-contracts.md) | `res/controllers/` | Covers controller APIs, QML properties, and internal control keys. |
| Data model | [`docs/data-model.md`](./data-model.md) | `res/schema.xml`, `docs/intake/inferred-data-model.md` | Domain records (`Track`), collections, and SQLite schema. |
| Local development and tooling | [`CONTRIBUTING.md`](../CONTRIBUTING.md) | [`README.md`](../README.md) | Contributor guidelines and compilation instructions. |
| Testing strategy | [`docs/testing-strategy.md`](./testing-strategy.md) | `src/test/`, `CMakeLists.txt` | Automated GTest structure and manual validation paths. |
| Operations and runbooks | [`docs/operations-runbook.md`](./operations-runbook.md) | `packaging/`, `.github/workflows/build.yml` | Builds, installer signing, and packaging flows. |
| Security and privacy | [`docs/security-and-privacy.md`](./security-and-privacy.md) | `src/network/` | Security policies and trust boundaries. |
| Observability and instrumentation | [`docs/observability-and-instrumentation.md`](./observability-and-instrumentation.md) | `src/effects/effectsmessenger.h` | Logging, instrumentation, and runtime telemetry. |
| Decisions and rationale | [`docs/decision-log.md`](./decision-log.md) | `docs/intake/inferred-decisions.md` | Log of active architectural and product decisions. |
| Implementation history | [`CHANGELOG.md`](../CHANGELOG.md) | Release tags | Public history of landed releases. |
| Feature registry / next work queue | [`docs/feature-registry.md`](./feature-registry.md) | `docs/features/` | Ranked list of features and pickup queue. |
| Active feature handoff | `docs/features/<feature-slug>.md` | `docs/feature-registry.md` | Handoff notes for ongoing tasks. |
| Documentation health | [`docs/doc-health.md`](./doc-health.md) | `docs/intake/` | Tracks stale docs, drift, and validation state. |

## Guide for Developers and Agents

1. **Check the Map**: When starting any task, read this document first to locate the single owner of the relevant capability.
2. **Do Not Duplicate**: Do not duplicate build commands, decisions, contracts, or active feature states across different files.
3. **Follow the Feature Queue**: Read [`docs/feature-registry.md`](./feature-registry.md) to locate active features or find the next ready-to-implement task.
4. **Resuming Work**: If resuming an interrupted task, read the corresponding feature document in `docs/features/` to retrieve context.
