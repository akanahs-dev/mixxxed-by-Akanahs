#pragma once

#include <vector>

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
            const std::vector<float>& monoSamples,
            double sampleRate,
            double fallbackBpm = 0.0,
            const QVector<double>& fallbackBeatTimes = {}) = 0;
};

} // namespace mixxx::scratchsensei
