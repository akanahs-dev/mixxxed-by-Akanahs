#include <gtest/gtest.h>
#include "test/librarytest.h"
#include "analyzer/scratchsensei/scratchsenseianalysisdao.h"
#include "analyzer/scratchsensei/scratchsenseisummary.h"

namespace mixxx::scratchsensei {

class ScratchSenseiAnalysisDaoTest : public LibraryTest {
  protected:
    void SetUp() override {
        LibraryTest::SetUp();
        m_pAnalysisDao = std::make_unique<AnalysisDao>(config());
        m_pAnalysisDao->initialize(dbConnection());
        m_pScratchSenseiAnalysisDao = std::make_unique<ScratchSenseiAnalysisDao>(m_pAnalysisDao.get());
    }

    void TearDown() override {
        m_pScratchSenseiAnalysisDao.reset();
        m_pAnalysisDao.reset();
        LibraryTest::TearDown();
    }

    std::unique_ptr<AnalysisDao> m_pAnalysisDao;
    std::unique_ptr<ScratchSenseiAnalysisDao> m_pScratchSenseiAnalysisDao;
};

TEST_F(ScratchSenseiAnalysisDaoTest, SaveAndLoadAndReplace) {
    TrackId trackId(QVariant(42));
    QString version1 = "scratch-sensei-v1:essentia-1";
    QByteArray payload1 = "payload1_data";

    // Saving first analysis
    ASSERT_TRUE(m_pScratchSenseiAnalysisDao->saveReplacing(trackId, version1, payload1));

    // Loading analysis
    AnalysisDao::AnalysisInfo loadedInfo;
    ASSERT_TRUE(m_pScratchSenseiAnalysisDao->loadLatest(trackId, &loadedInfo));
    EXPECT_EQ(loadedInfo.trackId, trackId);
    EXPECT_EQ(loadedInfo.type, AnalysisDao::TYPE_SCRATCHSENSEI_TRACK_ANALYSIS);
    EXPECT_EQ(loadedInfo.version, version1);
    EXPECT_EQ(loadedInfo.data, payload1);

    // Saving second analysis to replace the first
    QString version2 = "scratch-sensei-v1:essentia-2";
    QByteArray payload2 = "payload2_data_longer";
    ASSERT_TRUE(m_pScratchSenseiAnalysisDao->saveReplacing(trackId, version2, payload2));

    // Verify replacement
    AnalysisDao::AnalysisInfo loadedInfo2;
    ASSERT_TRUE(m_pScratchSenseiAnalysisDao->loadLatest(trackId, &loadedInfo2));
    EXPECT_EQ(loadedInfo2.version, version2);
    EXPECT_EQ(loadedInfo2.data, payload2);

    // Verify old analysis is not in database anymore
    QList<AnalysisDao::AnalysisInfo> list = m_pAnalysisDao->getAnalysesForTrackByType(
            trackId, AnalysisDao::TYPE_SCRATCHSENSEI_TRACK_ANALYSIS);
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list[0].analysisId, loadedInfo2.analysisId);
}

} // namespace mixxx::scratchsensei
