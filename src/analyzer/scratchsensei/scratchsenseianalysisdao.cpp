#include "analyzer/scratchsensei/scratchsenseianalysisdao.h"
#include "analyzer/scratchsensei/scratchsenseisummary.h"

namespace mixxx::scratchsensei {

ScratchSenseiAnalysisDao::ScratchSenseiAnalysisDao(AnalysisDao* analysisDao)
        : m_pAnalysisDao(analysisDao) {
}

bool ScratchSenseiAnalysisDao::saveReplacing(
        TrackId trackId,
        const QString& version,
        const QByteArray& payload) {
    if (!m_pAnalysisDao) {
        return false;
    }
    m_pAnalysisDao->deleteAnalysesForTrackByType(trackId, AnalysisDao::TYPE_SCRATCHSENSEI_TRACK_ANALYSIS);

    AnalysisDao::AnalysisInfo info;
    info.trackId = trackId;
    info.type = AnalysisDao::TYPE_SCRATCHSENSEI_TRACK_ANALYSIS;
    info.description = kAnalysisDescription;
    info.version = version;
    info.data = payload;

    return m_pAnalysisDao->saveAnalysis(&info);
}

bool ScratchSenseiAnalysisDao::loadLatest(
        TrackId trackId,
        AnalysisDao::AnalysisInfo* analysis) const {
    if (!m_pAnalysisDao || !analysis) {
        return false;
    }
    QList<AnalysisDao::AnalysisInfo> analyses =
            m_pAnalysisDao->getAnalysesForTrackByType(trackId, AnalysisDao::TYPE_SCRATCHSENSEI_TRACK_ANALYSIS);
    if (analyses.isEmpty()) {
        return false;
    }
    int maxIdIndex = 0;
    for (int i = 1; i < analyses.size(); ++i) {
        if (analyses[i].analysisId > analyses[maxIdIndex].analysisId) {
            maxIdIndex = i;
        }
    }
    *analysis = analyses[maxIdIndex];
    return true;
}

} // namespace mixxx::scratchsensei
