#pragma once

#include "library/dao/analysisdao.h"

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
