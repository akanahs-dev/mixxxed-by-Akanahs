# Observability and Instrumentation

Doc type: observability-and-instrumentation
Owner: current-agent-or-team
Status: active
Last updated: 2026-06-02
Last verified: 2026-06-02
Verified against: src/effects/effectsmessenger.h
Confidence: high
Canonical source: `docs/observability-and-instrumentation.md`
Related docs: `README.md`

This document defines the logging systems, diagnostic windows, and performance benchmarks used to inspect runtime health in Mixxx.

---

## Logging Subsystem

Mixxx writes local debug execution logs to coordinate diagnostics:
- Logs are managed by custom debug wrappers.
- Messages route to the standard system console and to a local text log file (e.g. `mixxx.log`) in the user's settings profile path.
- **Rules**: Log lines must not write private credentials or stream connection passwords.

---

## Diagnostic Presentation

- **Developer / Console Window**: An in-app debug terminal console allows contributors to monitor JavaScript errors, midi parser warnings, and latency alerts in real-time.
- **Audio Thread Metrics**: Mixxx monitors engine underrun counts and audio callback latency. These metrics trigger warnings when audio interface processing exceeds real-time frame limits.
