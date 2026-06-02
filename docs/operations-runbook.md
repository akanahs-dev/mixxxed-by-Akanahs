# Operations Runbook

Doc type: operations-runbook
Owner: current-agent-or-team
Status: active
Last updated: 2026-06-02
Last verified: 2026-06-02
Verified against: .github/workflows/build.yml
Confidence: high
Canonical source: `docs/operations-runbook.md`
Related docs: `README.md`

This document outlines compilation, packaging, and release processes for Mixxx.

---

## Local Compilation

Compilation guidelines are maintained in the main developer guide:
- Canonical owner: [`CONTRIBUTING.md`](../CONTRIBUTING.md)

---

## Continuous Integration (CI) and Builds

CI tasks run on GitHub Actions inside `.github/workflows/build.yml` on every pull request and commit:
- Performs static analysis checks (`clang-tidy`, `eslint`, `pre-commit`, `codespell`).
- Executes the standard test binary suite (`mixxx-test`).

---

## Packaging and Multi-Platform Target Releases

Mixxx distributes native installer assets across multiple operating systems. Scripts and metadata are located inside the `packaging/` directory:

### 1. Ubuntu Linux
- Builds launch using Launchpad PPA packaging.
- Supports Flatpak deployment manifests under `packaging/flatpak/`.

### 2. macOS (Intel + Apple Silicon)
- Uses buildenv scripts located in the project's build automation repositories.
- Applies standard Apple signing and notarization scripts prior to dmg creation.

### 3. Windows (x86_64 + ARM64)
- Packages installers using WiX toolsets.
- Signs binary executables using Authenticode certificates during release workflow pipelines.

### 4. Android (Experimental)
- Compiles library binaries and packages android bundles (`APK`, `AAB`) using Gradle build configs.
