#include "analyzer/scratchsensei/scratchsenseianalyzer.h"
#include "analyzer/scratchsensei/scratchsenseianalysisdao.h"
#include "analyzer/scratchsensei/scratchsenseisuggestionbuilder.h"
#include "analyzer/scratchsensei/scratchsenseisummary.h"
#include "library/dao/analysisdao.h"
#include "track/beats.h"
#include "track/track.h"
#include <cmath>
#include <algorithm>
#include <QtDebug>

namespace mixxx::scratchsensei {

constexpr size_t kMaxSamplesMemoryLimit = 64000000;

AnalyzerScratchSensei::AnalyzerScratchSensei(
        UserSettingsPointer pConfig,
        const QSqlDatabase& dbConnection,
        std::unique_ptr<ScratchSenseiAnalysisProvider> provider)
        : m_pConfig(pConfig),
          m_dbConnection(dbConnection),
          m_pProvider(std::move(provider)),
          m_sampleRate(0),
          m_channelCount(0),
          m_failed(false) {
}

bool AnalyzerScratchSensei::initialize(const AnalyzerTrack& track,
        mixxx::audio::SampleRate sampleRate,
        mixxx::audio::ChannelCount channelCount,
        SINT frameLength) {
    m_failed = false;
    m_trackId = track.getTrack()->getId();
    m_sampleRate = sampleRate;
    m_channelCount = channelCount;
    m_monoBuffer.clear();

    if (!m_trackId.isValid()) {
        qWarning() << "AnalyzerScratchSensei: Invalid track ID.";
        m_failed = true;
        return false;
    }

    if (!m_pProvider) {
        qWarning() << "AnalyzerScratchSensei: No provider available.";
        m_failed = true;
        return false;
    }

    if (sampleRate <= 0 || channelCount <= 0) {
        qWarning() << "AnalyzerScratchSensei: Invalid sample rate or channels.";
        m_failed = true;
        return false;
    }

    return true;
}

bool AnalyzerScratchSensei::processSamples(const CSAMPLE* pIn, SINT count) {
    if (m_failed) {
        return false;
    }

    if (!pIn || count <= 0) {
        return true;
    }

    size_t newFrames = count / m_channelCount.value();
    size_t currentSize = m_monoBuffer.size();

    if (currentSize + newFrames > kMaxSamplesMemoryLimit) {
        qWarning() << "AnalyzerScratchSensei: Track exceeds memory/duration limit. Aborting analysis.";
        m_failed = true;
        return false;
    }

    m_monoBuffer.resize(currentSize + newFrames);
    float* out = m_monoBuffer.data() + currentSize;

    int channels = m_channelCount.value();
    for (size_t f = 0; f < newFrames; ++f) {
        float sum = 0.0f;
        for (int c = 0; c < channels; ++c) {
            sum += pIn[f * channels + c];
        }
        out[f] = sum / channels;
    }

    return true;
}

void AnalyzerScratchSensei::storeResults(TrackPointer pTrack) {
    if (m_failed || !pTrack || m_monoBuffer.empty()) {
        return;
    }

    double fallbackBpm = 0.0;
    QVector<double> fallbackBeatTimes;
    auto beats = pTrack->getBeats();
    if (beats) {
        fallbackBpm = pTrack->getBpm();
        if (fallbackBpm > 0.0) {
            double duration = pTrack->getDuration();
            double beatInterval = 60.0 / fallbackBpm;
            for (double t = 0.0; t < duration; t += beatInterval) {
                fallbackBeatTimes.append(t);
            }
        }
    }

    ProviderResult result = m_pProvider->analyzeMonoSamples(
            m_monoBuffer,
            m_sampleRate.value(),
            fallbackBpm,
            fallbackBeatTimes);

    mixxx::track::io::ScratchSenseiTrackAnalysis analysis =
            ScratchSenseiSuggestionBuilder::buildAnalysis(
                    m_trackId,
                    m_pProvider->providerVersion(),
                    result,
                    m_sampleRate.value());

    QByteArray payload = serializeTrackAnalysis(analysis);
    if (payload.isEmpty()) {
        qWarning() << "AnalyzerScratchSensei: Failed to serialize analysis summary.";
        return;
    }

    AnalysisDao dbDao(m_pConfig);
    dbDao.initialize(m_dbConnection);
    ScratchSenseiAnalysisDao scratchDao(&dbDao);

    QString version = analysisVersionForEssentiaVersion(m_pProvider->providerVersion());
    if (!scratchDao.saveReplacing(m_trackId, version, payload)) {
        qWarning() << "AnalyzerScratchSensei: Failed to save analysis to database.";
    }
}

void AnalyzerScratchSensei::cleanup() {
    m_monoBuffer.clear();
    m_monoBuffer.shrink_to_fit();
}

} // namespace mixxx::scratchsensei
