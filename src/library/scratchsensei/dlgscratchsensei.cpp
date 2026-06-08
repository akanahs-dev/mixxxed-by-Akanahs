#include "library/scratchsensei/dlgscratchsensei.h"
#include "library/scratchsensei/scratchsenseifeature.h"
#include "library/scratchsensei/wscratchsenseitimeline.h"

#include "moc_dlgscratchsensei.cpp"

#include "ui_dlgscratchsensei.h"
#include <QDebug>
#include <QVBoxLayout>

DlgScratchSensei::DlgScratchSensei(QWidget* parent,
                                   UserSettingsPointer pConfig,
                                   Library* pLibrary,
                                   ScratchSenseiFeature* pFeature,
                                   KeyboardEventFilter* keyboard)
        : QWidget(parent),
          m_pUi(std::make_unique<Ui::DlgScratchSensei>()),
          m_pConfig(pConfig) {
    Q_UNUSED(pLibrary);
    Q_UNUSED(pFeature);
    Q_UNUSED(keyboard);
    m_pUi->setupUi(this);

    m_pTimeline = new WScratchSenseiTimeline(this);
    auto* pLayout = new QVBoxLayout(m_pUi->timelinePlaceholder);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(m_pTimeline);

    connect(m_pTimeline, &WScratchSenseiTimeline::suggestionSelected,
            this, &DlgScratchSensei::onSuggestionSelected);

    connect(m_pUi->btnAnalyze, &QPushButton::clicked, this, &DlgScratchSensei::onAnalyzeClicked);
    connect(m_pUi->btnCancel, &QPushButton::clicked, this, &DlgScratchSensei::onCancelClicked);
    connect(m_pUi->cmbDeckSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DlgScratchSensei::onDeckComboChanged);

    // Detail Action connections
    connect(m_pUi->btnJump, &QPushButton::clicked, this, [this]() {
        if (m_selectedSuggestion.has_time_seconds()) {
            emit jumpRequested(m_selectedSuggestion.time_seconds());
        }
    });
    connect(m_pUi->btnCreateCue, &QPushButton::clicked, this, [this]() {
        if (m_selectedSuggestion.has_id()) {
            emit createCueRequested(QString::fromStdString(m_selectedSuggestion.id()));
        }
    });
    connect(m_pUi->btnPromote, &QPushButton::clicked, this, [this]() {
        if (m_selectedSuggestion.has_id()) {
            emit promoteHotcueRequested(QString::fromStdString(m_selectedSuggestion.id()));
        }
    });
    connect(m_pUi->btnHide, &QPushButton::clicked, this, [this]() {
        if (m_selectedSuggestion.has_id()) {
            QString id = QString::fromStdString(m_selectedSuggestion.id());
            m_hiddenIds.insert(id);
            m_pTimeline->setHiddenSuggestions(m_hiddenIds);
            
            // Deselect
            mixxx::track::io::Suggestion emptySuggestion;
            onSuggestionSelected(emptySuggestion);
        }
    });

    // Disable detail action buttons initially
    mixxx::track::io::Suggestion emptySuggestion;
    onSuggestionSelected(emptySuggestion);

    // Initial state
    setAnalysisState(State::NoDeck);
}

DlgScratchSensei::~DlgScratchSensei() {
}

void DlgScratchSensei::setDeckContexts(const QList<int>& deckIndices, const QStringList& deckDescriptions) {
    m_pUi->cmbDeckSelector->clear();
    for (int i = 0; i < deckIndices.size(); ++i) {
        m_pUi->cmbDeckSelector->addItem(deckDescriptions[i], deckIndices[i]);
    }
}

