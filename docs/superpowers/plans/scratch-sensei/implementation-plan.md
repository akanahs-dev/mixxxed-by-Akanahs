# Scratch Sensei V1 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build Scratch Sensei V1 as a Mixxx-native loaded-deck Prepare Track workspace that uses Essentia-backed analysis, stores suggestions in analysis metadata, and lets the user manually create normal Mixxx cues/hotcues.

**Architecture:** Add a clean Scratch Sensei module under Mixxx's existing analyzer, analysis-cache, library-feature, and QWidget-library patterns. Reuse `track_analysis` / `AnalysisDao` for the stored artifact, run analysis only from an explicit Scratch Sensei action, and keep generated suggestions separate from user cue intent.

**Tech Stack:** C++17, Qt Widgets, Mixxx analyzer worker infrastructure, `AnalysisDao`, protobuf-lite, CMake, Essentia standard C++ API, GTest.

---

## Clean-Room Boundary

Do not inspect, copy, or reference old Scratch Sensei implementation code. This plan is based only on:

- `docs/superpowers/plans/scratch-sensei/spec.md`
- `docs/superpowers/plans/scratch-sensei/design.md`
- `docs/adr/0001-scratch-sensei-native-rebuild.md`
- `docs/adr/0002-scratch-sensei-required-essentia.md`
- `docs/adr/0003-scratch-sensei-v1-scope.md`
- `docs/adr/0004-scratch-sensei-analysis-summary-only.md`
- Current Mixxx architecture and contribution docs.
- Current Mixxx source surfaces listed in this plan.

The implementation must not add Practice Skills, lesson UI, generated cue rows, cue schema ownership fields, external DJ export, automatic deck control, library-row context, track-pair workflows, or automatic background Scratch Sensei analysis.

---

## Plan Decisions

These choices are fixed for V1 implementation:

- **Active deck:** Do not infer focus state in V1. If one visual deck has a track, use it. If multiple visual decks have tracks, show an explicit deck selector. If none have tracks, show the no-deck empty state.
- **Analysis trigger:** Scratch Sensei analysis is manual only from the Prepare Track UI. It must not run from normal track-load analysis, batch analysis, or library-row selection.
- **Analyzer integration:** Add a Scratch Sensei-only analyzer mode to `AnalyzerThread` so the existing decode/progress/threading path can be reused without running BPM, key, ReplayGain, waveform, or silence analyzers.
- **Storage:** Store one `ScratchSenseiTrackAnalysis` artifact per track in `track_analysis` through `AnalysisDao`. Re-analysis replaces only the Scratch Sensei artifact for that track.
- **Payload format:** Use protobuf under `src/proto/scratchsensei.proto`, matching Mixxx's existing protobuf build for waveform/beats/keys. Keep schema version and analyzer version inside the payload and in the `AnalysisDao::AnalysisInfo::version` string.
- **Essentia:** `SCRATCH_SENSEI=ON` requires Essentia. If Essentia is not found when enabled, CMake fails with install/build guidance. No fallback RMS-only analyzer is allowed.
- **Test provider boundary:** A fake provider may exist only in unit tests or dependency-injected test binaries. No user-visible app build, manual smoke, or production code path may analyze with fake provider output.
- **First descriptor set:** V1 stores normalized energy curve, spectral flux curve, onset-density curve, beat-strength curve, section candidates, suggestions, evidence, and capability flags.
- **Initial Essentia algorithm family:** Use source audio decoded by Mixxx and feed Essentia standard algorithms for frame/spectrum energy, onset functions, spectral flux, and rhythm/beat confidence. Confirm exact API names against the installed Essentia headers during Task 1, with `RhythmExtractor2013`, `OnsetDetection`, `Spectrum`, `RMS`, and flux/spectral change descriptors as the first target family.
- **Cue promotion:** `Create Cue` creates a normal Mixxx cue with no hotcue index. `Promote to Hotcue` creates a normal Mixxx hotcue only after the user chooses/accepts an available hotcue slot.
- **Re-analysis semantics:** The Scratch Sensei Analyze and Re-analyze buttons always replace only the Scratch Sensei artifact for the selected track. Do not add a separate analyzer option unless implementation proves the existing manual action cannot express replacement.
- **V1 memory posture:** Full-track mono buffering is acceptable only with an explicit memory/duration guard. Long tracks should fail safely with a low-confidence/unsupported message or use a streaming/downsampled accumulator before shipping.

---

## Source Anchors

Use these files as the implementation map:

| Area | Existing source anchor | Why it matters |
| --- | --- | --- |
| Analyzer interface | `src/analyzer/analyzer.h` | Base contract for initialize/process/store/cleanup. |
| Analyzer worker | `src/analyzer/analyzerthread.h`, `src/analyzer/analyzerthread.cpp` | Decode loop, analyzer mode flags, progress, thread boundaries. |
| Scheduler | `src/analyzer/trackanalysisscheduler.h`, `src/analyzer/trackanalysisscheduler.cpp` | Manual one-track queue and progress signals. |
| Loaded-track analysis | `src/mixer/playermanager.cpp` | Existing automatic analysis path to avoid for Scratch Sensei. |
| Analysis storage | `src/library/dao/analysisdao.h`, `src/library/dao/analysisdao.cpp`, `res/schema.xml` | Existing `track_analysis` table and compressed file-backed payload storage. |
| Protobuf | `src/proto/CMakeLists.txt`, `src/proto/waveform.proto` | Existing protobuf-lite generation pattern. |
| Library feature pattern | `src/library/recording/recordingfeature.cpp`, `src/library/analysis/analysisfeature.cpp` | Sidebar feature and `WLibrary::registerView` pattern. |
| Library registration | `src/library/library.h`, `src/library/library.cpp` | Add Scratch Sensei feature and pass `PlayerManager`. |
| Deck access | `src/mixer/playermanager.h`, `src/mixer/basetrackplayer.h` | Resolve loaded deck tracks through `getDeckBase()` / `getLoadedTrack()`. |
| Cue writing | `src/track/track.h`, `src/track/track.cpp`, `src/track/cue.h` | `Track::createAndAddCue()` is the cue promotion surface. |
| Build sources | `CMakeLists.txt`, `cmake/modules/` | Add files, option, find module, and links. |

