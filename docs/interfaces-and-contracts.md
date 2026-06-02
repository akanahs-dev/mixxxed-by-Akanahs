# Interfaces and Contracts

Doc type: interfaces-and-contracts
Owner: current-agent-or-team
Status: active
Last updated: 2026-06-02
Last verified: 2026-06-02
Verified against: res/controllers/
Confidence: high
Canonical source: `docs/interfaces-and-contracts.md`
Related docs: `README.md`, `architecture.md`

This document defines the key interface boundaries and scripting contracts used in the Mixxx ecosystem.

---

## Controller Mapping Interface

Mixxx integrates physical DJ controllers using MIDI and HID protocols. Each mapping combines two contract formats:

### 1. XML Device Descriptors (`res/controllers/*.xml`)
- Defines the controller name, protocol, and basic mapping hooks.
- Routes incoming MIDI/HID message bytes directly to core Control Object keys or script callbacks.

### 2. JavaScript Controller Scripts (`res/controllers/*.js`)
- Executed in a dedicated scripting environment (`mixxx::controllers::scripting`).
- Provides event callbacks (e.g. `init()`, `shutdown()`, value handlers).
- Interacts with the Mixxx core via global execution bindings like `engine.getValue()`, `engine.setValue()`, and `engine.beginTimer()`.

---

## Control Bus Inter-Thread Interfaces

Subsystems communicate with the core engine and UI using the control key interface:
- **Identifier format**: `[Group], Key` (e.g. `[Channel1], play`).
- UI controls, controller scripting, and mixing modules read or request changes to these keys thread-safely via `ControlObject` / `ControlProxy`.

---

## QML Presentation Bindings

For the QML UI transition path, Mixxx registers application-level C++ classes as QML singletons or standard types under the `org.mixxx.mixxx` namespace.
- Accesses domain properties directly via standard Qt property bindings (`Q_PROPERTY`).
- Communicates UI events back to the C++ core via standard QML signals.