void DlgScratchSensei::setAnalysisState(State state) {
    m_pUi->btnCancel->setVisible(state == State::Analyzing);
    m_pUi->btnAnalyze->setEnabled(state != State::Analyzing && state != State::NoDeck && state != State::AmbiguousDeck);
    m_pUi->cmbDeckSelector->setVisible(state == State::AmbiguousDeck);
    
    m_pUi->progressBar->setVisible(state == State::Analyzing);
    if (state != State::Analyzing) {
        m_pUi->progressBar->setValue(0);
    }

    if (state == State::Analyzed || state == State::Stale) {
        m_pUi->stackedWidget->setCurrentWidget(m_pUi->pageWorkspace);
    } else {
        m_pUi->stackedWidget->setCurrentWidget(m_pUi->pageStatus);
        
        QString message;
        switch (state) {
            case State::NoDeck:
                message = tr("Load a track to a deck to use Scratch Sensei.");
                m_pUi->lblHeader->setText(tr("No track loaded"));
                break;
            case State::AmbiguousDeck:
                message = tr("Choose which loaded deck to inspect.");
                m_pUi->lblHeader->setText(tr("Multiple tracks loaded"));
                break;
            case State::NotAnalyzed:
                message = tr("Scratch Sensei has not analyzed this track yet.");
                break;
            case State::Analyzing:
                message = tr("Listening for structure, energy, and phrase clues.");
                break;
            case State::Failed:
                message = tr("Scratch Sensei could not analyze this track.");
                break;
            case State::Cancelled:
                message = tr("Analysis cancelled.");
                break;
            default:
                break;
        }
        m_pUi->lblStatusMessage->setText(message);
    }
}

void DlgScratchSensei::setAnalysisPayload(const QByteArray& payload) {
    m_hiddenIds.clear();
    m_pTimeline->setHiddenSuggestions(m_hiddenIds);
    m_pTimeline->setAnalysisPayload(payload);
    
    // Clear details panel
    mixxx::track::io::Suggestion emptySuggestion;
    onSuggestionSelected(emptySuggestion);
}

void DlgScratchSensei::setAnalysisProgress(AnalyzerProgress progress) {
    if (progress >= kAnalyzerProgressNone) {
        m_pUi->progressBar->setValue(analyzerProgressPercent(progress));
    }
}

void DlgScratchSensei::setTrackInfo(TrackId trackId, const QString& title) {
    m_currentTrackId = trackId;
    m_pUi->lblHeader->setText(title);
}

void DlgScratchSensei::onAnalyzeClicked() {
    if (m_currentTrackId.isValid()) {
        emit analyzeRequested(m_currentTrackId);
    }
}

void DlgScratchSensei::onCancelClicked() {
    emit cancelRequested();
}

void DlgScratchSensei::onDeckComboChanged(int index) {
    if (index >= 0) {
        int deckIndex = m_pUi->cmbDeckSelector->itemData(index).toInt();
        emit deckSelected(deckIndex);
    }
}

void DlgScratchSensei::onSuggestionSelected(const mixxx::track::io::Suggestion& suggestion) {
    m_selectedSuggestion = suggestion;
    bool hasSelection = suggestion.has_id();

    m_pUi->btnJump->setEnabled(hasSelection);
    m_pUi->btnCreateCue->setEnabled(hasSelection);
    m_pUi->btnPromote->setEnabled(hasSelection);
    m_pUi->btnHide->setEnabled(hasSelection);

    if (hasSelection) {
        m_pUi->lblSelectedType->setText(tr("Type: %1").arg(QString::fromStdString(suggestion.type())));
        m_pUi->lblSelectedTime->setText(tr("Time: %1s").arg(QString::number(suggestion.time_seconds(), 'f', 2)));
        QString desc = QString::fromStdString(suggestion.label());
        if (suggestion.has_evidence() && suggestion.evidence().has_reason()) {
            desc += " - " + QString::fromStdString(suggestion.evidence().reason());
        }
        m_pUi->lblSelectedDesc->setText(tr("Description: %1").arg(desc));
    } else {
        m_pUi->lblSelectedType->setText(tr("Type: None"));
        m_pUi->lblSelectedTime->setText(tr("Time: -"));
        m_pUi->lblSelectedDesc->setText(tr("Description: Select a suggestion from the timeline."));
    }
}