---

## File Structure

Create these files:

- `cmake/modules/FindEssentia.cmake`
- `src/proto/scratchsensei.proto`
- `src/analyzer/scratchsensei/scratchsenseianalyzer.h`
- `src/analyzer/scratchsensei/scratchsenseianalyzer.cpp`
- `src/analyzer/scratchsensei/scratchsenseianalysisprovider.h`
- `src/analyzer/scratchsensei/scratchsenseiessentiaprovider.h`
- `src/analyzer/scratchsensei/scratchsenseiessentiaprovider.cpp`
- `src/analyzer/scratchsensei/scratchsenseifakeprovider.h`
- `src/analyzer/scratchsensei/scratchsenseisummary.h`
- `src/analyzer/scratchsensei/scratchsenseisummary.cpp`
- `src/analyzer/scratchsensei/scratchsenseisuggestionbuilder.h`
- `src/analyzer/scratchsensei/scratchsenseisuggestionbuilder.cpp`
- `src/analyzer/scratchsensei/scratchsenseianalysisdao.h`
- `src/analyzer/scratchsensei/scratchsenseianalysisdao.cpp`
- `src/library/scratchsensei/scratchsenseifeature.h`
- `src/library/scratchsensei/scratchsenseifeature.cpp`
- `src/library/scratchsensei/dlgscratchsensei.h`
- `src/library/scratchsensei/dlgscratchsensei.cpp`
- `src/library/scratchsensei/dlgscratchsensei.ui`
- `src/test/scratchsenseisummary_test.cpp`
- `src/test/scratchsenseisuggestionbuilder_test.cpp`
- `src/test/scratchsenseianalysisdao_test.cpp`
- `src/test/scratchsenseideckresolver_test.cpp`
- `src/test/scratchsenseiprovider_test.cpp`

Modify these files:

- `CMakeLists.txt`
- `src/proto/CMakeLists.txt`
- `src/analyzer/analyzerthread.h`
- `src/analyzer/analyzerthread.cpp`
- `src/library/dao/analysisdao.h`
- `src/library/dao/analysisdao.cpp`
- `src/library/library.h`
- `src/library/library.cpp`
- `docs/features/scratch-sensei.md`
- `docs/doc-health.md`

Avoid changing these files in V1 unless a compile error proves it is necessary:

- `res/schema.xml`
- `src/library/dao/cuedao.*`
- `src/track/cue.*`
- `src/track/track.*`
- `src/engine/`
- `res/skins/`
- `res/qml/`

---

## Validation Commands

Use the local build directory if it already exists. Otherwise create one with the macOS build environment sourced first.

```bash
source tools/macos_buildenv.sh
cmake -S . -B build -DSCRATCH_SENSEI=ON -DBUILD_TESTING=ON
cmake --build build --target mixxx-test --parallel "$(sysctl -n hw.ncpu)"
ctest --test-dir build --output-on-failure -R "ScratchSensei|AnalysisDao"
```

For docs after every phase:

```bash
python3 /Users/ipad/.gemini/config/skills/repo-memory/scripts/validate-docs.py --project-docs /Users/ipad/github/mixxxed-by-Akanahs --adoption-level continuity --strict
npx --yes markdownlint-cli2 "docs/features/scratch-sensei.md" "docs/doc-health.md" "docs/superpowers/plans/scratch-sensei/**/*.md" "docs/adr/*.md"
```

Manual UI smoke comes after compile/tests and before claiming the feature is usable:

```bash
./build/mixxx
```

Smoke path:

1. Load one track into Deck 1.
2. Open Scratch Sensei from the library/sidebar.
3. Confirm the header says Deck 1 and the track title.
4. Click Analyze with Scratch Sensei.
5. Confirm progress, analyzed state, energy curve, suggestions, and selected-suggestion details.
6. Click Jump and confirm transport seeks to the suggestion without starting playback unexpectedly.
7. Click Create Cue and confirm a normal cue appears for the track.
8. Re-analyze and confirm the user cue remains.
9. Load tracks into two decks and confirm the selector appears.
10. Eject both decks and confirm the no-deck empty state appears.

---

## Task 0: Preflight And Guardrails

**Files:**

- Read: `docs/superpowers/plans/scratch-sensei/spec.md`
- Read: `docs/superpowers/plans/scratch-sensei/design.md`
- Read: `CONTRIBUTING.md`
- Read: `AGENTS.md`
- Read: `docs/architecture.md`
- Read: `docs/interfaces-and-contracts.md`

- [ ] **Step 1: Confirm branch and diff scope**

Run:

```bash
git branch --show-current
git status --short
```

Expected:

- Current branch is the Scratch Sensei fresh-spec branch or a new implementation branch forked from it.
- Existing changes are docs/spec changes only before code work begins.

- [ ] **Step 2: Confirm no old Scratch Sensei code is used**

Run:

```bash
rg -n "ScratchSensei|Scratch Sensei|scratchsensei|scratch-sensei" src res CMakeLists.txt
```

Expected before implementation:

- No old implementation files are used as source material.
- If the command finds old Scratch Sensei implementation code, stop and ask the user whether to delete it before continuing.

- [ ] **Step 3: Run docs validation before coding**

Run:

```bash
python3 /Users/ipad/.gemini/config/skills/repo-memory/scripts/validate-docs.py --project-docs /Users/ipad/github/mixxxed-by-Akanahs --adoption-level continuity --strict
npx --yes markdownlint-cli2 "docs/features/scratch-sensei.md" "docs/doc-health.md" "docs/superpowers/plans/scratch-sensei/**/*.md" "docs/adr/*.md"
```

Expected:

- Repo-memory validation passes.
- Markdownlint passes.

---

## Task 1: Essentia Build Gate

**Files:**

