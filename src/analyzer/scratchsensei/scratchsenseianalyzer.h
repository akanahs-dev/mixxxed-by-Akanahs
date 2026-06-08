#pragma once

#include "analyzer/analyzer.h"
#include "analyzer/scratchsensei/scratchsenseianalysisprovider.h"
#include "preferences/usersettings.h"
#include "track/trackid.h"
#include <vector>
#include <memory>

namespace mixxx::scratchsensei {

class AnalyzerScratchSensei : public ::Analyzer {
  public:
    AnalyzerScratchSensei(
            UserSettingsPointer pConfig,
            const QSqlDatabase& dbConnection,
            std::unique_ptr<ScratchSenseiAnalysisProvider> provider);
    ~AnalyzerScratchSensei() override = default;

    bool initialize(const AnalyzerTrack& track,
            mixxx::audio::SampleRate sampleRate,
            mixxx::audio::ChannelCount channelCount,
            SINT frameLength) override;

    bool processSamples(const CSAMPLE* pIn, SINT count) override;

    void storeResults(TrackPointer pTrack) override;

    void cleanup() override;

  private:
    const UserSettingsPointer m_pConfig;
    QSqlDatabase m_dbConnection;
    const std::unique_ptr<ScratchSenseiAnalysisProvider> m_pProvider;

    TrackId m_trackId;
    mixxx::audio::SampleRate m_sampleRate;
    mixxx::audio::ChannelCount m_channelCount;

    std::vector<float> m_monoBuffer;
    bool m_failed;
};

} // namespace mixxx::scratchsensei
