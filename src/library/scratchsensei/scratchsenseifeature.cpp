#include "library/scratchsensei/scratchsenseifeature.h"

#include "controllers/keyboard/keyboardeventfilter.h"
#include "library/library.h"
#include "mixer/playermanager.h"
#include "mixer/basetrackplayer.h"
#include "track/track.h"
#include "widget/wlibrary.h"
#include "library/scratchsensei/dlgscratchsensei.h"
#include "library/treeitem.h"
#include "library/trackcollectionmanager.h"
#include "library/trackcollection.h"
#include "library/dao/analysisdao.h"
#include "analyzer/scratchsensei/scratchsenseianalysisdao.h"
#include "analyzer/scratchsensei/scratchsenseisummary.h"
#include "control/controlobject.h"
#include "util/defs.h"

#include <QInputDialog>
#include <QMessageBox>
#include <algorithm>

#include "moc_scratchsenseifeature.cpp"

namespace {

const QString kViewName = QStringLiteral("ScratchSensei");

} // anonymous namespace

ScratchSenseiFeature::ScratchSenseiFeature(Library* pLibrary,
                                           UserSettingsPointer pConfig,
                                           PlayerManagerInterface* pPlayerManager)
        : LibraryFeature(pLibrary, pConfig, QStringLiteral("prepare")),
          m_pPlayerManager(pPlayerManager),
          m_pSidebarModel(make_parented<TreeItemModel>(this)),
          m_pScratchSenseiView(nullptr),
          m_pTrackAnalysisScheduler(TrackAnalysisScheduler::NullPointer()),
          m_currentTrackId(),
          m_currentDeckIndex(-1) {
    m_pSidebarModel->setRootItem(TreeItem::newRoot(this));
}

QVariant ScratchSenseiFeature::title() {
    return QVariant(tr("Scratch Sensei"));
}

TreeItemModel* ScratchSenseiFeature::sidebarModel() const {
    return m_pSidebarModel.get();
}

void ScratchSenseiFeature::bindLibraryWidget(WLibrary* libraryWidget,
                                             KeyboardEventFilter* keyboard) {
    m_pScratchSenseiView = new DlgScratchSensei(libraryWidget,
                                                m_pConfig,
                                                m_pLibrary,
                                                this,
                                                keyboard);
    m_pScratchSenseiView->installEventFilter(keyboard);
    libraryWidget->registerView(kViewName, m_pScratchSenseiView);

    connect(m_pScratchSenseiView, &DlgScratchSensei::analyzeRequested,
            this, &ScratchSenseiFeature::onAnalyzeRequested);
    connect(m_pScratchSenseiView, &DlgScratchSensei::cancelRequested,
            this, &ScratchSenseiFeature::onCancelRequested);
    connect(m_pScratchSenseiView, &DlgScratchSensei::deckSelected,
            this, &ScratchSenseiFeature::onDeckSelected);
    connect(m_pScratchSenseiView, &DlgScratchSensei::jumpRequested,
            this, &ScratchSenseiFeature::onJumpRequested);
    connect(m_pScratchSenseiView, &DlgScratchSensei::createCueRequested,
            this, &ScratchSenseiFeature::onCreateCueRequested);
    connect(m_pScratchSenseiView, &DlgScratchSensei::promoteHotcueRequested,
            this, &ScratchSenseiFeature::onPromoteHotcueRequested);
}

void ScratchSenseiFeature::activate() {
    emit switchToView(kViewName);
    emit enableCoverArtDisplay(false);
}

QList<ScratchSenseiDeckContext> ScratchSenseiFeature::resolveLoadedDecks() const {
    QList<ScratchSenseiDeckContext> loadedDecks;
    if (!m_pPlayerManager) {
        return loadedDecks;
    }
    int numDecks = m_pPlayerManager->numberOfDecks();
    for (int i = 0; i < numDecks; ++i) {
        BaseTrackPlayer* pPlayer = m_pPlayerManager->getDeckBase(i);
        if (pPlayer) {
            TrackPointer pTrack = pPlayer->getLoadedTrack();
            if (pTrack && pTrack->getId().isValid()) {
                ScratchSenseiDeckContext ctx;
                ctx.deckIndex = i;
                ctx.group = pPlayer->getGroup();
                ctx.trackId = pTrack->getId();
                ctx.trackTitle = pTrack->getInfo();
                ctx.pTrack = pTrack;
                loadedDecks.append(ctx);
            }
        }
    }
    return loadedDecks;
}