- Create: `cmake/modules/FindEssentia.cmake`
- Modify: `CMakeLists.txt`
- Test: CMake configure with `-DSCRATCH_SENSEI=ON`

- [ ] **Step 1: Add Essentia discovery**

Create `cmake/modules/FindEssentia.cmake` with config-package, pkg-config, and manual path support.

The module must expose:

- `Essentia_FOUND`
- `Essentia_INCLUDE_DIRS`
- `Essentia_LIBRARIES`
- Imported target `Essentia::essentia`

The module must search:

- `find_package(essentia CONFIG QUIET)`
- `pkg_check_modules(PC_Essentia QUIET essentia)`
- `ESSENTIA_ROOT`, if supplied by the user.

- [ ] **Step 2: Add build option**

Modify `CMakeLists.txt` near the other optional feature flags:

```cmake
option(SCRATCH_SENSEI "Enable Scratch Sensei Essentia analysis" ON)
```

Add Essentia discovery after the option is defined but before Scratch Sensei
source files are compiled:

```cmake
if(SCRATCH_SENSEI)
  find_package(Essentia REQUIRED)
endif()
```

Add target definitions and linking only after `mixxx-lib` has been created:

```cmake
if(SCRATCH_SENSEI)
  target_compile_definitions(mixxx-lib PUBLIC __SCRATCH_SENSEI__)
  target_link_libraries(mixxx-lib PRIVATE Essentia::essentia)
endif()
```

Expected behavior:

- `-DSCRATCH_SENSEI=ON` fails at configure time if Essentia is missing.
- `-DSCRATCH_SENSEI=OFF` keeps unrelated Mixxx builds possible.
- No code path silently swaps in a non-Essentia analyzer.
- CMake does not call `target_compile_definitions()` or `target_link_libraries()` before `mixxx-lib` exists.

- [ ] **Step 3: Decide and document macOS Essentia install path**

Before implementing provider code, choose one supported local development path:

- Preferred for this fork: `ESSENTIA_ROOT=/path/to/essentia/install` pointing to headers and libraries built outside Mixxx.
- Acceptable fallback: Homebrew or local package manager path if `FindEssentia.cmake` discovers it reliably.
- Do not vendor Essentia source into Mixxx in V1 unless the user explicitly approves the repository size, license, and build-time cost.

Document the chosen command in this plan or `docs/features/scratch-sensei.md`
before moving beyond Task 1. The command must include the exact CMake configure
shape, for example:

```bash
source tools/macos_buildenv.sh
ESSENTIA_ROOT=/absolute/path/to/essentia/install cmake -S . -B build -DSCRATCH_SENSEI=ON -DBUILD_TESTING=ON
```

Expected:

- The implementation agent knows how this fork discovers Essentia on macOS.
- Missing Essentia has one obvious fix path.

- [ ] **Step 4: Verify configure failure is loud**

Temporarily configure without Essentia discoverable:

```bash
cmake -S . -B build-scratch-sensei-missing -DSCRATCH_SENSEI=ON
```

Expected:

- CMake fails.
- Error text says Essentia is required for Scratch Sensei and points to `ESSENTIA_ROOT` or the buildenv package path.

- [ ] **Step 5: Verify configure success with Essentia**

After installing or pointing to Essentia:

```bash
source tools/macos_buildenv.sh
ESSENTIA_ROOT=/absolute/path/to/essentia/install cmake -S . -B build -DSCRATCH_SENSEI=ON -DBUILD_TESTING=ON
```

Expected:

- CMake configures.
- Build output shows `SCRATCH_SENSEI` enabled.
- Build output shows which Essentia include/library path was selected.

---

## Task 2: Analysis Contract And Serialization

**Files:**

- Create: `src/proto/scratchsensei.proto`
- Modify: `src/proto/CMakeLists.txt`
- Create: `src/analyzer/scratchsensei/scratchsenseisummary.h`
- Create: `src/analyzer/scratchsensei/scratchsenseisummary.cpp`
- Create: `src/test/scratchsenseisummary_test.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Add protobuf schema**

Create `src/proto/scratchsensei.proto`:

```proto
syntax = "proto2";

package mixxx.track.io;

option optimize_for = LITE_RUNTIME;

message ScratchSenseiTrackAnalysis {
  optional string schema_version = 1;
  optional string analyzer_version = 2;
  optional int64 track_id = 3;
  optional string audio_fingerprint = 4;
  optional double duration_seconds = 5;
  optional BeatgridSnapshot beatgrid_snapshot = 6;
  optional CapabilityFlags capabilities = 7;
  repeated DescriptorTimeline descriptor_timeline = 8;
  repeated Suggestion suggestion = 9;
}

message BeatgridSnapshot {
  optional bool available = 1;
  optional double bpm = 2;
  optional string source = 3;
  optional string fingerprint = 4;
}

message CapabilityFlags {
  optional bool has_energy_curve = 1;
  optional bool has_phrase_candidates = 2;
  optional bool has_beatgrid_snapshot = 3;
  optional bool has_reliable_tempo = 4;
  optional bool has_suggestion_evidence = 5;
  optional bool analysis_stale = 6;
  optional string low_confidence_reason = 7;
}

message DescriptorTimeline {
  optional string name = 1;
  optional double hop_seconds = 2;
  repeated double value = 3;
}

message Suggestion {
  optional string id = 1;
  optional string type = 2;
  optional string label = 3;
  optional double time_seconds = 4;
  optional int64 frame_position = 5;
  optional double sample_rate = 6;
  optional double window_seconds = 7;
  optional int32 beat_index = 8;
  optional int32 bar_index = 9;
  optional int32 phrase_index = 10;
  optional double confidence = 11;
  optional string confidence_bucket = 12;
  optional Evidence evidence = 13;
}

message Evidence {
  optional double energy_delta = 1;
  optional double spectral_flux = 2;
  optional double onset_density = 3;
  optional double beat_strength = 4;
  optional string reason = 5;
}
```

Add `scratchsensei.proto` to the `PROTOS` list in `src/proto/CMakeLists.txt`.

- [ ] **Step 2: Add C++ summary wrapper**

Create `src/analyzer/scratchsensei/scratchsenseisummary.h` with:

```cpp
#pragma once

