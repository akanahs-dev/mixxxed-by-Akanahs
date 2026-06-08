#pragma once

#include <QWidget>
#include <QSet>
#include "preferences/usersettings.h"

#include "track/trackid.h"
#include "analyzer/analyzerprogress.h"
#include "analyzer/scratchsensei/scratchsenseisummary.h"

namespace Ui {
class DlgScratchSensei;
}

class KeyboardEventFilter;
class Library;
class ScratchSenseiFeature;
class WScratchSenseiTimeline;

class DlgScratchSensei : public QWidget {
    Q_OBJECT
  public:
    enum class State {
        NoDeck,
        AmbiguousDeck,
        NotAnalyzed,
        Analyzing,
        Analyzed,
        Stale,
        Failed,
        Cancelled
    };

    DlgScratchSensei(QWidget* parent,
                     UserSettingsPointer pConfig,
                     Library* pLibrary,
                     ScratchSenseiFeature* pFeature,
                     KeyboardEventFilter* keyboard);
    ~DlgScratchSensei() override;

  signals:
    void analyzeRequested(TrackId trackId);
    void cancelRequested();
    void deckSelected(int deckIndex);
    void jumpRequested(double timeSeconds);
    void createCueRequested(QString suggestionId);
    void promoteHotcueRequested(QString suggestionId);

  public slots:
    void setDeckContexts(const QList<int>& deckIndices, const QStringList& deckDescriptions);
    void setAnalysisState(State state);
    void setAnalysisPayload(const QByteArray& payload);
    void setAnalysisProgress(AnalyzerProgress progress);
    void setTrackInfo(TrackId trackId, const QString& title);

  private slots:
    void onAnalyzeClicked();
    void onCancelClicked();
    void onDeckComboChanged(int index);
    void onSuggestionSelected(const mixxx::track::io::Suggestion& suggestion);

  private:
    std::unique_ptr<Ui::DlgScratchSensei> m_pUi;
    UserSettingsPointer m_pConfig;
    TrackId m_currentTrackId;
    WScratchSenseiTimeline* m_pTimeline;
    mixxx::track::io::Suggestion m_selectedSuggestion;
    QSet<QString> m_hiddenIds;
};
