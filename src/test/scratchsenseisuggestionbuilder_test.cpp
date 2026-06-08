#include <gtest/gtest.h>

#include "analyzer/scratchsensei/scratchsenseisuggestionbuilder.h"
#include "analyzer/scratchsensei/scratchsenseifakeprovider.h"

namespace mixxx::scratchsensei {

TEST(ScratchSenseiSuggestionBuilderTest, BuildsSuggestionsFromFakeProvider) {
    TrackId trackId(QVariant(101));
    double sampleRate = 44100.0;
    std::vector<float> samples(44100 * 40, 0.0f); // 40 seconds of silent samples

    ScratchSenseiFakeProvider provider;
    ProviderResult result = provider.analyzeMonoSamples(samples, sampleRate);

    mixxx::track::io::ScratchSenseiTrackAnalysis analysis =
            ScratchSenseiSuggestionBuilder::buildAnalysis(
                    trackId,
                    provider.providerVersion(),
                    result,
                    sampleRate);

    // Verify properties
    EXPECT_EQ(analysis.track_id(), 101);
    EXPECT_DOUBLE_EQ(analysis.duration_seconds(), 40.0);

    // Verify timelines exist
    EXPECT_EQ(analysis.descriptor_timeline_size(), 4);

    // Verify capabilities
    EXPECT_TRUE(analysis.capabilities().has_energy_curve());
    EXPECT_TRUE(analysis.capabilities().has_phrase_candidates());
    EXPECT_TRUE(analysis.capabilities().has_beatgrid_snapshot());
    EXPECT_TRUE(analysis.capabilities().has_reliable_tempo());

    // Verify suggestion is created near the energy lift
    // In our fake provider:
    // energy_norm: [0.1, 0.2, 0.8, 0.9, 0.3] -> rise at index 2 (from 0.2 to 0.8 = +0.6)
    // t = 2 * 8.0 = 16.0 seconds.
    // energy_lift should exist around t = 16.0
    bool foundEnergyLift = false;
    bool foundEnergyDip = false;
    bool foundSectionChange = false;
    for (int i = 0; i < analysis.suggestion_size(); ++i) {
        const auto& sug = analysis.suggestion(i);
        if (sug.type() == "energy_lift") {
            foundEnergyLift = true;
            EXPECT_NEAR(sug.time_seconds(), 16.0, 0.1);
            EXPECT_EQ(sug.confidence_bucket(), "Maybe"); // confidence = 0.6
            EXPECT_TRUE(sug.has_beat_index());
            EXPECT_TRUE(sug.has_bar_index());
            EXPECT_TRUE(sug.has_phrase_index());
            // stable ID check
            std::string expectedId = "101_energy_lift_" + std::to_string(static_cast<int64_t>(std::round(16.0 * sampleRate)));
            EXPECT_EQ(sug.id(), expectedId);
        } else if (sug.type() == "energy_dip") {
            foundEnergyDip = true;
            EXPECT_NEAR(sug.time_seconds(), 32.0, 0.1); // index 4: t = 32.0 (from 0.9 to 0.3 = -0.6)
        } else if (sug.type() == "section_change") {
            foundSectionChange = true;
        }
    }
    EXPECT_TRUE(foundEnergyLift);
    EXPECT_TRUE(foundEnergyDip);
    EXPECT_TRUE(foundSectionChange);
}

TEST(ScratchSenseiSuggestionBuilderTest, HandlesMissingBeatgrid) {
    TrackId trackId(QVariant(202));
    double sampleRate = 44100.0;
    std::vector<float> samples(44100 * 20, 0.0f);

    ScratchSenseiFakeProvider provider;
    ProviderResult result = provider.analyzeMonoSamples(samples, sampleRate);
    // Erase beatgrid info
    result.bpm = 0.0;
    result.beatTimesSeconds.clear();

    mixxx::track::io::ScratchSenseiTrackAnalysis analysis =
            ScratchSenseiSuggestionBuilder::buildAnalysis(
                    trackId,
                    provider.providerVersion(),
                    result,
                    sampleRate);

    EXPECT_FALSE(analysis.capabilities().has_beatgrid_snapshot());
    EXPECT_FALSE(analysis.capabilities().has_reliable_tempo());

    // Suggestions should still build but not have beat/bar/phrase indices
    EXPECT_GT(analysis.suggestion_size(), 0);
    for (int i = 0; i < analysis.suggestion_size(); ++i) {
        const auto& sug = analysis.suggestion(i);
        EXPECT_FALSE(sug.has_beat_index());
        EXPECT_FALSE(sug.has_bar_index());
        EXPECT_FALSE(sug.has_phrase_index());
    }
}

} // namespace mixxx::scratchsensei