#include <QByteArray>
#include <QString>

#include "proto/scratchsensei.pb.h"

namespace mixxx::scratchsensei {

constexpr const char* kSchemaVersion = "scratch-sensei-v1";
constexpr const char* kAnalysisDescription = "Scratch Sensei Track Analysis";
constexpr const char* kAnalysisVersionPrefix = "scratch-sensei-v1:";

QByteArray serializeTrackAnalysis(
        const mixxx::track::io::ScratchSenseiTrackAnalysis& analysis);

bool parseTrackAnalysis(
        const QByteArray& data,
        mixxx::track::io::ScratchSenseiTrackAnalysis* analysis);

QString analysisVersionForEssentiaVersion(const QString& essentiaVersion);

} // namespace mixxx::scratchsensei
```

Create `src/analyzer/scratchsensei/scratchsenseisummary.cpp` with protobuf serialization and parsing that returns `false` for empty or invalid payloads.

- [ ] **Step 3: Add serialization tests**

Create `src/test/scratchsenseisummary_test.cpp`:

```cpp
#include <gtest/gtest.h>

#include "analyzer/scratchsensei/scratchsenseisummary.h"

namespace {

TEST(ScratchSenseiSummaryTest, RoundTripsMinimalPayload) {
    mixxx::track::io::ScratchSenseiTrackAnalysis analysis;
    analysis.set_schema_version(mixxx::scratchsensei::kSchemaVersion);
    analysis.set_analyzer_version("essentia-test");
    analysis.set_track_id(42);
    analysis.set_duration_seconds(180.0);

    const QByteArray bytes = mixxx::scratchsensei::serializeTrackAnalysis(analysis);
    ASSERT_FALSE(bytes.isEmpty());

    mixxx::track::io::ScratchSenseiTrackAnalysis parsed;
    ASSERT_TRUE(mixxx::scratchsensei::parseTrackAnalysis(bytes, &parsed));
    EXPECT_EQ(parsed.schema_version(), mixxx::scratchsensei::kSchemaVersion);
    EXPECT_EQ(parsed.analyzer_version(), "essentia-test");
    EXPECT_EQ(parsed.track_id(), 42);
    EXPECT_DOUBLE_EQ(parsed.duration_seconds(), 180.0);
}

TEST(ScratchSenseiSummaryTest, RejectsInvalidPayload) {
    mixxx::track::io::ScratchSenseiTrackAnalysis parsed;
    EXPECT_FALSE(mixxx::scratchsensei::parseTrackAnalysis(QByteArray("bad"), &parsed));
}

} // namespace
```

- [ ] **Step 4: Run summary tests**

Run:

```bash
cmake --build build --target mixxx-test --parallel "$(sysctl -n hw.ncpu)"
ctest --test-dir build --output-on-failure -R ScratchSenseiSummary
```

Expected:

- Summary tests pass.
- `mixxx-proto` regenerates with `scratchsensei.pb.h`.

---

## Task 3: AnalysisDao Storage Type And Replacement

**Files:**

- Modify: `src/library/dao/analysisdao.h`
- Modify: `src/library/dao/analysisdao.cpp`
- Create: `src/analyzer/scratchsensei/scratchsenseianalysisdao.h`
- Create: `src/analyzer/scratchsensei/scratchsenseianalysisdao.cpp`
- Create: `src/test/scratchsenseianalysisdao_test.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Add analysis type**

Modify `AnalysisDao::AnalysisType`:

```cpp
enum AnalysisType {
    TYPE_UNKNOWN = 0,
    TYPE_WAVEFORM,
    TYPE_WAVESUMMARY,
    TYPE_SCRATCHSENSEI_TRACK_ANALYSIS
};
```

Do not add a SQL migration. The existing `track_analysis.type` column stores the numeric type value already.

- [ ] **Step 2: Add type-delete helper**

Add to `AnalysisDao`:

```cpp
bool deleteAnalysesForTrackByType(TrackId trackId, AnalysisType type);
```

Implement it by loading analyses for the track/type and calling `deleteAnalysis()` for each `analysisId`. Keep the file deletion path inside `deleteAnalysis()`.

- [ ] **Step 3: Add Scratch Sensei DAO wrapper**

Create `ScratchSenseiAnalysisDao` with:

```cpp
namespace mixxx::scratchsensei {

class ScratchSenseiAnalysisDao {
  public:
    explicit ScratchSenseiAnalysisDao(AnalysisDao* analysisDao);

    bool saveReplacing(
            TrackId trackId,
            const QString& version,
            const QByteArray& payload);

    bool loadLatest(
            TrackId trackId,
            AnalysisDao::AnalysisInfo* analysis) const;

  private:
    AnalysisDao* const m_pAnalysisDao;
};

} // namespace mixxx::scratchsensei
```

Rules:

- `saveReplacing()` deletes existing `TYPE_SCRATCHSENSEI_TRACK_ANALYSIS` records for the track, then saves the new artifact.
- `description` is `Scratch Sensei Track Analysis`.
- `version` is `scratch-sensei-v1:<essentia-provider-version>`.
- `loadLatest()` returns the newest matching artifact if multiple exist after an interrupted old run.

- [ ] **Step 4: Add DAO tests**

Create tests that use the same test database pattern as existing DAO tests in `src/test`.

Required assertions:

- Saving a Scratch Sensei artifact creates one `track_analysis` row for the track/type.
- Saving a second Scratch Sensei artifact for the same track replaces the first.
- Waveform and wavesummary analysis rows for the same track are not deleted.
- Loading invalid or missing analysis returns `false`.

- [ ] **Step 5: Run DAO tests**

Run:

```bash
ctest --test-dir build --output-on-failure -R ScratchSenseiAnalysisDao
```

Expected:

- Tests pass.
- No schema migration is required.

---