void ScratchSenseiFeature::updateDeckContexts() {
    auto loadedDecks = resolveLoadedDecks();
    QList<int> deckIndices;
    QStringList deckDescriptions;
    for (const auto& ctx : loadedDecks) {
        deckIndices.append(ctx.deckIndex);
        deckDescriptions.append(QString("Deck %1: %2").arg(ctx.deckIndex + 1).arg(ctx.trackTitle));
    }
    
    if (m_pScratchSenseiView) {
        m_pScratchSenseiView->setDeckContexts(deckIndices, deckDescriptions);
        if (loadedDecks.isEmpty()) {
            m_pScratchSenseiView->setAnalysisState(DlgScratchSensei::State::NoDeck);
            m_pScratchSenseiView->setTrackInfo(TrackId(), QString());
            m_currentTrackId = TrackId();
            m_currentPayload.Clear();
        } else if (loadedDecks.size() == 1) {
            onDeckSelected(loadedDecks.first().deckIndex);
        } else {
            m_pScratchSenseiView->setAnalysisState(DlgScratchSensei::State::AmbiguousDeck);
            m_pScratchSenseiView->setTrackInfo(TrackId(), QString());
            m_currentTrackId = TrackId();
            m_currentPayload.Clear();
        }
    }
}

void ScratchSenseiFeature::onDeckSelected(int deckIndex) {
    m_currentDeckIndex = deckIndex;
    auto loadedDecks = resolveLoadedDecks();
    for (const auto& ctx : loadedDecks) {
        if (ctx.deckIndex == deckIndex) {
            m_currentTrackId = ctx.trackId;
            if (m_pScratchSenseiView) {
                m_pScratchSenseiView->setTrackInfo(m_currentTrackId, ctx.trackTitle);
            }
            loadAnalysisResult(m_currentTrackId);
            return;
        }
    }
    
    // Not found
    m_currentTrackId = TrackId();
    m_currentPayload.Clear();
    if (m_pScratchSenseiView) {
        m_pScratchSenseiView->setAnalysisState(DlgScratchSensei::State::NoDeck);
        m_pScratchSenseiView->setTrackInfo(TrackId(), QString());
    }
}

void ScratchSenseiFeature::onAnalyzeRequested(TrackId trackId) {
    if (m_pTrackAnalysisScheduler || !trackId.isValid()) {
        return;
    }
    
    m_pTrackAnalysisScheduler = m_pLibrary->createTrackAnalysisScheduler(
            1, static_cast<AnalyzerModeFlags>(AnalyzerModeFlags::ScratchSenseiOnly | AnalyzerModeFlags::LowPriority));
            
    connect(m_pTrackAnalysisScheduler.get(),
            &TrackAnalysisScheduler::progress,
            this,
            &ScratchSenseiFeature::onTrackAnalysisSchedulerProgress);
    connect(m_pTrackAnalysisScheduler.get(),
            &TrackAnalysisScheduler::finished,
            this,
            &ScratchSenseiFeature::onTrackAnalysisSchedulerFinished);
            
    AnalyzerScheduledTrack track(trackId);
    m_pTrackAnalysisScheduler->scheduleTrack(track);
    m_pTrackAnalysisScheduler->resume();
    
    if (m_pScratchSenseiView) {
        m_pScratchSenseiView->setAnalysisState(DlgScratchSensei::State::Analyzing);
    }
}

void ScratchSenseiFeature::onCancelRequested() {
    if (m_pTrackAnalysisScheduler) {
        m_pTrackAnalysisScheduler->stop();
        m_pTrackAnalysisScheduler.reset();
    }
    if (m_pScratchSenseiView) {
        m_pScratchSenseiView->setAnalysisState(DlgScratchSensei::State::Cancelled);
    }
}

void ScratchSenseiFeature::onTrackAnalysisSchedulerProgress(AnalyzerProgress currentTrackProgress, int currentTrackNumber, int totalTracksCount) {
    Q_UNUSED(currentTrackNumber);
    Q_UNUSED(totalTracksCount);
    if (m_pScratchSenseiView) {
        m_pScratchSenseiView->setAnalysisProgress(currentTrackProgress);
    }
}

void ScratchSenseiFeature::onTrackAnalysisSchedulerFinished() {
    if (m_pTrackAnalysisScheduler) {
        m_pTrackAnalysisScheduler.reset();
    }
    loadAnalysisResult(m_currentTrackId);
}

void ScratchSenseiFeature::onJumpRequested(double timeSeconds) {
    if (!m_currentTrackId.isValid() || m_currentDeckIndex < 0) {
        return;
    }
    TrackPointer pTrack = m_pLibrary->trackCollectionManager()->getTrackById(m_currentTrackId);
    if (!pTrack) {
        return;
    }
    double duration = pTrack->getDuration();
    if (duration <= 0.0) {
        return;
    }
    double fraction = timeSeconds / duration;
    fraction = std::clamp(fraction, 0.0, 1.0);

    QString group = PlayerManager::groupForDeck(m_currentDeckIndex);
    ControlObject::set(ConfigKey(group, QStringLiteral("playposition")), fraction);
}

