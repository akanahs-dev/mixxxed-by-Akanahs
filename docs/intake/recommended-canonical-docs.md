---
doc_type: intake-recommended-canonical-docs
status: draft
last_updated: 2026-06-02
last_verified: 2026-06-02
confidence: medium
mode: bootstrapper
canonical: false
---

# Recommended canonical docs and promotion plan

This draft proposes a Repo Memory ownership map for Mixxx that preserves strong
existing docs where possible and introduces new canonical owners only where the
repository currently has gaps.

## Proposed ownership map

| Capability | Proposed canonical owner | Supporting docs / evidence | Notes |
| --- | --- | --- | --- |
| Documentation map and ownership map | `docs/README.md` | all intake docs in `docs/intake/` | New file recommended. Needed to prevent duplicate ownership. |
| Project overview, goal, users, high-level scope | `README.md` initially | `CONTRIBUTING.md` | Lowest-friction start. Revisit later if maintainers want a deeper non-marketing owner. |
| Architecture and system shape | `docs/architecture.md` | `docs/intake/inferred-architecture.md`, `AGENTS.md` | New file recommended. Existing evidence is strong but scattered. |
| Interfaces and contracts | `docs/interfaces-and-contracts.md` | controller APIs, control bus, QML/controller docs | New file recommended. No current single owner found. |
| Data model | `docs/data-model.md` | `docs/intake/inferred-data-model.md`, `res/schema.xml` | New file recommended. |
| Local development and tooling | `CONTRIBUTING.md` | `README.md`, CI workflows | Existing doc is already strong. Mention this explicitly in the ownership map to avoid duplication. |
| Testing strategy | `docs/testing-strategy.md` | `CMakeLists.txt`, `src/test/`, `.github/workflows/build.yml`, `CONTRIBUTING.md` | New file recommended. Current facts are split across code and CI. |
| Operations and runbooks | `docs/operations-runbook.md` | `packaging/`, `.github/workflows/build.yml`, platform buildenv scripts | New file recommended. No single owner found. |
| Security and privacy | `docs/security-and-privacy.md` | network/integration code, release/signing flows | New file recommended. No obvious current owner found. |
| Observability and instrumentation | `docs/observability-and-instrumentation.md` | logging, stats, benchmark/test surfaces | New file recommended. |
| Decisions and rationale | `docs/decision-log.md` | `docs/intake/inferred-decisions.md`, future ADR links | New file recommended. |
| Implementation history | `CHANGELOG.md` initially | release workflows | Existing file can stay canonical unless maintainers want finer-grained in-repo history. |
| Feature registry / next work queue | `docs/feature-registry.md` | future feature docs | New file recommended. Repo Memory continuity gap today. |
| Active feature handoff | `docs/features/<feature-slug>.md` | feature-specific evidence and next steps | New files as needed. |
| Documentation health | `docs/doc-health.md` | intake findings, future verification notes | New file recommended. |

## Recommended promotion order

1. Create `docs/README.md` and explicitly name carried-forward owners:
   - `README.md`
   - `CONTRIBUTING.md`
   - `CHANGELOG.md`
2. Promote reviewed architecture content into `docs/architecture.md`.
3. Promote reviewed data-model content into `docs/data-model.md`.
4. Create `docs/testing-strategy.md` and `docs/operations-runbook.md`.
5. Create `docs/feature-registry.md` and `docs/doc-health.md`.
6. Add `docs/decision-log.md` only after maintainers confirm which inferred
   decisions are real and worth preserving.

## Promotion checklist

Use this checklist when reviewing and promoting the intake drafts:

1. **Review every inferred claim**
   - confirm
   - correct
   - or leave as explicitly unknown
2. **Do not duplicate existing strong docs**
   - keep `CONTRIBUTING.md` as local-dev owner unless there is a clear reason
     to split ownership
   - keep `CHANGELOG.md` as implementation-history owner unless maintainers want
     a richer replacement
3. **Create an ownership map before broad doc creation**
   - start with `docs/README.md`
   - list canonical owners and supporting docs separately
4. **Promote only reviewed material**
   - architecture facts -> `docs/architecture.md`
   - data model facts -> `docs/data-model.md`
   - unresolved items -> `docs/doc-health.md` or `unknowns.md` follow-up notes
5. **Keep intake docs as evidence**
   - do not rewrite history by deleting them immediately
   - mark promoted sections or superseding docs once canonical docs exist
6. **Update agent entrypoints after adoption**
   - point `AGENTS.md` to `docs/README.md` once the ownership map exists
7. **Validate the docs set**
   - preferred path: run the Repo Memory validator for the adopted level
   - fix duplicate ownership, broken links, and missing canonical owners before
     claiming adoption

## Smallest reasonable adoption for this repo

If maintainers want the minimal useful Repo Memory footprint first, the
recommended first canonical set is:

```text
docs/
├── README.md
├── architecture.md
├── data-model.md
├── testing-strategy.md
├── feature-registry.md
└── doc-health.md
```

with explicit reuse of:

- `README.md` for overview
- `CONTRIBUTING.md` for local development
- `CHANGELOG.md` for implementation history

That would satisfy the “complement, do not replace” goal while creating the
missing shared handoff surfaces.