## Task 4: Suggestion Builder With Fake Provider

**Files:**

- Create: `src/analyzer/scratchsensei/scratchsenseianalysisprovider.h`
- Create: `src/analyzer/scratchsensei/scratchsenseifakeprovider.h`
- Create: `src/analyzer/scratchsensei/scratchsenseisuggestionbuilder.h`
- Create: `src/analyzer/scratchsensei/scratchsenseisuggestionbuilder.cpp`
- Create: `src/test/scratchsenseisuggestionbuilder_test.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Define provider output**

Create provider structs in `scratchsenseianalysisprovider.h`:

```cpp
#pragma once

#include <span>

#include <QString>
#include <QVector>

namespace mixxx::scratchsensei {

struct DescriptorSeries {
    QString name;
    double hopSeconds;
    QVector<double> values;
};

struct ProviderResult {
    double durationSeconds = 0.0;
    double bpm = 0.0;
    double beatConfidence = 0.0;
    QVector<double> beatTimesSeconds;
    QVector<DescriptorSeries> timelines;
};

class ScratchSenseiAnalysisProvider {
  public:
    virtual ~ScratchSenseiAnalysisProvider() = default;

    virtual QString providerVersion() const = 0;
    virtual ProviderResult analyzeMonoSamples(
            std::span<const float> monoSamples,
            double sampleRate) = 0;
};

} // namespace mixxx::scratchsensei
```

- [ ] **Step 2: Add fake provider**

Create `ScratchSenseiFakeProvider` that returns deterministic curves:

- `energy_norm`: `[0.1, 0.2, 0.8, 0.9, 0.3]`
- `spectral_flux_norm`: `[0.0, 0.1, 0.9, 0.4, 0.1]`
- `onset_density_norm`: `[0.1, 0.2, 0.7, 0.8, 0.2]`
- `beat_strength_norm`: `[0.8, 0.8, 0.9, 0.9, 0.6]`
- `hopSeconds`: `8.0`
- `bpm`: `128.0`
- `beatConfidence`: `0.9`

Use this provider for unit tests only. Production builds and manual smoke must use Essentia or fail loudly. If UI development needs deterministic data, inject the fake provider only in a test binary or unit test harness, never behind `SCRATCH_SENSEI=ON` in the normal app.

- [ ] **Step 3: Build suggestions from descriptors**

Implement `ScratchSenseiSuggestionBuilder` that:

- Copies descriptor timelines into `ScratchSenseiTrackAnalysis`.
- Sets capability flags from available timelines.
- Produces stable suggestion IDs using `trackId`, suggestion type, and rounded frame position.
- Emits at least these suggestion types:
  - `energy_lift`
  - `energy_dip`
  - `section_change`
  - `phrase_change`
- Uses confidence buckets:
  - `Strong` for confidence `>= 0.75`
  - `Maybe` for confidence `>= 0.45`
  - `Weak` below `0.45`

Do not encode lesson names, practice scoring, or curriculum terms into suggestions.

- [ ] **Step 4: Add suggestion tests**

Required assertions:

- Fake provider data creates at least one `energy_lift` suggestion near the rising energy point.
- Suggestion IDs are stable for the same input.
- Missing beatgrid marks phrase/bar anchors unavailable without failing the entire analysis.
- Capability flags accurately describe available timelines.
- Confidence bucket thresholds match the spec.
- The fake provider is not compiled into or selected by normal app builds.

- [ ] **Step 5: Run suggestion tests**

Run:

```bash
ctest --test-dir build --output-on-failure -R ScratchSenseiSuggestionBuilder
```

Expected:

- Tests pass with fake provider only.

---

## Task 5: Essentia Provider

**Files:**

- Create: `src/analyzer/scratchsensei/scratchsenseiessentiaprovider.h`
- Create: `src/analyzer/scratchsensei/scratchsenseiessentiaprovider.cpp`
- Modify: `CMakeLists.txt`
- Test: `src/test/scratchsenseiprovider_test.cpp`

- [ ] **Step 1: Initialize Essentia safely**

Implement a provider that owns Essentia algorithm instances inside the worker thread. Do not use shared global mutable algorithm instances across tracks.

Provider lifecycle:

- Constructor initializes provider metadata.
- `analyzeMonoSamples()` creates or resets algorithm instances for one track.
- Destructor releases provider-owned algorithm objects.

- [ ] **Step 2: Produce normalized descriptor timelines**

The provider returns normalized descriptor names, not raw Essentia names:

- `energy_norm`
- `spectral_flux_norm`
- `onset_density_norm`
- `beat_strength_norm`

Rules:

- Values are clamped to `[0.0, 1.0]`.
- Empty or corrupt audio returns an empty `ProviderResult` with `durationSeconds` set when known.
- Descriptor timelines use seconds as the UI-facing anchor.
- Frame positions are calculated later from track sample rate and `timeSeconds`.
- Provider code accepts `std::span<const CSAMPLE>` or `std::vector<float>` only at the provider boundary. Avoid `QVector<float>` for full-track sample buffers in the analyzer worker.

- [ ] **Step 3: Use the first Essentia algorithm family**

Use the installed Essentia C++ headers to wire:

- Spectrum input for spectral descriptors.
- RMS or energy for `energy_norm`.
- Flux or onset-detection function for `spectral_flux_norm`.
- Onset functions aggregated per window for `onset_density_norm`.
- `RhythmExtractor2013` beat positions/confidence for beat strength context.

Do not ask Essentia to overwrite Mixxx BPM, key, waveform, ReplayGain, or silence values.

- [ ] **Step 4: Add provider boundary tests**

Use synthetic vectors:

- Silence: all descriptor values are zero or absent, no crash.
- Single energy jump: energy curve rises and creates input suitable for `energy_lift`.
- Short audio: provider returns a valid empty/low-confidence result without throwing.

Use fake provider tests for deterministic suggestion logic. Essentia provider tests should prove safety and shape, not exact musical judgment.

- [ ] **Step 5: Run provider tests**

Run:

```bash
ctest --test-dir build --output-on-failure -R "ScratchSensei.*Provider"
```

Expected:

- Provider tests pass.
- No test requires a full music file.
- Tests prove fake provider is test-only and not selected by `SCRATCH_SENSEI=ON` production code.

---

## Task 6: Scratch Sensei Analyzer Worker

**Files:**

- Create: `src/analyzer/scratchsensei/scratchsenseianalyzer.h`
- Create: `src/analyzer/scratchsensei/scratchsenseianalyzer.cpp`
- Modify: `src/analyzer/analyzerthread.h`
- Modify: `src/analyzer/analyzerthread.cpp`
- Modify: `CMakeLists.txt`
- Test: analyzer initialization and storage with fake provider

- [ ] **Step 1: Add analyzer mode flag**

Modify `AnalyzerModeFlags`:

```cpp
enum AnalyzerModeFlags {
    None = 0x00,
    WithBeats = 0x01,
    WithWaveform = 0x02,
    LowPriority = 0x04,
    ScratchSenseiOnly = 0x08,
    All = WithBeats | WithWaveform,
};
```

Rule:

- Existing callers keep their current behavior.
- Scratch Sensei caller uses `ScratchSenseiOnly | LowPriority`.

- [ ] **Step 2: Gate analyzer construction**

Update `AnalyzerThread::doRun()`:

- Acquire a thread-local database connection when `WithWaveform` or `ScratchSenseiOnly` is set.
- If `ScratchSenseiOnly` is set, construct only `AnalyzerScratchSensei`.
- If `ScratchSenseiOnly` is not set, keep current waveform/gain/EBU/beats/key/silence behavior.

This prevents a manual Scratch Sensei analysis from changing normal BPM, key, ReplayGain, waveform, or silence results.

- [ ] **Step 3: Implement `AnalyzerScratchSensei`**

Responsibilities:

- `initialize()` checks valid track, valid duration, provider availability, and whether re-analysis is requested by the caller.
- `processSamples()` downmixes analysis samples into a mono buffer or descriptor accumulator for the provider.
- `storeResults()` runs provider analysis, builds the protobuf artifact, and saves it through `ScratchSenseiAnalysisDao`.
- `cleanup()` clears sample buffers and pending state.

Do not run Essentia on the real-time audio engine thread. This analyzer runs inside analyzer worker threads only.

Memory guard:

- Track the number of buffered mono samples and estimated bytes.
- Set a V1 ceiling before shipping, such as 256 MB of mono samples or a duration limit chosen during implementation.
- If the selected track exceeds the ceiling and a streaming accumulator is not implemented, save a failed/low-confidence artifact or surface a clear UI failure message instead of exhausting memory.
- Prefer a streaming or windowed descriptor accumulator if Essentia APIs make it straightforward.

- [ ] **Step 4: Add analyzer tests**

Required assertions with fake provider:

- Valid track stores `TYPE_SCRATCHSENSEI_TRACK_ANALYSIS`.
- Re-analysis replaces only the Scratch Sensei artifact.
- ScratchSenseiOnly mode does not create waveform, wavesummary, key, BPM, ReplayGain, or silence updates.
- Cleanup clears buffered samples after failure.
- A track exceeding the V1 memory/duration guard fails safely and clears buffers.

- [ ] **Step 5: Run analyzer tests**

Run:

```bash
ctest --test-dir build --output-on-failure -R "ScratchSensei.*Analyzer"
```

Expected:

- Tests pass.
- Existing analyzer tests still pass:

```bash
ctest --test-dir build --output-on-failure -R "Analyzer|Waveform"
```

---

## Task 7: Library Feature And Deck Resolver

**Files:**

- Create: `src/library/scratchsensei/scratchsenseifeature.h`
- Create: `src/library/scratchsensei/scratchsenseifeature.cpp`
- Create: `src/test/scratchsenseideckresolver_test.cpp`
- Modify: `src/library/library.h`
- Modify: `src/library/library.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Add deck resolver**

