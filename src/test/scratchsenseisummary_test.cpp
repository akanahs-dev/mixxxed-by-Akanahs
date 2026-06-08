#include <gtest/gtest.h>

#include "analyzer/scratchsensei/scratchsenseisummary.h"

namespace {

TEST(ScratchSenseiSummaryTest, RoundTripsMinimalPayload) {
    mixxx::track::io::ScratchSenseiTrackAnalysis analysis;
    analysis.set_schema_version(mixxx::scratchsensei::kSchemaVersion);
    analysis.set_analyzer_version("essentia-test");
    analysis.set_track_id(42);
    analysis.set_duration_seconds(180.0);

    const QByteArray bytes = mixxx::scratchsensei::serializeTrackAnalysis(analysis);
    ASSERT_FALSE(bytes.isEmpty());

    mixxx::track::io::ScratchSenseiTrackAnalysis parsed;
    ASSERT_TRUE(mixxx::scratchsensei::parseTrackAnalysis(bytes, &parsed));
    EXPECT_EQ(parsed.schema_version(), std::string(mixxx::scratchsensei::kSchemaVersion));
    EXPECT_EQ(parsed.analyzer_version(), "essentia-test");
    EXPECT_EQ(parsed.track_id(), 42);
    EXPECT_DOUBLE_EQ(parsed.duration_seconds(), 180.0);
}

TEST(ScratchSenseiSummaryTest, RejectsInvalidPayload) {
    mixxx::track::io::ScratchSenseiTrackAnalysis parsed;
    EXPECT_FALSE(mixxx::scratchsensei::parseTrackAnalysis(QByteArray("bad"), &parsed));
}

} // namespace
