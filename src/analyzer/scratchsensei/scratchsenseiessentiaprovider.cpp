#include "analyzer/scratchsensei/scratchsenseiessentiaprovider.h"
#include <essentia/algorithmfactory.h>
#include <essentia/essentia.h>
#include <cmath>
#include <algorithm>
#include <mutex>
#include <QtDebug>

namespace {
class EssentiaGlobalManager {
  public:
    static void init() {
        std::lock_guard<std::mutex> lock(s_mutex);
        if (s_refCount == 0) {
            essentia::init();
        }
        s_refCount++;
    }

    static void shutdown() {
        std::lock_guard<std::mutex> lock(s_mutex);
        if (s_refCount > 0) {
            s_refCount--;
            if (s_refCount == 0) {
                essentia::shutdown();
            }
        }
    }

  private:
    inline static int s_refCount = 0;
    inline static std::mutex s_mutex;
};
} // namespace

namespace mixxx::scratchsensei {

ScratchSenseiEssentiaProvider::ScratchSenseiEssentiaProvider() {
    EssentiaGlobalManager::init();

    try {
        essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();
        m_rhythm = std::unique_ptr<essentia::standard::Algorithm>(factory.create("RhythmExtractor2013"));
        m_fc = std::unique_ptr<essentia::standard::Algorithm>(factory.create("FrameCutter"));
        m_win = std::unique_ptr<essentia::standard::Algorithm>(factory.create("Windowing"));
        m_spec = std::unique_ptr<essentia::standard::Algorithm>(factory.create("Spectrum"));
        m_rms = std::unique_ptr<essentia::standard::Algorithm>(factory.create("RMS"));
        m_flux = std::unique_ptr<essentia::standard::Algorithm>(factory.create("Flux"));
        m_onset = std::unique_ptr<essentia::standard::Algorithm>(factory.create("OnsetDetection"));

        m_fc->configure("frameSize", 2048);
        m_fc->configure("hopSize", 1024);

        m_win->configure("type", "hann");

        m_onset->configure("method", "hfc");
    } catch (const std::exception& e) {
        qWarning() << "ScratchSenseiEssentiaProvider algorithm initialization failed:" << e.what();
    }
}

ScratchSenseiEssentiaProvider::~ScratchSenseiEssentiaProvider() {
    // Explicitly release unique_ptrs before global shutdown
    m_rhythm.reset();
    m_fc.reset();
    m_win.reset();
    m_spec.reset();
    m_rms.reset();
    m_flux.reset();
    m_onset.reset();

    EssentiaGlobalManager::shutdown();
}

QString ScratchSenseiEssentiaProvider::providerVersion() const {
    return "essentia-" + QString::fromStdString(essentia::version);
}

ProviderResult ScratchSenseiEssentiaProvider::analyzeMonoSamples(
        const std::vector<float>& monoSamples,
        double sampleRate,
        double fallbackBpm,
        const QVector<double>& fallbackBeatTimes) {
    ProviderResult result;
    if (monoSamples.empty() || sampleRate <= 0.0 || !m_fc) {
        return result;
    }

    result.durationSeconds = static_cast<double>(monoSamples.size()) / sampleRate;

    // Reset algorithms for reuse
    m_rhythm->reset();
    m_fc->reset();
    m_win->reset();
    m_spec->reset();
    m_rms->reset();
    m_flux->reset();
    m_onset->reset();

    if (fallbackBpm > 0.0 && !fallbackBeatTimes.isEmpty()) {
        // Skip RhythmExtractor2013 and use existing Mixxx beatgrid
        result.bpm = fallbackBpm;
        result.beatConfidence = 1.0;
        result.beatTimesSeconds = fallbackBeatTimes;
    } else {
        try {
            m_rhythm->input("signal").set(monoSamples);

            std::vector<float> ticks;
            float bpm = 0.0f;
            float confidence = 0.0f;
            std::vector<float> estimates;
            std::vector<float> bpmIntervals;

            m_rhythm->output("ticks").set(ticks);
            m_rhythm->output("bpm").set(bpm);
            m_rhythm->output("confidence").set(confidence);
            m_rhythm->output("estimates").set(estimates);
            m_rhythm->output("bpmIntervals").set(bpmIntervals);

            m_rhythm->compute();

            result.bpm = bpm;
            result.beatConfidence = confidence;
            for (float t : ticks) {
                result.beatTimesSeconds.append(t);
            }
        } catch (const std::exception& e) {
            qWarning() << "RhythmExtractor2013 failed:" << e.what();
            result.bpm = 0.0;
            result.beatConfidence = 0.0;
        }
    }

    double binSeconds = 8.0;
    int numBins = static_cast<int>(std::max(1.0, std::round(result.durationSeconds / binSeconds)));

    QVector<double> energyCurve(numBins, 0.0);
    QVector<double> fluxCurve(numBins, 0.0);
    QVector<double> onsetCurve(numBins, 0.0);
    QVector<double> beatStrengthCurve(numBins, 0.0);

    QVector<int> energyCounts(numBins, 0);
    QVector<int> fluxCounts(numBins, 0);
    QVector<int> onsetCounts(numBins, 0);

    try {
        std::vector<float> frame;
        std::vector<float> windowedFrame;
        std::vector<float> spectrum;
        std::vector<float> phase;
        float rmsVal = 0.0f;
        float fluxVal = 0.0f;
        float onsetVal = 0.0f;

        m_fc->input("signal").set(monoSamples);
        m_fc->output("frame").set(frame);

        m_win->input("frame").set(frame);
        m_win->output("frame").set(windowedFrame);

        m_spec->input("frame").set(windowedFrame);
        m_spec->output("spectrum").set(spectrum);

        m_rms->input("array").set(windowedFrame);
        m_rms->output("rms").set(rmsVal);

        m_flux->input("spectrum").set(spectrum);
        m_flux->output("flux").set(fluxVal);

        m_onset->input("spectrum").set(spectrum);
        m_onset->input("phase").set(phase);
        m_onset->output("onsetDetection").set(onsetVal);

        int frameIndex = 0;
        double maxEnergy = 0.0001;
        double maxFlux = 0.0001;
        double maxOnset = 0.0001;

        while (true) {
            m_fc->compute();
            if (frame.empty()) {
                break;
            }

            m_win->compute();
            m_spec->compute();
            m_rms->compute();
            m_flux->compute();
            m_onset->compute();

            double t = (frameIndex * 1024.0) / sampleRate;
            int binIdx = static_cast<int>(t / binSeconds);
            if (binIdx >= numBins) {
                binIdx = numBins - 1;
            }

            energyCurve[binIdx] += rmsVal;
            energyCounts[binIdx]++;
            maxEnergy = std::max(maxEnergy, static_cast<double>(rmsVal));

            fluxCurve[binIdx] += fluxVal;
            fluxCounts[binIdx]++;
            maxFlux = std::max(maxFlux, static_cast<double>(fluxVal));

            onsetCurve[binIdx] += onsetVal;
            onsetCounts[binIdx]++;
            maxOnset = std::max(maxOnset, static_cast<double>(onsetVal));

            frameIndex++;
        }

        for (int i = 0; i < numBins; ++i) {
            if (energyCounts[i] > 0) {
                energyCurve[i] /= energyCounts[i];
            }
            energyCurve[i] = std::clamp(energyCurve[i] / maxEnergy, 0.0, 1.0);

            if (fluxCounts[i] > 0) {
                fluxCurve[i] /= fluxCounts[i];
            }
            fluxCurve[i] = std::clamp(fluxCurve[i] / maxFlux, 0.0, 1.0);

            if (onsetCounts[i] > 0) {
                onsetCurve[i] /= onsetCounts[i];
            }
            onsetCurve[i] = std::clamp(onsetCurve[i] / maxOnset, 0.0, 1.0);

            double binStart = i * binSeconds;
            double binEnd = binStart + binSeconds;
            int beatsInBin = 0;
            for (double bt : result.beatTimesSeconds) {
                if (bt >= binStart && bt < binEnd) {
                    beatsInBin++;
                }
            }
            double expectedBeats = (result.bpm > 0.0) ? (result.bpm / 60.0) * binSeconds : 8.0;
            double density = static_cast<double>(beatsInBin) / std::max(1.0, expectedBeats);
            beatStrengthCurve[i] = std::clamp(density * result.beatConfidence, 0.0, 1.0);
        }
    } catch (const std::exception& e) {
        qWarning() << "Descriptor extraction failed:" << e.what();
    }

    DescriptorSeries energy;
    energy.name = "energy_norm";
    energy.hopSeconds = binSeconds;
    energy.values = energyCurve;
    result.timelines.append(energy);

    DescriptorSeries flux;
    flux.name = "spectral_flux_norm";
    flux.hopSeconds = binSeconds;
    flux.values = fluxCurve;
    result.timelines.append(flux);

    DescriptorSeries onsets;
    onsets.name = "onset_density_norm";
    onsets.hopSeconds = binSeconds;
    onsets.values = onsetCurve;
    result.timelines.append(onsets);

    DescriptorSeries beatStrength;
    beatStrength.name = "beat_strength_norm";
    beatStrength.hopSeconds = binSeconds;
    beatStrength.values = beatStrengthCurve;
    result.timelines.append(beatStrength);

    return result;
}

} // namespace mixxx::scratchsensei