void ScratchSenseiFeature::onCreateCueRequested(QString suggestionId) {
    if (!m_currentTrackId.isValid()) {
        return;
    }
    TrackPointer pTrack = m_pLibrary->trackCollectionManager()->getTrackById(m_currentTrackId);
    if (!pTrack) {
        return;
    }

    const mixxx::track::io::Suggestion* pFound = nullptr;
    for (int i = 0; i < m_currentPayload.suggestion_size(); ++i) {
        if (QString::fromStdString(m_currentPayload.suggestion(i).id()) == suggestionId) {
            pFound = &m_currentPayload.suggestion(i);
            break;
        }
    }
    if (!pFound) {
        return;
    }

    mixxx::audio::FramePos startPos(pFound->time_seconds() * pTrack->getSampleRate());
    pTrack->createAndAddCue(
        mixxx::CueType::MainCue,
        Cue::kNoHotCue,
        startPos,
        mixxx::audio::kInvalidFramePos
    );
}

void ScratchSenseiFeature::onPromoteHotcueRequested(QString suggestionId) {
    if (!m_currentTrackId.isValid()) {
        return;
    }
    TrackPointer pTrack = m_pLibrary->trackCollectionManager()->getTrackById(m_currentTrackId);
    if (!pTrack) {
        return;
    }

    const mixxx::track::io::Suggestion* pFound = nullptr;
    for (int i = 0; i < m_currentPayload.suggestion_size(); ++i) {
        if (QString::fromStdString(m_currentPayload.suggestion(i).id()) == suggestionId) {
            pFound = &m_currentPayload.suggestion(i);
            break;
        }
    }
    if (!pFound) {
        return;
    }

    QStringList availableSlots;
    for (int i = 0; i < kMaxNumberOfHotcues; ++i) {
        if (!pTrack->findHotcueByIndex(i)) {
            availableSlots.append(QString::number(i + 1));
        }
    }

    if (availableSlots.isEmpty()) {
        QMessageBox::warning(
            m_pScratchSenseiView,
            tr("No Available Hotcues"),
            tr("All hotcue slots are currently in use."));
        return;
    }

    bool ok = false;
    QString chosen = QInputDialog::getItem(
        m_pScratchSenseiView,
        tr("Promote to Hotcue"),
        tr("Select an available hotcue slot:"),
        availableSlots,
        0,
        false,
        &ok
    );

    if (!ok || chosen.isEmpty()) {
        return;
    }

    int hotcueIndex = chosen.toInt() - 1;

    mixxx::audio::FramePos startPos(pFound->time_seconds() * pTrack->getSampleRate());
    pTrack->createAndAddCue(
        mixxx::CueType::HotCue,
        hotcueIndex,
        startPos,
        mixxx::audio::kInvalidFramePos
    );
}

void ScratchSenseiFeature::loadAnalysisResult(TrackId trackId) {
    if (!trackId.isValid()) {
        m_currentPayload.Clear();
        if (m_pScratchSenseiView) {
            m_pScratchSenseiView->setAnalysisState(DlgScratchSensei::State::NoDeck);
        }
        return;
    }

    auto& analysisDao = m_pLibrary->trackCollectionManager()->internalCollection()->getAnalysisDAO();
    mixxx::scratchsensei::ScratchSenseiAnalysisDao ssDao(&analysisDao);

    AnalysisDao::AnalysisInfo analysisInfo;
    if (!ssDao.loadLatest(trackId, &analysisInfo)) {
        m_currentPayload.Clear();
        if (m_pScratchSenseiView) {
            m_pScratchSenseiView->setAnalysisState(DlgScratchSensei::State::NotAnalyzed);
        }
        return;
    }

    mixxx::track::io::ScratchSenseiTrackAnalysis payload;
    if (!mixxx::scratchsensei::parseTrackAnalysis(analysisInfo.data, &payload)) {
        m_currentPayload.Clear();
        if (m_pScratchSenseiView) {
            m_pScratchSenseiView->setAnalysisState(DlgScratchSensei::State::Failed);
        }
        return;
    }

    m_currentPayload = payload;

    bool stale = false;
    if (m_currentPayload.track_id() != trackId.toVariant().toLongLong()) {
        stale = true;
    }

    TrackPointer pTrack = m_pLibrary->trackCollectionManager()->getTrackById(trackId);
    if (pTrack) {
        if (std::abs(pTrack->getDuration() - m_currentPayload.duration_seconds()) > 1.0) {
            stale = true;
        }
        mixxx::BeatsPointer pBeats = pTrack->getBeats();
        bool hasGrid = static_cast<bool>(pBeats);
        if (hasGrid != m_currentPayload.beatgrid_snapshot().available()) {
            stale = true;
        } else if (hasGrid && pBeats->getVersion() != QString::fromStdString(m_currentPayload.beatgrid_snapshot().fingerprint())) {
            stale = true;
        }
    } else {
        stale = true;
    }

    if (!QString::fromStdString(m_currentPayload.analyzer_version()).startsWith(mixxx::scratchsensei::kAnalysisVersionPrefix)) {
        stale = true;
    }

    if (m_pScratchSenseiView) {
        m_pScratchSenseiView->setAnalysisPayload(analysisInfo.data);
        m_pScratchSenseiView->setAnalysisState(stale ? DlgScratchSensei::State::Stale : DlgScratchSensei::State::Analyzed);
    }
}