Implement resolver logic in `ScratchSenseiFeature` or a small internal helper:

- Iterate visual decks from `PlayerManagerInterface::numberOfDecks()`.
- For each deck, call `getDeckBase(index)` and `getLoadedTrack()`.
- Build a list of loaded deck contexts with:
  - deck index
  - group string
  - track id
  - display title
  - `TrackPointer`

V1 behavior:

- Zero loaded decks: emit no-deck state.
- One loaded deck: select it automatically.
- Multiple loaded decks: show selector and wait for user choice.

- [ ] **Step 2: Add deck resolver tests**

Use a fake `PlayerManagerInterface`.

Required assertions:

- No loaded deck returns no-deck state.
- One loaded deck auto-selects that deck.
- Two loaded decks requires selection.
- Preview decks and samplers are ignored for V1.

- [ ] **Step 3: Add library feature**

Follow the `RecordingFeature` / `AnalysisFeature` pattern:

- `ScratchSenseiFeature : public LibraryFeature`
- Constructor takes `Library*`, `UserSettingsPointer`, and `PlayerManagerInterface*`.
- `title()` returns `tr("Scratch Sensei")`.
- `bindLibraryWidget()` creates `DlgScratchSensei`, installs keyboard event filter, registers the view, and connects feature signals.
- `activate()` emits `switchToView(kScratchSenseiViewName)` and disables cover art display for this view.

- [ ] **Step 4: Register feature in Library**

Modify `Library`:

- Add forward declaration and member pointer for `ScratchSenseiFeature`.
- In `Library::Library(...)`, create the feature after `AnalysisFeature` or near other preparation features.
- Pass the existing `pPlayerManager` pointer.
- Add the feature with `addFeature(...)`.

- [ ] **Step 5: Run feature tests**

Run:

```bash
ctest --test-dir build --output-on-failure -R ScratchSenseiDeckResolver
cmake --build build --target mixxx-lib --parallel "$(sysctl -n hw.ncpu)"
```

Expected:

- Deck resolver tests pass.
- `mixxx-lib` compiles with the new library feature.

---

