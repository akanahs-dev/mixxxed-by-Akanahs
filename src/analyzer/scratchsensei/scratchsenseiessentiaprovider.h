#pragma once

#include "analyzer/scratchsensei/scratchsenseianalysisprovider.h"
#include <memory>
#include <vector>

namespace essentia::standard {
class Algorithm;
}

namespace mixxx::scratchsensei {

class ScratchSenseiEssentiaProvider : public ScratchSenseiAnalysisProvider {
  public:
    ScratchSenseiEssentiaProvider();
    ~ScratchSenseiEssentiaProvider() override;

    QString providerVersion() const override;
    ProviderResult analyzeMonoSamples(
            const std::vector<float>& monoSamples,
            double sampleRate,
            double fallbackBpm = 0.0,
            const QVector<double>& fallbackBeatTimes = {}) override;

  private:
    std::unique_ptr<essentia::standard::Algorithm> m_rhythm;
    std::unique_ptr<essentia::standard::Algorithm> m_fc;
    std::unique_ptr<essentia::standard::Algorithm> m_win;
    std::unique_ptr<essentia::standard::Algorithm> m_spec;
    std::unique_ptr<essentia::standard::Algorithm> m_rms;
    std::unique_ptr<essentia::standard::Algorithm> m_flux;
    std::unique_ptr<essentia::standard::Algorithm> m_onset;
};

} // namespace mixxx::scratchsensei
