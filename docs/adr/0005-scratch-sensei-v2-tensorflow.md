# ADR 0005: Scratch Sensei V2 TensorFlow Integration

Status: accepted

For Scratch Sensei V2, we will integrate pre-trained TensorFlow models via Essentia to provide advanced music information retrieval (MIR) capabilities, such as advanced segmentation, mood tagging, and instrumentation analysis.

This requires rebuilding Essentia with TensorFlow support (`--with-tensorflow`) and linking Mixxx against `libtensorflow` / `libtensorflow_framework`.

## Context

Using machine learning models (e.g. MusiCNN, VGGish, or transfer learning classifiers) in C++ allows Scratch Sensei to move beyond simple threshold-based descriptor analysis to deep neural network classifications of song structure, energy, and genre characteristics.

However, TensorFlow models are large (several megabytes to hundreds of megabytes) and loading them can block the UI or startup threads. Additionally, compiling Essentia with TensorFlow adds a dependency on TensorFlow's C/C++ runtime library.

## Decision

1. **Essentia TensorFlow Requirement:** V2 features will be conditionally enabled or required depending on the presence of TensorFlow support in the Essentia build.
2. **Model Lifecycle and Laziness:** TensorFlow model graphs (`.pb` files) will be loaded lazily on demand when the first analysis job starts, rather than at Mixxx startup. Model execution will run entirely on the asynchronous analyzer worker thread.
3. **Model Storage:** Model files will not be checked into the source tree. Instead, they will be loaded from a user-configurable local path (defaulting to a `models` subdirectory in the Mixxx user settings folder: `~/.mixxx/models`).
4. **Graceful Degradation:** If the TensorFlow models are missing or the TensorFlow runtime fails to initialize, Scratch Sensei will fall back gracefully to V1-style classic Essentia analysis, displaying a warning in the UI rather than crashing or blocking the analysis entirely.

## Consequences

- Essentia must be recompiled with `--with-tensorflow`.
- CMake will search for TensorFlow dependencies if `SCRATCH_SENSEI=ON`.
- Protobuf schema `ScratchSenseiTrackAnalysis` will be extended to store classification vectors and confidence scores.
- Memory usage of the analyzer worker process will increase when models are loaded.
- Mixxx developer/user documentation must provide instructions on downloading and placing `.pb` files in `~/.mixxx/models/`.
