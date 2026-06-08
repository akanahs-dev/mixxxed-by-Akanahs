#include <gtest/gtest.h>
#include "test/librarytest.h"
#include "analyzer/scratchsensei/scratchsenseianalyzer.h"
#include "analyzer/scratchsensei/scratchsenseifakeprovider.h"
#include "analyzer/scratchsensei/scratchsenseianalysisdao.h"
#include "analyzer/scratchsensei/scratchsenseisummary.h"
#include "analyzer/analyzertrack.h"
#include "track/track.h"

namespace mixxx::scratchsensei {

class ScratchSenseiAnalyzerTest : public LibraryTest {
  protected:
    void SetUp() override {
        LibraryTest::SetUp();
        m_pTrack = Track::newDummy("/path/to/track.wav", TrackId(QVariant(42)));
    }

    TrackPointer m_pTrack;
};

TEST_F(ScratchSenseiAnalyzerTest, AnalyzeTrackWithFakeProvider) {
    auto fakeProvider = std::make_unique<ScratchSenseiFakeProvider>();
    AnalyzerScratchSensei analyzer(config(), dbConnection(), std::move(fakeProvider));

    AnalyzerTrack analyzerTrack(m_pTrack);
    ASSERT_TRUE(analyzer.initialize(analyzerTrack, mixxx::audio::SampleRate(44100), mixxx::audio::ChannelCount(2), 0));

    std::vector<CSAMPLE> samples(44100 * 2 * 10, 0.1f);
    ASSERT_TRUE(analyzer.processSamples(samples.data(), static_cast<SINT>(samples.size())));

    analyzer.storeResults(m_pTrack);

    AnalysisDao dbDao(config());
    dbDao.initialize(dbConnection());
    ScratchSenseiAnalysisDao scratchDao(&dbDao);
    AnalysisDao::AnalysisInfo info;
    ASSERT_TRUE(scratchDao.loadLatest(m_pTrack->getId(), &info));
    EXPECT_EQ(info.trackId, m_pTrack->getId());
    EXPECT_EQ(info.type, AnalysisDao::TYPE_SCRATCHSENSEI_TRACK_ANALYSIS);

    mixxx::track::io::ScratchSenseiTrackAnalysis parsed;
    ASSERT_TRUE(parseTrackAnalysis(info.data, &parsed));
    EXPECT_EQ(parsed.track_id(), m_pTrack->getId().toVariant().toLongLong());
    EXPECT_DOUBLE_EQ(parsed.duration_seconds(), 10.0);

    analyzer.cleanup();
}

TEST_F(ScratchSenseiAnalyzerTest, ExceedsMemoryLimit) {
    auto fakeProvider = std::make_unique<ScratchSenseiFakeProvider>();
    AnalyzerScratchSensei analyzer(config(), dbConnection(), std::move(fakeProvider));

    AnalyzerTrack analyzerTrack(m_pTrack);
    ASSERT_TRUE(analyzer.initialize(analyzerTrack, mixxx::audio::SampleRate(44100), mixxx::audio::ChannelCount(1), 0));

    std::vector<CSAMPLE> largeSamples(65000000, 0.0f);
    EXPECT_FALSE(analyzer.processSamples(largeSamples.data(), static_cast<SINT>(largeSamples.size())));

    analyzer.cleanup();
}

} // namespace mixxx::scratchsensei