## Task 8: Prepare Track UI Shell

**Files:**

- Create: `src/library/scratchsensei/dlgscratchsensei.h`
- Create: `src/library/scratchsensei/dlgscratchsensei.cpp`
- Create: `src/library/scratchsensei/dlgscratchsensei.ui`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Add UI states**

Implement these states in `DlgScratchSensei`:

- `NoDeck`
- `AmbiguousDeck`
- `NotAnalyzed`
- `Analyzing`
- `Analyzed`
- `Stale`
- `Failed`
- `Cancelled`

Visible copy:

- No deck: `Load a track to a deck to use Scratch Sensei.`
- Ambiguous deck: `Choose which loaded deck to inspect.`
- Not analyzed: `Scratch Sensei has not analyzed this track yet.`
- Analyzing: `Listening for structure, energy, and phrase clues.`
- Analyzed: `Suggestions are ready for review.`
- Stale: `Track or beatgrid changed since analysis.`
- Failed: `Scratch Sensei could not analyze this track.`

- [ ] **Step 2: Add fixed layout**

The `.ui` file should contain:

- Header label for deck and track identity.
- Analyze/Re-analyze button.
- Cancel button shown only while analyzing.
- Deck selector shown only for multiple loaded decks.
- Timeline area widget.
- Selected suggestion detail panel.
- Buttons: Jump, Create Cue, Promote to Hotcue, Hide.

Avoid table views in V1.

- [ ] **Step 3: Connect UI to feature**

Signals:

- `analyzeRequested(TrackId trackId)`
- `cancelRequested()`
- `deckSelected(int deckIndex)`
- `jumpRequested(double timeSeconds)`
- `createCueRequested(QString suggestionId)`
- `promoteHotcueRequested(QString suggestionId)`
- `hideSuggestionRequested(QString suggestionId)`

Slots:

- `setDeckContexts(...)`
- `setAnalysisState(...)`
- `setAnalysisPayload(...)`
- `setAnalysisProgress(AnalyzerProgress progress)`

- [ ] **Step 4: Compile UI shell**

Run:

```bash
cmake --build build --target mixxx-lib --parallel "$(sysctl -n hw.ncpu)"
```

Expected:

- UI compiles.
- Scratch Sensei appears in the sidebar when `SCRATCH_SENSEI=ON`.
- UI shows no-deck state without crashing.

---

## Task 9: Manual Analysis Flow

**Files:**

- Modify: `src/library/scratchsensei/scratchsenseifeature.h`
- Modify: `src/library/scratchsensei/scratchsenseifeature.cpp`
- Modify: `src/library/scratchsensei/dlgscratchsensei.cpp`

- [ ] **Step 1: Create one-track scheduler**

When the user clicks Analyze/Re-analyze:

- Resolve selected deck context to `TrackId`.
- Create a one-worker `TrackAnalysisScheduler` with `ScratchSenseiOnly | LowPriority`.
- Schedule only the selected track.
- Connect scheduler progress to the UI.
- Disable Analyze while running.

Do not connect Scratch Sensei analysis to `PlayerManager::newTrackLoaded`.

- [ ] **Step 2: Handle cancellation**

On Cancel or selected deck track change:

- Stop the scheduler.
- Mark state `Cancelled` if the same track remains loaded.
- Mark state `NotAnalyzed` or `NoDeck` if context changed.

- [ ] **Step 3: Load stored result after completion**

When scheduler finishes:

- Load `TYPE_SCRATCHSENSEI_TRACK_ANALYSIS` through `ScratchSenseiAnalysisDao`.
- Parse protobuf.
- Verify track id, schema version, analyzer version prefix, duration, and beatgrid snapshot.
- Show `Analyzed` if valid.
- Show `Failed` if no valid artifact was produced.

- [ ] **Step 4: Keep stale state explicit**

Mark analysis stale when:

- Loaded track id differs from payload track id.
- Track duration differs beyond a small tolerance.
- Beatgrid snapshot availability/value differs from current track context.
- Analyzer schema version does not match `scratch-sensei-v1`.

- [ ] **Step 5: Run manual flow smoke with Essentia**

Run the normal app with `SCRATCH_SENSEI=ON` and Essentia available.

Do not use the fake provider for manual app smoke. Fake output is allowed only in unit tests or test harness binaries.

Expected:

- Analyze button starts progress.
- Completion loads Essentia-backed suggestions.
- Re-analysis replaces the stored artifact.
- No default Mixxx analysis values are changed by Scratch Sensei analysis.

---

## Task 10: Timeline Rendering And Suggestion Interaction

**Files:**

- Modify: `src/library/scratchsensei/dlgscratchsensei.h`
- Modify: `src/library/scratchsensei/dlgscratchsensei.cpp`
- Modify: `src/library/scratchsensei/dlgscratchsensei.ui`

- [ ] **Step 1: Render energy curve**

Add a lightweight QWidget inside `DlgScratchSensei` for the energy curve.

Rendering rules:

- Draw normalized `energy_norm` from the payload.
- Draw suggestion markers by `timeSeconds`.
- Use confidence bucket to vary marker emphasis.
- Empty descriptor timeline shows analyzed state with text: `No reliable energy curve was found for this track.`

- [ ] **Step 2: Render selected suggestion detail**

When a suggestion is selected, show:

- Label.
- Time.
- Confidence bucket.
- Evidence reason.
- Energy delta.
- Spectral flux.
- Onset density.
- Beat strength.

Do not show raw Essentia algorithm names.

- [ ] **Step 3: Implement session-only hide**

Maintain a `QSet<QString>` of hidden suggestion IDs in `DlgScratchSensei`.

Rules:

- Hide removes the marker/detail from the current UI session.
- Hide does not modify the stored protobuf payload.
- Re-opening Scratch Sensei clears hidden suggestions.

- [ ] **Step 4: Run UI interaction smoke**

Expected:

- Suggestion selection updates detail panel.
- Hide removes one suggestion only.
- Re-analysis restores suggestions from the regenerated payload.

---

