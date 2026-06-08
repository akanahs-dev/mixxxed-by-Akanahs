#pragma once

#include "preferences/usersettings.h"
#include "library/libraryfeature.h"
#include "library/treeitemmodel.h"
#include "util/parented_ptr.h"
#include "analyzer/trackanalysisscheduler.h"
#include "analyzer/scratchsensei/scratchsenseisummary.h"

#include <QList>
#include "track/track_decl.h"
#include "track/trackid.h"

class PlayerManagerInterface;
class DlgScratchSensei;

struct ScratchSenseiDeckContext {
    int deckIndex;
    QString group;
    TrackId trackId;
    QString trackTitle;
    TrackPointer pTrack;
};

class ScratchSenseiFeature final : public LibraryFeature {
    Q_OBJECT
  public:
    ScratchSenseiFeature(Library* pLibrary,
                         UserSettingsPointer pConfig,
                         PlayerManagerInterface* pPlayerManager);
    ~ScratchSenseiFeature() override = default;

    QVariant title() override;

    void bindLibraryWidget(WLibrary* libraryWidget,
                           KeyboardEventFilter* keyboard) override;

    TreeItemModel* sidebarModel() const override;

    QList<ScratchSenseiDeckContext> resolveLoadedDecks() const;

  public slots:
    void activate() override;

  private slots:
    void onAnalyzeRequested(TrackId trackId);
    void onCancelRequested();
    void onDeckSelected(int deckIndex);
    void onTrackAnalysisSchedulerProgress(AnalyzerProgress currentTrackProgress, int currentTrackNumber, int totalTracksCount);
    void onTrackAnalysisSchedulerFinished();

    // Cue safety and timeline jump actions
    void onJumpRequested(double timeSeconds);
    void onCreateCueRequested(QString suggestionId);
    void onPromoteHotcueRequested(QString suggestionId);

  private:
    void loadAnalysisResult(TrackId trackId);
    void updateDeckContexts();

    PlayerManagerInterface* const m_pPlayerManager;
    parented_ptr<TreeItemModel> m_pSidebarModel;
    DlgScratchSensei* m_pScratchSenseiView;
    TrackAnalysisScheduler::Pointer m_pTrackAnalysisScheduler;
    TrackId m_currentTrackId;
    int m_currentDeckIndex;
    mixxx::track::io::ScratchSenseiTrackAnalysis m_currentPayload;
};
