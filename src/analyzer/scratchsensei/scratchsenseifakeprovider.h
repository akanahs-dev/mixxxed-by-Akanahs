#pragma once

#include "analyzer/scratchsensei/scratchsenseianalysisprovider.h"

namespace mixxx::scratchsensei {

class ScratchSenseiFakeProvider : public ScratchSenseiAnalysisProvider {
  public:
    QString providerVersion() const override {
        return "fake-v1";
    }

    ProviderResult analyzeMonoSamples(
            const std::vector<float>& monoSamples,
            double sampleRate,
            double fallbackBpm = 0.0,
            const QVector<double>& fallbackBeatTimes = {}) override {
        ProviderResult result;
        // If sampleRate is 0 or less, avoid division by zero.
        double rate = (sampleRate > 0.0) ? sampleRate : 44100.0;
        result.durationSeconds = static_cast<double>(monoSamples.size()) / rate;
        result.bpm = 128.0;
        result.beatConfidence = 0.9;

        double beatInterval = 60.0 / 128.0;
        for (double t = 0.0; t < result.durationSeconds; t += beatInterval) {
            result.beatTimesSeconds.append(t);
        }

        DescriptorSeries energy;
        energy.name = "energy_norm";
        energy.hopSeconds = 8.0;
        energy.values = QVector<double>{0.1, 0.2, 0.8, 0.9, 0.3};

        DescriptorSeries flux;
        flux.name = "spectral_flux_norm";
        flux.hopSeconds = 8.0;
        flux.values = QVector<double>{0.0, 0.1, 0.9, 0.4, 0.1};

        DescriptorSeries onsets;
        onsets.name = "onset_density_norm";
        onsets.hopSeconds = 8.0;
        onsets.values = QVector<double>{0.1, 0.2, 0.7, 0.8, 0.2};

        DescriptorSeries beatStrength;
        beatStrength.name = "beat_strength_norm";
        beatStrength.hopSeconds = 8.0;
        beatStrength.values = QVector<double>{0.8, 0.8, 0.9, 0.9, 0.6};

        result.timelines.append(energy);
        result.timelines.append(flux);
        result.timelines.append(onsets);
        result.timelines.append(beatStrength);

        return result;
    }
};

} // namespace mixxx::scratchsensei
