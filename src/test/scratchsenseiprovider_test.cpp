#include <gtest/gtest.h>
#include "analyzer/scratchsensei/scratchsenseifakeprovider.h"
#include "analyzer/scratchsensei/scratchsenseiessentiaprovider.h"

namespace mixxx::scratchsensei {

TEST(ScratchSenseiProviderTest, FakeProviderDeterministicOutput) {
    ScratchSenseiFakeProvider provider;
    EXPECT_EQ(provider.providerVersion(), "fake-v1");

    std::vector<float> silence(44100 * 10, 0.0f);
    double sampleRate = 44100.0;
    ProviderResult result = provider.analyzeMonoSamples(silence, sampleRate);

    EXPECT_DOUBLE_EQ(result.durationSeconds, 10.0);
    EXPECT_DOUBLE_EQ(result.bpm, 128.0);
    EXPECT_DOUBLE_EQ(result.beatConfidence, 0.9);

    ASSERT_EQ(result.timelines.size(), 4);
    EXPECT_EQ(result.timelines[0].name, "energy_norm");
    EXPECT_EQ(result.timelines[1].name, "spectral_flux_norm");
    EXPECT_EQ(result.timelines[2].name, "onset_density_norm");
    EXPECT_EQ(result.timelines[3].name, "beat_strength_norm");

    for (const auto& series : result.timelines) {
        EXPECT_EQ(series.values.size(), 5);
        EXPECT_DOUBLE_EQ(series.hopSeconds, 8.0);
    }
}

TEST(ScratchSenseiProviderTest, EssentiaProviderSilenceInput) {
    ScratchSenseiEssentiaProvider provider;
    EXPECT_TRUE(provider.providerVersion().startsWith("essentia-"));

    std::vector<float> silence(44100 * 20, 0.0f);
    double sampleRate = 44100.0;
    ProviderResult result = provider.analyzeMonoSamples(silence, sampleRate);

    EXPECT_NEAR(result.durationSeconds, 20.0, 0.01);
    
    ASSERT_EQ(result.timelines.size(), 4);
    for (const auto& series : result.timelines) {
        for (double val : series.values) {
            EXPECT_GE(val, 0.0);
            EXPECT_LE(val, 1.0);
        }
    }
}

TEST(ScratchSenseiProviderTest, EssentiaProviderStepEnergyInput) {
    ScratchSenseiEssentiaProvider provider;
    double sampleRate = 44100.0;
    
    std::vector<float> audio(static_cast<size_t>(sampleRate * 16), 0.0f);
    for (size_t i = static_cast<size_t>(sampleRate * 8); i < audio.size(); ++i) {
        audio[i] = static_cast<float>((rand() % 100) / 100.0 * 0.5);
    }

    ProviderResult result = provider.analyzeMonoSamples(audio, sampleRate);
    EXPECT_NEAR(result.durationSeconds, 16.0, 0.01);

    const DescriptorSeries* energySeries = nullptr;
    for (const auto& series : result.timelines) {
        if (series.name == "energy_norm") {
            energySeries = &series;
            break;
        }
    }
    ASSERT_NE(energySeries, nullptr);
    ASSERT_GE(energySeries->values.size(), 2);
    
    EXPECT_GT(energySeries->values[1], energySeries->values[0]);
}

TEST(ScratchSenseiProviderTest, EssentiaProviderShortAudioSafety) {
    ScratchSenseiEssentiaProvider provider;
    double sampleRate = 44100.0;
    
    std::vector<float> shortAudio(static_cast<size_t>(sampleRate * 0.1), 0.1f);
    
    ProviderResult result = provider.analyzeMonoSamples(shortAudio, sampleRate);
    EXPECT_NEAR(result.durationSeconds, 0.1, 0.01);
}

} // namespace mixxx::scratchsensei