## Task 11: Jump And Cue Promotion

**Files:**

- Modify: `src/library/scratchsensei/scratchsenseifeature.h`
- Modify: `src/library/scratchsensei/scratchsenseifeature.cpp`
- Modify: `src/library/scratchsensei/dlgscratchsensei.cpp`
- Test: cue safety coverage in a new or existing Scratch Sensei test

- [ ] **Step 1: Implement Jump**

Use the selected deck group and Mixxx control bus to seek to the suggestion time without starting playback.

Rules:

- Jump only affects the selected loaded deck.
- Jump is disabled if the selected deck changed or unloaded.
- Jump does not create cues.
- Jump does not start playback.

- [ ] **Step 2: Implement Create Cue**

Use the loaded `TrackPointer`:

```cpp
track->createAndAddCue(
        mixxx::CueType::Jump,
        Cue::kNoHotCue,
        framePosition,
        mixxx::audio::FramePos());
```

Rules:

- Use `Cue::kNoHotCue` for Create Cue.
- Use suggestion frame position when present.
- Convert `timeSeconds` to frame position only when a reliable sample rate is available from the payload, current track, or audio source metadata.
- Disable Create Cue and show a clear UI failure if neither frame position nor sample rate is available.
- Label/color can stay default in V1 unless existing Mixxx cue APIs require a value.
- Do not write Scratch Sensei ownership metadata to cues.

- [ ] **Step 3: Implement Promote to Hotcue**

Rules:

- Find the first available hotcue index by checking `Track::findHotcueByIndex(index)`.
- Ask the user before overwriting any occupied hotcue.
- Create a normal `mixxx::CueType::HotCue` with the selected hotcue index.
- Do not store suggestion ID in the cue.

- [ ] **Step 4: Add cue safety tests**

Required assertions:

- Create Cue increases cue count by one.
- Create Cue does not use a hotcue index.
- Promote to Hotcue uses an available hotcue index.
- Re-analysis does not delete or mutate promoted cues.
- Hiding a suggestion does not delete any cue.

- [ ] **Step 5: Run cue tests**

Run:

```bash
ctest --test-dir build --output-on-failure -R "ScratchSensei.*Cue"
```

Expected:

- Cue safety tests pass.

---

## Task 12: Final Verification And Docs Update

**Files:**

- Modify: `docs/features/scratch-sensei.md`
- Modify: `docs/doc-health.md`
- Modify: `docs/decision-log.md` only if a durable decision changes

- [ ] **Step 1: Run targeted tests**

Run:

```bash
ctest --test-dir build --output-on-failure -R "ScratchSensei|AnalysisDao|Analyzer"
```

Expected:

- Scratch Sensei tests pass.
- Existing analyzer tests pass.

- [ ] **Step 2: Build app**

Run:

```bash
cmake --build build --target mixxx --parallel "$(sysctl -n hw.ncpu)"
```

Expected:

- App builds with `SCRATCH_SENSEI=ON`.

- [ ] **Step 3: Manual smoke**

Run:

```bash
./build/mixxx
```

Complete the smoke path from the Validation Commands section.

Expected:

- UI states behave as specified.
- Analysis is manual.
- Suggestions appear.
- Create Cue and Promote to Hotcue are explicit user actions.
- Re-analysis does not delete user cues.

- [ ] **Step 4: Update feature docs**

Update `docs/features/scratch-sensei.md`:

- Mark implementation plan reviewed if the user approved it.
- Mark implemented phases accurately.
- Record any skipped validation with reason.
- Update Exact Next Prompt for the next unfinished phase.

Update `docs/doc-health.md` with validation date and commands.

- [ ] **Step 5: Run docs checks**

Run:

```bash
python3 /Users/ipad/.gemini/config/skills/repo-memory/scripts/validate-docs.py --project-docs /Users/ipad/github/mixxxed-by-Akanahs --adoption-level continuity --strict
npx --yes markdownlint-cli2 "docs/features/scratch-sensei.md" "docs/doc-health.md" "docs/superpowers/plans/scratch-sensei/**/*.md" "docs/adr/*.md"
```

Expected:

- Docs validation passes.
- Markdownlint passes.

---

## Acceptance Checklist

- [ ] `SCRATCH_SENSEI=ON` requires Essentia and fails loudly without it.
- [ ] No fallback non-Essentia analyzer exists.
- [ ] Fake provider is available only to unit tests or test harness binaries, not normal app or manual smoke builds.
- [ ] Full-track buffering has an explicit V1 memory/duration guard or streaming descriptor accumulator.
- [ ] Scratch Sensei analysis only runs after the user clicks Analyze/Re-analyze.
- [ ] Scratch Sensei analysis does not run in normal track-load or batch-analysis paths.
- [ ] Stored artifact is `ScratchSenseiTrackAnalysis`.
- [ ] Stored suggestions live only in `track_analysis` metadata.
- [ ] Re-analysis replaces only Scratch Sensei metadata.
- [ ] Re-analysis does not edit or delete user cues/hotcues.
- [ ] Loaded-deck context is one deck, explicit selector, or no-deck state.
- [ ] Multiple loaded decks do not rely on ambiguous focus state.
- [ ] UI is one Prepare Track workspace.
- [ ] No Practice Skills, lesson catalog, track pairs, exports, generated cue rows, cue source/confidence columns, or waveform scrims are implemented.
- [ ] Descriptor names are normalized Scratch Sensei terms, not raw Essentia names.
- [ ] Future training platform can consume stable suggestion IDs, capability flags, musical anchors, descriptor timelines, and evidence fields.

---

## Execution Handoff

Plan execution should happen only after user approval.

Recommended execution mode:

1. Use `superpowers:subagent-driven-development`.
2. Give one task at a time to a fresh implementation agent.
3. Review the diff and run the task's validation command before starting the next task.
4. Keep commits phase-sized after tests pass, but do not commit without user approval in this branch.

Inline execution is acceptable if the user wants one agent to keep context, but use `superpowers:executing-plans` and pause after each major phase for review.
