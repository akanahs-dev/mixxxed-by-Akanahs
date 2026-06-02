# Security and Privacy

Doc type: security-and-privacy
Owner: current-agent-or-team
Status: active
Last updated: 2026-06-02
Last verified: 2026-06-02
Verified against: src/network/
Confidence: high
Canonical source: `docs/security-and-privacy.md`
Related docs: `README.md`

This document defines the security boundaries, network integration surfaces, and privacy posture of the Mixxx codebase.

---

## Sandbox and Script Execution Boundaries

- **Controller JS Engine**: JavaScript mappings executed by the controller engine are sandboxed inside standard Qt JS environments. They do not have access to arbitrary local filesystem writes or raw system call hooks, preventing malicious controller definitions from executing arbitrary commands.

---

## Network and Telemetry Boundaries

Mixxx does not include analytics, tracking, or call-home telemetry services. Network activity is limited to explicitly configured user actions:

### 1. Broadcast Services (Icecast/Shoutcast)
- Mixxx broadcasts live audio streams directly to user-configured server addresses.
- Login details are stored locally in `mixxx.cfg` and are not shared with third-party servers.

### 2. Cover Art Fetches (Optional)
- Mixxx resolves cover art from local directories.
- Optional external metadata queries (if configured) contact official web databases directly without intermediate tracking.
