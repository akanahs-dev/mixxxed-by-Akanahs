---
doc_type: intake-unknowns
status: draft
last_updated: 2026-06-02
last_verified: 2026-06-02
confidence: high
mode: bootstrapper
canonical: false
---

# Unknowns

This checklist consolidates important questions that were **not** answered by
the files reviewed during bootstrap.

## Product and roadmap

- [ ] What is the intended end-state of the QML rewrite?
- [ ] Which parts of the legacy QWidget/skin UI are expected to remain long
      term?
- [ ] Which hardware controller families or integrations are strategically most
      important today?
- [ ] Are there explicit non-goals for current major-version work beyond what is
      implied in code comments and branch/release practices?

## Architecture

- [ ] Is `CoreServices` the intended long-term composition root, or a temporary
      centralizer pending refactoring?
- [ ] Which interfaces are considered stable internal contracts versus purely
      implementation detail (for example controller JS APIs, QML singletons,
      control keys, DB tables)?
- [ ] What is the intended ownership boundary between `Library`,
      `TrackCollectionManager`, and the lower-level DAO classes?

## Data model and persistence

- [ ] Which DB tables should be considered stable and document-worthy versus
      migration/history detail?
- [ ] What is the exact authoritative relationship between `track_locations` and
      `library` in the current schema/runtime model?
- [ ] Which external-library tables/integrations are actively supported,
      best-effort, or effectively legacy?
- [ ] What are the backup/migration guarantees expected when users switch
      between stable/beta/main or QML/legacy paths?

## Testing and quality

- [ ] Is there an intended documented test pyramid or subsystem ownership model
      for tests, or is coverage driven mainly by contributor practice?
- [ ] Which benchmark suites matter most for release/regression decisions?
- [ ] What hidden/manual verification steps are expected for controller changes,
      UI changes, and audio engine changes beyond automated tests?

## Operations and release management

- [ ] What is the canonical runbook for release, signing, packaging, and branch
      promotion across stable/beta/main?
- [ ] Which CI failures are tolerated/known flaky versus must-fix before merge?
- [ ] Where should future operational knowledge live: existing wiki pages,
      in-repo docs, or both?

## Security and privacy

- [ ] Is there an existing security policy or threat model outside the checked-in
      repo files reviewed here?
- [ ] What telemetry, analytics, crash reporting, or external service calls are
      considered part of the product’s privacy surface?
- [ ] Which controller scripts, network features, or metadata integrations have
      explicit trust boundaries that should be documented canonically?

## Documentation ownership

- [ ] Should `README.md` remain the canonical project overview, or should a new
      `docs/project-overview.md` own deeper project scope/non-goals?
- [ ] Should `CONTRIBUTING.md` remain the canonical local-development owner, or
      should it be complemented by `docs/local-development.md` with clearer
      ownership boundaries?
- [ ] Should `CHANGELOG.md` remain the canonical implementation history owner,
      or is a separate `docs/implementation-log.md` desired?
- [ ] Which existing external docs (wiki/manual/forum/Zulip) should be linked as
      supporting sources from future in-repo canonical docs?