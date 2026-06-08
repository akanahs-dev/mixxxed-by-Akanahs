#pragma once

#include <QWidget>
#include <QSet>
#include <QByteArray>
#include "analyzer/scratchsensei/scratchsenseisummary.h"

class WScratchSenseiTimeline : public QWidget {
    Q_OBJECT
  public:
    explicit WScratchSenseiTimeline(QWidget* pParent = nullptr);
    ~WScratchSenseiTimeline() override = default;

    void setAnalysisPayload(const QByteArray& payload);
    void setHiddenSuggestions(const QSet<QString>& hiddenIds);

  signals:
    void suggestionSelected(const mixxx::track::io::Suggestion& suggestion);

  protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private:
    mixxx::track::io::ScratchSenseiTrackAnalysis m_payload;
    bool m_hasPayload;
    QSet<QString> m_hiddenIds;
    int m_hoveredSuggestionIndex;
    int m_selectedSuggestionIndex;
    
    QRect energyCurveRect() const;
    int suggestionAtPos(const QPoint& pos) const;
};
