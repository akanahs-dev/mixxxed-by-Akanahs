#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <QScopedPointer>
#include <QList>
#include <QString>

#include "library/scratchsensei/scratchsenseifeature.h"
#include "mixer/playermanager.h"
#include "mixer/basetrackplayer.h"
#include "track/track.h"

using ::testing::_;
using ::testing::Return;

namespace {

class FakeDeck : public BaseTrackPlayer {
  public:
    FakeDeck(const QString& group)
            : BaseTrackPlayer(nullptr, group) {}

    TrackPointer getLoadedTrack() const override {
        return loadedTrack;
    }

    void setupEqControls() override {}

    void slotLoadTrack(TrackPointer pTrack,
#ifdef __STEM__
            mixxx::StemChannelSelection,
#endif
            bool bPlay) override {
        loadedTrack = pTrack;
        Q_UNUSED(bPlay);
    }

    void slotEjectTrack(double val) override {
        if (val > 0) {
            loadedTrack = nullptr;
        }
    }

    MOCK_METHOD1(slotCloneFromGroup, void(const QString& group));
    MOCK_METHOD0(slotCloneDeck, void());

    TrackPointer loadedTrack;
};

class MockPlayerManager : public PlayerManagerInterface {
  public:
    MockPlayerManager() = default;
    virtual ~MockPlayerManager() = default;

    MOCK_CONST_METHOD1(getPlayer, BaseTrackPlayer*(const QString&));
    MOCK_CONST_METHOD1(getPlayer, BaseTrackPlayer*(const ChannelHandle&));
    MOCK_CONST_METHOD1(getDeckBase, BaseTrackPlayer*(int));
    MOCK_CONST_METHOD1(getPreviewDeck, PreviewDeck*(int));
    MOCK_CONST_METHOD1(getSampler, Sampler*(int));

    int numberOfDecks() const override {
        return m_numDecks;
    }

    int numberOfSamplers() const override {
        return 0;
    }

    int numberOfPreviewDecks() const override {
        return 0;
    }

    int m_numDecks = 0;
};

class ScratchSenseiDeckResolverTest : public ::testing::Test {
  protected:
    ScratchSenseiDeckResolverTest()
            : deck1("[Channel1]"),
              deck2("[Channel2]") {}

    void SetUp() override {
        pPlayerManager.reset(new MockPlayerManager());
    }

    FakeDeck deck1;
    FakeDeck deck2;
    QScopedPointer<MockPlayerManager> pPlayerManager;
};

TEST_F(ScratchSenseiDeckResolverTest, ResolveNoLoadedDecks) {
    pPlayerManager->m_numDecks = 2;
    EXPECT_CALL(*pPlayerManager, getDeckBase(0)).WillRepeatedly(Return(&deck1));
    EXPECT_CALL(*pPlayerManager, getDeckBase(1)).WillRepeatedly(Return(&deck2));

    deck1.loadedTrack = nullptr;
    deck2.loadedTrack = nullptr;

    ScratchSenseiFeature feature(nullptr, nullptr, pPlayerManager.data());
    QList<ScratchSenseiDeckContext> resolved = feature.resolveLoadedDecks();

    EXPECT_TRUE(resolved.isEmpty());
}

TEST_F(ScratchSenseiDeckResolverTest, ResolveOneLoadedDeck) {
    pPlayerManager->m_numDecks = 2;
    EXPECT_CALL(*pPlayerManager, getDeckBase(0)).WillRepeatedly(Return(&deck1));
    EXPECT_CALL(*pPlayerManager, getDeckBase(1)).WillRepeatedly(Return(&deck2));

    TrackPointer track1 = Track::newDummy("/path/to/test1.mp3", TrackId(QVariant(1)));
    deck1.loadedTrack = track1;
    deck2.loadedTrack = nullptr;

    ScratchSenseiFeature feature(nullptr, nullptr, pPlayerManager.data());
    QList<ScratchSenseiDeckContext> resolved = feature.resolveLoadedDecks();

    ASSERT_EQ(resolved.size(), 1);
    EXPECT_EQ(resolved[0].deckIndex, 0);
    EXPECT_EQ(resolved[0].group, "[Channel1]");
    EXPECT_EQ(resolved[0].trackId, TrackId(QVariant(1)));
    EXPECT_EQ(resolved[0].pTrack, track1);
}

TEST_F(ScratchSenseiDeckResolverTest, ResolveMultipleLoadedDecks) {
    pPlayerManager->m_numDecks = 2;
    EXPECT_CALL(*pPlayerManager, getDeckBase(0)).WillRepeatedly(Return(&deck1));
    EXPECT_CALL(*pPlayerManager, getDeckBase(1)).WillRepeatedly(Return(&deck2));

    TrackPointer track1 = Track::newDummy("/path/to/test1.mp3", TrackId(QVariant(1)));
    TrackPointer track2 = Track::newDummy("/path/to/test2.mp3", TrackId(QVariant(2)));
    deck1.loadedTrack = track1;
    deck2.loadedTrack = track2;

    ScratchSenseiFeature feature(nullptr, nullptr, pPlayerManager.data());
    QList<ScratchSenseiDeckContext> resolved = feature.resolveLoadedDecks();

    ASSERT_EQ(resolved.size(), 2);
    EXPECT_EQ(resolved[0].deckIndex, 0);
    EXPECT_EQ(resolved[0].group, "[Channel1]");
    EXPECT_EQ(resolved[0].trackId, TrackId(QVariant(1)));

    EXPECT_EQ(resolved[1].deckIndex, 1);
    EXPECT_EQ(resolved[1].group, "[Channel2]");
    EXPECT_EQ(resolved[1].trackId, TrackId(QVariant(2)));
}

} // namespace
